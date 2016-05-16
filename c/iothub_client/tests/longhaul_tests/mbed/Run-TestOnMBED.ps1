PARAM (
	$TestBinaryFilePath = $null, 
	$MBEDUsbDriveLetter = $null, 
	$Port = $(throw "Parameter 'Port' must be provided."), 
	$BaudRate = 9600,
	$EventHubListenTimeoutInSeconds = 10,
	$SerialPortBinPath = $null,
	$NugetDependenciesPackageConfig = "..\..\..\..\..\tools\DeviceExplorer\DeviceExplorer\packages.config")

Import-Module ".\MBED.psm1";

$ScriptStates = New-Enum @{ 
	"Initializing" = 0;
	"DeployingTestBinary" = 1;
    "ConnectingToDevice" = 2;
    "ResetingDevice" = 3;
    "ProvidingParameters" = 4; 
    "WaitingForTestCompletion" = 5;
    "Terminating" = 6 
}

$ReturnCodes = New-Enum @{ 
    "None" = -1;
    "Success" = 0;
    "InvalidArgument" = 5;
	"DeploymentFailed" = 8;
    "InitialConnectionFailed" = 10;
	"FailedConnectingToEventHub" = 15;
    "TestFailed" = 20;
    "TestTimedOut" = 30;
}

$ReturnCode = $ReturnCodes.None;


# Begin

$ScriptState = $ScriptStates.Initializing;


# Loading dependency binaries

Write-Message "Loading dependencies through nuget."

if ((Get-Command nuget -ErrorAction SilentlyContinue) -EQ $null)
{
	Write-Message -Error "Please install nuget.exe and add it to the PATH environment var.";
	return $ReturnCodes.InvalidArgument;
}

nuget install -OutputDirectory .\packages $NugetDependenciesPackageConfig
$packages = dir .\packages\ -Include *.dll -Recurse;
$packages | %{[System.Reflection.Assembly]::LoadFrom( $_ )};


if ([String]::IsNullOrWhiteSpace($SerialPortBinPath) -OR (Test-Path $SerialPortBinPath) -EQ $false)
{
	Write-Message -Error "Please provide a valid path to SerialPort binary.";
	return $ReturnCodes.InvalidArgument;
}


Write-Message "Loading SerialPort library."

$SerialPortBinPath = Resolve-Path $SerialPortBinPath;
[System.Reflection.Assembly]::LoadFrom($SerialPortBinPath);


# Copying test binary to device.

if ($TestBinaryFilePath -NE $null)
{
	$ScriptState = $ScriptStates.DeployingTestBinary

	if (!(Test-Path $TestBinaryFilePath))
	{
		Write-Message -Error "Could not find test binary file on path '$TestBinaryFilePath'.";
		
		return $ReturnCodes.InvalidArgument;
	}
	
	$MBEDUsbDrivePath = "${MBEDUsbDriveLetter}:\";
	
	if ($MBEDUsbDriveLetter -EQ $null -OR (!(Test-Path $MBEDUsbDrivePath)))
	{
		Write-Message -Error "Invalid MBED USB drive path '$MBEDUsbDrivePath' (drive letter '$MBEDUsbDriveLetter' is invalid or incorrect).";
		
		return $ReturnCodes.InvalidArgument;
	}
	
	Write-Message "Copying test binary '$TestBinaryFilePath' to MBED device drive '$MBEDUsbDrivePath'";
	
	copy "$TestBinaryFilePath" "$MBEDUsbDrivePath" -ErrorVariable CopyErrors -Verbose;
	
	if ($CopyErrors.Count -NE 0)
	{
		White-Message -Error "Failed copying test binary '$TestBinaryFilePath' to MBED device drive '$MBEDUsbDrivePath'";
		
		return $ReturnCodes.DeploymentFailed;
	}
	
	Write-Message "Test binary deployed successfully on device.";
	
	# Waiting for device to get ready for Serial Port connection.
	Sleep 5 
}
else
{
	Write-Message -Warning "Test binary not provided. Skipping deployment on MBED device.";
}


# Connecting to Device

Write-Message "Connecting to device through serial port '$Port'.";

$ScriptState = $ScriptStates.ConnectingToDevice;

$SerialPort = New-Object Azure.IoT.Internal.Tools.SerialPort -ArgumentList $Port;

$void = $SerialPort.Open();

if (!($SerialPort.IsOpen))
{
	Write-Message -Error "Failed opening connection with serial port '$Port'";

	return $ReturnCodes.InitialConnectionFailed;
}

$SerialPort.SetDefaultTimeouts();
$State = $SerialPort.GetState();
$State.BaudRate = $BaudRate;
$void = $SerialPort.SetState($State);


# Reseting Device
Write-Message "Resetting device.";

$ScriptState = $ScriptStates.ResetingDevice;

$void = $SerialPort.SetBreakState();
Sleep 3;
$void = $SerialPort.ClearBreakState();


# Managing test execution.

Write-Message "Starting test control logic.";

$EventHubListener = New-IoTHubEventListener -EventHubConnectionString $env:IOTHUB_EVENTHUB_CONNECTION_STRING -EventHubListenName $env:IOTHUB_EVENTHUB_LISTEN_NAME -EventHubConsumerGroup $env:IOTHUB_EVENTHUB_CONSUMER_GROUP -DeviceId $env:IOTHUB_DEVICE_ID

if ($EventHubListener -EQ $null)
{
	Write-Message -Error "Failed creating a Event Hub listener";
	$ReturnCode = $ReturnCodes.FailedConnectingToEventHub;
	$ScriptState = $ScriptStates.Terminating;
}
else
{
	$ScriptState = $ScriptStates.ProvidingParameters;

	$MBEDMessageList = New-List;
	$MBEDMessageBuffer = "";
	$EventsPendingVerification = New-List;

	# There are 2 distinct timeout mechanisms implemented below.
	# A. One for the overall test execution;
	#    To prevent the script from waiting forever for the test to end;
	#    This timeout is applicable to the script, the test might continue running forever on the device.
	# B. One for each iteration between MBED<->SCRIPT (keep-alive mechanism).
	#    Tracked for each cycle of MBED sending the event, and requesting to this SCRIPT to verify it on the server.
	#    This is to cause this SCRIPT to fail fast if it is heading to a failure.
	
	$ExtraTimeoutInSeconds = 300;
	$TimeoutInSeconds = 120; # Default value. It will be updated once the script receives the duration from the test running on MBED.
	$InitialTime = Get-Date;
	$SleepTime = 2;
	$MaxEventReceivedWaitTime = 180; # seconds

	$KeepAliveTimeoutInSeconds = 120;
	$LastKeepAliveTime = Get-Date;
	$TrackKeepAlives = $false;

	while ($ScriptState -NE $ScriptStates.Terminating)
	{
		if (Read-MBEDMessages -SerialPort $SerialPort -MessageBuffer ([REF]$MBEDMessageBuffer) -MessageList $MBEDMessageList)
		{
			while ($MBEDMessageList.Count -GT 0)
			{
				$Message = $MBEDMessageList.RemoveAt(0);

				Write-Message "MBED:[$Message].";

				if ($Message -EQ "" -OR $Message -IMATCH "^[`t`r`n ]+$") 
				{ 
					continue; 
				}

				if ($Message -IMATCH "Starting Long Haul tests ")
				{
					if ($Message -IMATCH "totalRunTimeInSeconds=([0-9]+)")
					{
						$TimeoutInSeconds = [int]($Matches[1]) + $ExtraTimeoutInSeconds;

						Write-Message -Warning "Set test run execution timeout to $TimeoutInSeconds seconds.";
					}

					continue;
				}
				
				if ($ScriptState -EQ $ScriptStates.ProvidingParameters)
				{
					if (Test-Path "env:$Message")
					{
						Write-Message "Sending parameter [$Message]";
						$SerialPort.WriteLine($((Get-ChildItem "env:$Message").Value -Replace " ", ""));
					}
					else
					{
						Write-Message -Error "The device requested the env var '$Message', but it is not set in the local host.";
						$ReturnCode = $ReturnCodes.TestFailed;
						$ScriptState = $ScriptStates.Terminating;
					}

					continue;					
				}
				
				if ($Message -IMATCH "VerifyMessageReceived\[(.*)\] sent on \[(.*)\]")
				{
					$ScriptState = $ScriptStates.WaitingForTestCompletion;
					$TrackKeepAlives = $true;
					$LastKeepAliveTime = Get-Date;
					
					$VerificationResult = "0";
					
					$MessageToVerify = $MATCHES[1];
					$TimeSent = [DateTime]::Parse($MATCHES[2]);
					
					$EventsPendingVerification.Add(@($MessageToVerify, $TimeSent));
					
					if (Verify-EventsReceivedByHub -EventHubListener $EventHubListener -EventHubListenTimeoutInSeconds $EventHubListenTimeoutInSeconds -VerificationList $EventsPendingVerification -MaxEventReceivedWaitTime $MaxEventReceivedWaitTime)
					{
						$VerificationResult = "1";
					}
					
					$SerialPort.WriteLine($VerificationResult);
					continue;
				}
				
				if ($Message -IMATCH "Error:")
				{
					Write-Message -Error "A failure has been reported by the device, please investigate. The test run will be terminated.";
					$ReturnCode = $ReturnCodes.TestFailed;
					$ScriptState = $ScriptStates.Terminating;
					continue;
				}

				if ($Message -IMATCH "([0-1]+) tests ran, ([0-1]+) failed, ([0-1]+) succeeded.")
				{
					Write-Message "Test execution completed on MBED device (Tests Run=$($MATCHES[1]), Failed=$($MATCHES[2]), Passed=$($MATCHES[3])).";
					
					# Regex will have 4 matches, one for the whole string and one for each component within parenthesis, on the order they appear on the string.
					if ($MATCHES[3] -EQ "1")
					{
						$ReturnCode = $ReturnCodes.Success;
					}
					else
					{
						$ReturnCode = $ReturnCodes.TestFailed;
					}
					
					$ScriptState = $ScriptStates.Terminating;
					continue;
				}
			}
		}
		
		if ($TrackKeepAlives -EQ $true -AND ((Get-Date).Subtract($LastKeepAliveTime).TotalSeconds -GT $KeepAliveTimeoutInSeconds))
		{
			Write-Message -Error "Time out occurred receiving test keep-alive.";
			$ReturnCode = $ReturnCodes.TestTimedOut;
			$ScriptState = $ScriptStates.Terminating;
			continue;
		}

		if ((Get-Date).Subtract($InitialTime).TotalSeconds -GT $TimeoutInSeconds)
		{
			Write-Message -Error "Test execution timed out.";
			$ReturnCode = $ReturnCodes.TestTimedOut;
			$ScriptState = $ScriptStates.Terminating;
			continue;
		}
		
		Sleep $SleepTime;
	}
}

if ($EventHubListener -NE $null)
{
	$EventHubListener.Close();
}

Write-Message "Closing connection to device (serial port '$Port').";

$SerialPort.Close();

Write-Message "The script is terminating (return code=$ReturnCode).";

return $ReturnCode;