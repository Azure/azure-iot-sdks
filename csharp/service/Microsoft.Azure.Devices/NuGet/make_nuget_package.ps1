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

$dotNetFile = "..\Properties\AssemblyInfo.cs"

# Delete existing packages to force rebuild
ls Microsoft.Azure.Devices.*.nupkg | % { del $_ }

$v1 = GetAssemblyVersionFromFile($dotNetFile)
$id='Microsoft.Azure.Devices'

echo "Creating NuGet package $id version $v1"

.\NuGet.exe pack "$id.nuspec" -Prop Configuration=Release -Prop Version=$v1