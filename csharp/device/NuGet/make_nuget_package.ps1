# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

function GetAssemblyVersionFromFile($filename) {
    $regex = 'AssemblyInformationalVersion\("(\d{1,3}\.\d{1,3}\.\d{1,3}(?:-[A-Za-z0-9-]+)?)"\)'
    $values = select-string -Path $filename -Pattern $regex | % { $_.Matches } | % { $_.Groups } | % { $_.Value }
    if( $values.Count -eq 2 ) {
        return $values[1]
    }
    Write-Host "Error: Unable to find AssemblyInformationalVersion in $filename" -foregroundcolor "red"
    exit
}

if (-Not (Test-Path 'NuGet.exe')) {
    Invoke-WebRequest 'https://nuget.org/nuget.exe' -OutFile 'NuGet.exe'
}

# Delete existing packages to force rebuild
ls Microsoft.Azure.Devices.Client.*.nupkg | % { del $_ }

# Get the assembly versions from both files, make sure they match, and use that as the package version
$dotNetFile = "..\Microsoft.Azure.Devices.Client\Properties\AssemblyInfo.cs"
$winRTNetFile = "..\Microsoft.Azure.Devices.Client.WinRT\Properties\AssemblyInfo.cs"

$v1 = GetAssemblyVersionFromFile($dotNetFile)
$v2 = GetAssemblyVersionFromFile($winRTNetFile)

if($v1 -ne $v2) {
    Write-Host "Error: Mismatching assembly versions in files $dotNetFile and $winRTNetFile. Check AssemblyInformationalVersion attribute in each file." -foregroundcolor "red"
    return
}

$id='Microsoft.Azure.Devices.Client'

echo "Creating NuGet package $id version $v1"

.\NuGet.exe pack "$id.nuspec" -Prop Configuration=Release -Prop id=$id -Prop Version=$v1