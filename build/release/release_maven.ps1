# Constants
$pom_url = "http://azure.github.io/azure-iot-sdks/"
$pom_scm_connection = "scm:git:git@github.com:Azure/azure-iot-sdks.git"
$pom_scm_url = "https://github.com/Azure/azure-iot-sdks.git"

$pom_license_name = "MIT License"
$pom_license_url = "http://opensource.org/licenses/MIT"
$pom_license_distribution = "repo"

$maven_plugin_groupId = "org.apache.maven.plugins"
$maven_compiler_plugin_artifactId = "maven-compiler-plugin"
$maven_compiler_plugin_version = "3.3"
$maven_source_plugin_artifactId = "maven-source-plugin"
$maven_javadoc_plugin_artifactId = "maven-javadoc-plugin"
$maven_javadoc_plugin_version = "2.10.3"
$java_version_service = "1.8"
$java_version_client = "1.7"
$java_version_websocket_layer = "1.7"

$sourcesPath = "src\main\java\com\microsoft\azure\iothub"
$testsPath = "src\test\java\tests\unit\com\microsoft\azure\iothub"

# Set location variables
$currentLocation = Get-Location
$currentScriptLocation = $PSScriptRoot
$mavenDir = (join-path $currentScriptLocation Maven)
$mavenArtifactsDir = (join-path $mavenDir MavenArtifacts)
$copyLocation = (join-path $mavenDir _temp)
$cleanClassFilesLocation = (join-path $mavenDir CleanClassFiles)
$javaDeviceClientLocation = (join-path (join-path (join-path (split-path (split-path $currentScriptLocation)) java) device) iothub-java-client)
$javaServiceClientLocation = (join-path (join-path (join-path (split-path (split-path $currentScriptLocation)) java) service) iothub-service-sdk)
$websocketTransportLayerLocation = (join-path (join-path (split-path (split-path $currentScriptLocation)) java) websocket-transport-layer)
#$jarsCopyLocation = (join-path $mavenArtifactsDir UnsignedJars)

function Is-Null
{
    if(!$args[0]){
        return $FALSE;
    }
    return $TRUE;
}

function Release-Client
{
	param([string]$clientLocation, [string]$groupId, [string]$artifactId, [string]$java_version)
	$clientDirectoryName = (split-path $clientLocation -leaf)
	$jarsCopyLocation = (join-path (join-path $mavenArtifactsDir "$clientDirectoryName") "UnsignedJars")

	# Copy directory to temporary locatation
	if(Test-Path -Path "$copyLocation\$clientDirectoryName"){
		echo "Cleaning directory first"
		Remove-Item -Recurse -Force "$copyLocation\$clientDirectoryName"
	}
	echo "Copy java client directory..."
	Copy-Item $clientLocation $copyLocation -recurse

	$temporaryClient = (join-path $copyLocation $clientDirectoryName)

	# Copy license and third party notice files
	$resourcesCopyLocation = "$temporaryClient\src\main\resources"
	if(!(Test-Path -Path $resourcesCopyLocation)){
		New-Item -ItemType directory -Path $resourcesCopyLocation
	}

	echo "Copying license and third party notice files"
	Copy-Item -Force "$cleanClassFilesLocation\LICENSE.txt" $resourcesCopyLocation
	Copy-Item -Force "$cleanClassFilesLocation\thirdpartynotice.txt" $resourcesCopyLocation

	# Modify pom file
	echo "Modifying pom"
	$xmlPath = $temporaryClient + "\pom.xml"
	$xmlStr = Get-Content $xmlPath
	[xml]$xml = $xmlStr
	$hasParent = Is-Null $xml.project.SelectSingleNode('parent')

	# Save attributes we will need
	if($hasParent){
		$version = $xml.project.parent.version
	} else {
		$version = $xml.project.version
	}
	$name = $xml.project.name
	$pom_description = $xml.project.description
	$dependencies = $xml.SelectNodes('/project/dependencies/dependency')

	# Remove "parent" node and "repositories" node
	echo "Removing Parent and Repositories nodes"
    if($hasParent){
        $parentNode = $xml.project.SelectSingleNode('parent')
        $xml.project.RemoveChild($parentNode) | Out-Null
    }

	# Iterate over entire POM inserting nodes in appropriate places
	$nodes = $xml.project.ChildNodes
	$nodes | % {
		if($_.LocalName -eq "modelVersion" -and $hasParent){
			$element = $xml.CreateElement('groupId')
			$element.InnerText = $groupId
			$xml.project.InsertAfter($element, $_) | Out-Null
		}
		elseif($_.LocalName -eq "artifactId" -and $hasParent){
			$element = $xml.CreateElement('version')
			$element.InnerText = $version
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

	#forcible add group id and artifact id
	$groupIdNode = $xml.project.SelectSingleNode('groupId')
	$artifactIdNode = $xml.project.SelectSingleNode('artifactId')
	$groupIdNode.InnerText = $groupId
	$artifactIdNode.InnerText = $artifactId

	$plugin_groupId = $xml.CreateElement('groupId')
	$plugin_groupId.InnerText = $maven_plugin_groupId

	# Add the compiler plugin as the first plugin
	$element = $xml.CreateElement('plugin')
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

	$plugin_executions = $xml.CreateElement('executions')
	$plugin_execution = $xml.CreateElement('execution')
	$plugin_phase = $xml.CreateElement('phase')
	$plugin_phase.InnerText = 'package'
	$plugin_goals = $xml.CreateElement('goals')
	$plugin_goal = $xml.CreateElement('goal')
	$plugin_goal.InnerText = 'jar'
	$plugin_goals.AppendChild($plugin_goal) | Out-Null
	$plugin_execution.AppendChild($plugin_phase) | Out-Null
	$plugin_execution.AppendChild($plugin_goals) | Out-Null
	$plugin_executions.AppendChild($plugin_execution) | Out-Null

	$sources = $xml.CreateElement('plugin')
	$plugin_artifactId = $xml.CreateElement('artifactId')
	$plugin_artifactId.InnerText = $maven_source_plugin_artifactId
	$sources.AppendChild($plugin_groupId.Clone()) | Out-Null
	$sources.AppendChild($plugin_artifactId.Clone()) | Out-Null
	$sources.AppendChild($plugin_executions.Clone()) | Out-Null

	$xml.project.build.plugins.AppendChild($sources) | Out-Null

	$javadoc = $xml.CreateElement('plugin')
	$plugin_artifactId = $xml.CreateElement('artifactId')
	$plugin_artifactId.InnerText = $maven_javadoc_plugin_artifactId
	$plugin_version = $xml.CreateElement('version')
	$plugin_version.InnerText = $maven_javadoc_plugin_version
	$javadoc.AppendChild($plugin_groupId.Clone()) | Out-Null
	$javadoc.AppendChild($plugin_artifactId.Clone()) | Out-Null
	$javadoc.AppendChild($plugin_version.Clone()) | Out-Null
	$javadoc.AppendChild($plugin_executions.Clone()) | Out-Null

	$xml.project.build.plugins.AppendChild($javadoc) | Out-Null

	$xml.Save($xmlPath)

	# Attempt to build
	Set-Location $temporaryClient
	$cmd = "mvn clean install -DskipTests"
	iex $cmd

	if($LastExitCode -ne 0){
		Write-Host "Error building $clientDirectoryName. Exiting..." -f red;
		Set-Location $currentLocation
		exit $LastExitCode
	}

	# Copy jars to main Maven directory
	if(!(Test-Path -Path $jarsCopyLocation)){
		New-Item -ItemType directory -Path $jarsCopyLocation
	}

	if(Test-Path -Path "$temporaryClient\target\$artifactId-$version-with-deps.jar"){
		Remove-Item -Force "$temporaryClient\target\$artifactId-$version-with-deps.jar"
	}
	Copy-Item -Force "$temporaryClient\target\*.jar" $jarsCopyLocation

	# Modify publishing pom file and copy
	# Copy parent pom first and then make changes
	Copy-Item -Force "$cleanClassFilesLocation\parent.pom" (join-path (split-path $jarsCopyLocation) "$artifactId-$version.pom")

	$xmlPath = (join-path (split-path $jarsCopyLocation) "$artifactId-$version.pom")
	$xmlStr = Get-Content $xmlPath
	[xml]$xml = $xmlStr

	# Copy over dependencies
	$dependenciesNode = $xml.project.SelectSingleNode('dependencies')
	$dependencies | % {
        $node = $xml.ImportNode($_, $TRUE)
		$dependenciesNode.AppendChild($node) | Out-Null
	}

	$xml.project.SelectSingleNode('groupId').InnerText = $groupId
	$xml.project.SelectSingleNode('artifactId').InnerText = $artifactId
	$xml.project.SelectSingleNode('version').InnerText = $version
	$xml.project.SelectSingleNode('name').InnerText = $name
	$xml.project.SelectSingleNode('description').InnerText = $pom_description
	$xml.Save($xmlPath)
}

if(!(Test-Path -Path $copyLocation)){
	echo "Creating temporary directory $copyLocation"
	New-Item -ItemType directory -Path $copyLocation
}

Release-Client $javaDeviceClientLocation "com.microsoft.azure.iothub-java-client" "iothub-java-device-client" $java_version_client
Release-Client $javaServiceClientLocation "com.microsoft.azure.iothub-java-client" "iothub-java-service-client" $java_version_service
Release-Client $websocketTransportLayerLocation "com.microsoft.azure.iothub-java-client" "websocket-transport-layer" $java_version_websocket_layer

Set-Location $currentLocation

Remove-Item -Recurse -Force "$copyLocation"

# Point to location of jars
Write-Host "If successful, jars located here: $mavenArtifactsDir\<project_name>\UnsignedJars\. " -f green;
