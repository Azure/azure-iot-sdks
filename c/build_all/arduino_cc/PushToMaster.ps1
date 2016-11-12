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

#-------------------------------------------------------
# Clone the github repo in the specified directory
function CloneDirectory($gitUrl, $dst)
{   
    Write-Host "clone $gitUrl into $dst"
    
    if([System.IO.Directory]::Exists($dst))
    {
        throw "Directory $dst already exists. Delete it before try to publish a new Arduino library."
    }

    git clone "$gitUrl" "$dst" 
} 

#-------------------------------------------------------
# Update directory
function UpdateDirectory($workPath, $src, $dst)
{   
    Write-Host ""
    Write-Host "Update directory $dst"
    
    $gitSrc = Join-Path "$dst" ".git"
    $gitDst = Join-Path "$workPath" ".git"

    Move-Item "$gitSrc" "$gitDst"
    robocopy "$src" "$dst" /MIR
    Move-Item "$gitDst" "$gitSrc"

    Write-Host ""
} 

#-------------------------------------------------------
# Upload directory to github repo
function UploadDirectory($gitName, $dst)
{   
    Write-Host ""
    Write-Host "Upload directory $dst"
    
    pushd "$dst"

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
        Write-Host "Publishing $dst on $gitName"
        Write-Host ""

        git commit -m "Sync the Arduino Library $gitName with the latest azure-iot-sdks version"

        git push origin master
    }
    else
    {
        Write-Host "New Library on $dst was not published. You shall publish it manually or rerun this script."
    }
    
    popd

    Write-Host ""
} 

#-------------------------------------------------------
# Upload directory to github repo
function PublishDirectory($workPath, $relativeLibPath, $gitName)
{   
    $src = Join-Path "$workPath" "$relativeLibPath";
    $dst = Join-Path "$workPath" "$gitName";
    $gitUrl = "https://github.com/Azure/"+"$gitName"+".git"
    
    Write-Host ""
    Write-Host "PRUBLISH $gitName"
    Write-Host "       Source directory: $src"
    Write-Host "  Destination directory: $dst"
    Write-Host "             Github URL: $gitUrl"
    Write-Host ""
    CloneDirectory -gitUrl "$gitUrl" -dst "$dst"
    UpdateDirectory -workPath "$workPath" -src "$src" -dst "$dst"
    UploadDirectory -gitName $gitName -dst "$dst"
    Write-Host ""
    Write-Host ""
}


PublishDirectory -workPath "$workPath" -relativeLibPath "arduino\libraries\AzureIoTHub" -gitName "azure-iot-arduino"
PublishDirectory -workPath "$workPath" -relativeLibPath "arduino\libraries\AzureIoTUtility" -gitName "azure-iot-arduino-utility"
PublishDirectory -workPath "$workPath" -relativeLibPath "arduino\libraries\AzureIoTProtocol_HTTP" -gitName "azure-iot-arduino-protocol-http"

