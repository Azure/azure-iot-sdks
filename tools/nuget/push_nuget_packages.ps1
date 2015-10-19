# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.
$pkg_source=$args[0]
$feed_path=$args[1]
$api_key=$args[2]

if(!$pkg_source -Or !$feed_path) {
	Write-Host ""
	Write-Host "Missing required input(s)" -foregroundcolor "red"
	Write-Host ""
	Write-Host ""
	Write-Host "Usage: push_nuget_packages <packages_path> <nuget_feed_path> [api_Key]"  -foregroundcolor "green"
	Write-Host ""
	Write-Host ""
	return
}

if (-Not (Test-Path 'NuGet.exe')) {
    Invoke-WebRequest 'https://nuget.org/nuget.exe' -OutFile 'NuGet.exe'
}

if(!$api_Key) {
	.\NuGet.exe push "$pkg_source\*.nupkg" -Source "$feed_path"
}
Else {
	.\NuGet.exe push "$pkg_source\*.nupkg" -Source "$feed_path" -ApiKey "$api_Key"
}
