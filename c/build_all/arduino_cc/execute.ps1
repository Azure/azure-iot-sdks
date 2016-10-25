# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

<#
	This script uploads a binary on Arduino Huzzah (esp8266) and monitor its serial for some time, looking for issues.
	It will throw an exception if Arduino send Panic, Abort, reset, or Exception to the serial, or if it send a lower than the minimum heap size.
#>

param (
    [Parameter(Mandatory=$true)]
	[ValidateNotNullOrEmpty()]
    [string] $binaryPath,

    [Parameter(Mandatory=$true)]
	[ValidateNotNullOrEmpty()]
    [string] $serialPort,

    [Parameter(Mandatory=$true)]
	[ValidateNotNullOrEmpty()]
    [string] $esptool,

    [Parameter(Mandatory=$false)]
	[ValidateNotNullOrEmpty()]
    [int] $logLines=300,

    [Parameter(Mandatory=$false)]
	[ValidateNotNullOrEmpty()]
    [int] $maxFailed=10,

    [Parameter(Mandatory=$false)]
	[ValidateNotNullOrEmpty()]
    [int] $minimumHeap=22000,

    [Parameter(Mandatory=$false)]
	[ValidateSet("75", "110", "300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200")]
    [string] $serialBW="115200"
)

$installcmd = "$esptool -vv -cd nodemcu -cb $serialBW -cp $serialPort -ca 0x00000 -cf $binaryPath"

Write-Output ""
Write-Output "Upload the binaries to the hardware with command:"
Write-Output $installcmd
Write-Output ""

Invoke-Expression $installcmd

Write-Output ""
Write-Output "Monitor the hardware via USB/Serial:"
Write-Output ""

$port = New-Object System.IO.Ports.SerialPort $serialPort,$serialBW,None,8,one
try
{
    $port.Open()
    while($logLines--) 
    { 
        $result = $port.ReadLine()
        Write-Output $result
	    if($result.ToLower().Contains("panic"))
	    {
            throw("Arduino throw a panic")
	    }
	    if($result.ToLower().Contains("failed"))
	    {
            if(($maxFailed--) -eq 0)
            {
                throw("Arduino ran with fail")
            }
	    }
	    if($result.ToLower().Contains("abort"))
	    {
            $port.Close()
            throw("Arduino aborted")
	    }
	    if($result.ToLower().Contains("reset"))
	    {
            $port.Close()
            throw("Arduino reseted")
	    }
	    if($result.ToLower().Contains("exception"))
	    {
            $port.Close()
            throw("Arduino throw an exception")
	    }
	    $keyVal=$result.split(":")
	    if(($keyVal[0].ToLower().Contains("heap")) -and ([convert]::ToInt32($keyVal[1]) -le $minimumHeap))
	    {
            $port.Close()
		    throw("Out of memory")
	    }
    }
}
catch
{
    throw 
}
finally
{
    $port.Close()
}
