# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

<#
    This script pushes the Arduino library to the Master branch of the Azure\azure-iot-arduino.
#>


param (
    [Parameter(Mandatory=$true)]
	[ValidateNotNullOrEmpty()]
    [string] $workPath
)

$libSrcPath = Join-Path "$workPath" "arduino\libraries\AzureIoTHub";
$libDstPath = Join-Path "$workPath" "azure-iot-arduino";

$gitSrc = Join-Path "$libDstPath" ".git"
$gitDst = Join-Path "$workPath" ".git"

if([System.IO.Directory]::Exists($libDstPath))
{
    throw "Directory $libDstPath already exists. Delete it before try to publish a new Arduino library."
}

git clone "https://github.com/Azure/azure-iot-arduino.git" "$libDstPath" 

Move-Item "$gitSrc" "$gitDst"
robocopy "$libSrcPath" "$libDstPath" /MIR
Move-Item "$gitDst" "$gitSrc"

pushd "$libDstPath"

Write-Host ""
Write-Host "Comparing the new library with the last version in Github, these are the changes…"
Write-Host ""
git add .
git status
Write-Host ""

$mustPush = Read-Host 'Take a look at the changes... Shall we publish it (y/n)?'

if($mustPush.ToLower() -eq "y")
{
    Write-Host ""
    Write-Host "Publishing $libDstPath"
    Write-Host ""
    git commit -m "Sync the Arduino Library azure-iot-arduino with the latest azure-iot-sdks version"

    git push origin master
}
else
{
    Write-Host "New Library on $libDstPath was not published. You shall publish it manually or rerun this script."
}
popd