# Constants
$pom_description = "The Microsoft Azure IoT Device SDK for Java"
$pom_url = "http://azure.github.io/azure-iot-sdks/"
$pom_scm_connection = "scm:git:git@github.com:Azure/azure-iot-sdks.git"
$pom_scm_url = "https://github.com/Azure/azure-iot-sdks.git"

$pom_license_name = "MIT License"
$pom_license_url = "http://opensource.org/licenses/MIT"
$pom_license_distribution = "repo"

$maven_plugin_groupId = "org.apache.maven.plugins"
$maven_compiler_plugin_artifactId = "maven-compiler-plugin"
$maven_compiler_plugin_version = "3.3"
$java_version = "1.8"

$sourcesPath = "src\main\java\com\microsoft\azure\iothub"
$testsPath = "src\test\java\tests\unit\com\microsoft\azure\iothub"

# Set location variables
$currentLocation = Get-Location
$currentScriptLocation = $PSScriptRoot
$mavenDir = (join-path $currentScriptLocation Maven)
$copyLocation = (join-path $mavenDir _temp)
$cleanClassFilesLocation = (join-path $mavenDir CleanClassFiles)
$javaDeviceClientLocation = (join-path (join-path (join-path (split-path (split-path $currentScriptLocation)) java) device) iothub-java-client)

if(!(Test-Path -Path $copyLocation)){
	echo "Creating temporary directory $copyLocation"
	New-Item -ItemType directory -Path $copyLocation
}

# Copy directory to temporary location
echo "Cleaning directory first, if necessary"
if(Test-Path -Path "$copyLocation\iothub-java-client"){
	Remove-Item -Recurse -Force "$copyLocation\iothub-java-client"
}
echo "Copying java client directory..."
Copy-Item $javaDeviceClientLocation $copyLocation -recurse

$temporaryClient = (join-path $copyLocation iothub-java-client)

echo "Removing MQTT from project"
# Remove mqtt directory
Remove-Item -Recurse -Force "$temporaryClient\$sourcesPath\transport\mqtt"
# Remove mqtt test directory
Remove-Item -Recurse -Force "$temporaryClient\$testsPath\transport\mqtt"

echo "Copying clean old java files"
Copy-Item -Force "$cleanClassFilesLocation\$sourcesPath\*" "$temporaryClient\$sourcesPath\"
Copy-Item -Force "$cleanClassFilesLocation\$testsPath\*" "$temporaryClient\$testsPath\"

# Remove mqtt protocol from IotHubClientProtocol.java
(get-content "$temporaryClient\$sourcesPath\IotHubClientProtocol.java") | Foreach-Object {$_ -replace ", MQTT", ""} | set-content "$temporaryClient\$sourcesPath\IotHubClientProtocol.java"

# Copy license and third party notice files
$resourcesCopyLocation = "$temporaryClient\src\main\resources"
if(!(Test-Path -Path $resourcesCopyLocation)){
	New-Item -ItemType directory -Path $resourcesCopyLocation
}

echo "Copying license and third party notice files"
Copy-Item -Force "$cleanClassFilesLocation\LICENSE.txt" $resourcesCopyLocation
Copy-Item -Force "$cleanClassFilesLocation\thirdpartynotice.txt" $resourcesCopyLocation

# Modify pom file
echo "Removing MQTT from pom files and modifying pom"
$xmlPath = $temporaryClient + "\pom.xml"
$xmlStr = Get-Content $xmlPath
[xml]$xml = $xmlStr

# Save attributes we will need
$groupId = $xml.project.parent.groupId
$version = $xml.project.parent.version
$artifactId = $xml.project.artifactId

# Remove "parent" node and "repositories" node
echo "Removing Parent and Repositories nodes"
$parentNode = $xml.project.SelectSingleNode('parent')
$repositoriesNode = $xml.project.SelectSingleNode('repositories')
$xml.project.RemoveChild($parentNode) | Out-Null
$xml.project.RemoveChild($repositoriesNode) | Out-Null

# Iterate through dependencies and find the one to remove
echo "Removing paho dependencies"
$dependencies = $xml.SelectNodes('/project/dependencies/dependency')
$dependencies | % {
	$d_groupId = $_.groupId
	if($d_groupId -eq "org.eclipse.paho"){
		$xml.project.dependencies.RemoveChild($_) | Out-Null
		# break
	}
}

# Iterate over entire POM inserting nodes in appropriate places
$nodes = $xml.project.ChildNodes
$nodes | % {
	if($_.LocalName -eq "modelVersion"){
		$element = $xml.CreateElement('groupId')
		$element.InnerText = $groupId
		$xml.project.InsertAfter($element, $_) | Out-Null
	}
	elseif($_.LocalName -eq "artifactId"){
		$element = $xml.CreateElement('version')
		$element.InnerText = $version
		$xml.project.InsertAfter($element, $_) | Out-Null
	}
	elseif($_.LocalName -eq "name"){
		$element = $xml.CreateElement('description')
		$element.InnerText = $pom_description
		$xml.project.InsertAfter($element, $_) | Out-Null
	}
	elseif($_.LocalName -eq "description"){
		$element = $xml.CreateElement('url')
		$element.InnerText = $pom_url
		$xml.project.InsertAfter($element, $_) | Out-Null
	}
	elseif($_.LocalName -eq "developers"){
		$element = $xml.CreateElement('licenses')
		$license = $xml.CreateElement('license')
		$license_name = $xml.CreateElement('name')
		$license_name.InnerText = $pom_license_name
		$license_url = $xml.CreateElement('url')
		$license_url.InnerText = $pom_license_url
		$license_distribution = $xml.CreateElement('distribution')
		$license_distribution.InnerText = $pom_license_distribution
		$license.AppendChild($license_name) | Out-Null
		$license.AppendChild($license_url) | Out-Null
		$license.AppendChild($license_distribution) | Out-Null
		$element.AppendChild($license) | Out-Null
		$xml.project.InsertAfter($element, $_) | Out-Null
	}
	elseif($_.LocalName -eq "licenses"){
		$element = $xml.CreateElement('scm')
		$scm_connection = $xml.CreateElement('connection')
		$scm_connection.InnerText = $pom_scm_connection
		$scm_dev_connection = $xml.CreateElement('developerConnection')
		$scm_dev_connection.InnerText = $pom_scm_connection
		$scm_url = $xml.CreateElement('url')
		$scm_url.InnerText = $pom_scm_url
		$element.AppendChild($scm_connection) | Out-Null
		$element.AppendChild($scm_dev_connection) | Out-Null
		$element.AppendChild($scm_url) | Out-Null
		$xml.project.InsertAfter($element, $_) | Out-Null
	}
}

# Add the compiler plugin as the first plugin
$element = $xml.CreateElement('plugin')
$plugin_groupId = $xml.CreateElement('groupId')
$plugin_groupId.InnerText = $maven_plugin_groupId
$plugin_artifactId = $xml.CreateElement('artifactId')
$plugin_artifactId.InnerText = $maven_compiler_plugin_artifactId
$plugin_version = $xml.CreateElement('version')
$plugin_version.InnerText = $maven_compiler_plugin_version
$plugin_config = $xml.CreateElement('configuration')
$config_source = $xml.CreateElement('source')
$config_target = $xml.CreateElement('target')
$config_source.InnerText = $config_target.InnerText = $java_version
$plugin_config.AppendChild($config_source) | Out-Null
$plugin_config.AppendChild($config_target) | Out-Null
$element.AppendChild($plugin_groupId) | Out-Null
$element.AppendChild($plugin_artifactId) | Out-Null
$element.AppendChild($plugin_version) | Out-Null
$element.AppendChild($plugin_config) | Out-Null
$xml.project.build.plugins.PrependChild($element) | Out-Null


$xml.Save($xmlPath)

# Attempt to build
Set-Location $temporaryClient
$cmd = "mvn clean install -DskipTests"
iex $cmd
Set-Location $currentLocation

# Copy jars to main Maven directory
$jarsCopyLocation = (join-path (join-path $mavenDir MavenArtifacts) UnsignedJars)
if(!(Test-Path -Path $jarsCopyLocation)){
	New-Item -ItemType directory -Path $jarsCopyLocation
}

Remove-Item -Force "$temporaryClient\target\$artifactId-$version-with-deps.jar"
Copy-Item -Force "$temporaryClient\target\*.jar" $jarsCopyLocation
Remove-Item -Recurse -Force "$copyLocation"

# Modify publishing pom file and copy
$xmlPath = $cleanClassFilesLocation + "\parent.pom"
$xmlStr = Get-Content $xmlPath
[xml]$xml = $xmlStr

$xml.project.SelectSingleNode('version').InnerText = $version
$xml.Save($xmlPath)


Copy-Item -Force "$cleanClassFilesLocation\parent.pom" (join-path (split-path $jarsCopyLocation) "$artifactId-$version.pom")

# Point to location of jars
Write-Host "If successful, jars located here: $jarsCopyLocation. " -f green;