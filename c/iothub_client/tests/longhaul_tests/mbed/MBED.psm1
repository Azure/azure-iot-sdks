function New-List
{
    $List = New-Object PSObject;
    Add-Member -InputObject $List -MemberType NoteProperty -Name items -Value @()
    Add-Member -InputObject $List -MemberType ScriptProperty -Name "Count" -Value { return $this.items.Length; }
    Add-Member -InputObject $List -MemberType ScriptMethod -Name "Add" -Value { PARAM($item); $this.items += $item }
    Add-Member -InputObject $List -MemberType ScriptMethod -Name "RemoveAt" -Value { 
        PARAM($index); 
    
        if ($index -LT 0 -OR $index -GE $this.items.Length) { throw "Index out of range."; }

        $itemRemoved = $null;
        $newItems = @();
        for ($i = 0; $i -LT $this.items.Length; $i++)
        {
            if ($i -NE $index)
            {
                $newItems += $this.items[$i];
            }
            else
            {
                $itemRemoved = $this.items[$i];
            }
        }

        $this.items = $newItems;

        return $itemRemoved;
    }
    Add-Member -InputObject $List -MemberType ScriptMethod -Name "Remove" -Value { 
        PARAM($itemToRemove); 
    
        $newItems = @();

        foreach ($item in $this.items)
        {
            if ($item -NE $itemToRemove)
            {
                $newItems += $item;
            }
        }

        $this.items = $newItems;
    }
    Add-Member -InputObject $List -MemberType ScriptMethod -Name "GetElementAt" -Value { 
        PARAM($index); 

        return $this.items[$index];
    }
    Add-Member -InputObject $List -MemberType ScriptMethod -Name "SetElementAt" -Value { 
        PARAM($index, $item); 

        return $this.items[$index] = $item;
    }
    Add-Member -InputObject $List -MemberType ScriptMethod -Name "InsertAt" -Value { 
        PARAM($index, $item); 

        $newItems = @();
        for ($i = 0; $i -LT $this.items.Length; $i++)
        {
            if ($i -GE $index)
            {
                $newItems += $item;
            }

            $newItems += $this.items[$i];
        }

        $this.items = $newItems;
    }
    Add-Member -InputObject $List -MemberType ScriptProperty -Name "FirstElement" -Value { 
        return $this.items[0];
    }
    Add-Member -InputObject $List -MemberType ScriptProperty -Name "LastElement" -Value { 
        return $this.items[$this.items.Length - 1];
    }

    return $List;
}

function New-Enum
{
    PARAM ($KeyValuePairs = @{})
    
    $Enum = New-Object PSObject;

    foreach ($Key in $KeyValuePairs.Keys)
    {
        Add-Member -InputObject $Enum -MemberType NoteProperty -Name $Key -Value $KeyValuePairs[$Key];    
    }

    return $Enum;
}

function Write-Message
{
    PARAM ([Switch]$Error, [Switch]$Warning, $Message)

    $TimeStamp = Get-Date;
    $MessageType = "INFO";

    if ($Warning)
    {
        $MessageType = "WARN";
    }
    if ($Error)
    {
        $MessageType = "ERRO";
    }

    Write-Host "$($TimeStamp.ToString('yyyy/MM/dd hh:mm:ss.fff')) [$MessageType]: $Message";
}


function Read-MBEDMessages
{
    PARAM ($SerialPort, [REF]$MessageBufferRef, $MessageList)

    $MessageBuffer = $MessageBufferRef.Value;

    $HasNewMessage = $false;

	$MessageBuffer += $SerialPort.ReadLine();

	$CRLFPosition = -1;
	while (($CRLFPosition = $MessageBuffer.IndexOf("`r`n")) -NE -1)
	{
		$HasNewMessage = $true;

		$NewMessage = $MessageBuffer.Substring(0, $CRLFPosition);
		
		$MessageList.Add($NewMessage);
		$MessageBuffer = $MessageBuffer.Substring($CRLFPosition + 2, $MessageBuffer.Length - $CRLFPosition - 2);
	}

    return $HasNewMessage;
}


function Get-EventHubFullConnectionString()
{
	PARAM ($EventHubConnectionString, $EventHubListenName);
	
	$FullPath = @($EventHubConnectionString, $EventHubListenName) -join ";EntityPath=";
	
	return $FullPath;
}

function New-IoTHubEventListener
{
	PARAM ($EventHubConnectionString, $EventHubListenName, $EventHubConsumerGroup, $DeviceId, $ListeningStartTime = $(get-date))
	
	$EventHubFullConnectionString = Get-EventHubFullConnectionString -EventHubConnectionString $EventHubConnectionString -EventHubListenName $EventHubListenName;
	
	$EventHubClient = [Microsoft.ServiceBus.Messaging.EventHubClient]::CreateFromConnectionString($EventHubFullConnectionString);

	$PartitionCount = $EventHubClient.GetRuntimeInformation().PartitionCount;

	$DevicePartition = [Microsoft.Azure.Devices.Common.EventHubPartitionKeyResolver]::ResolveToPartition($DeviceId, $PartitionCount);

	$EventHubReceiver = $EventHubClient.GetConsumerGroup($EventHubConsumerGroup).CreateReceiver($DevicePartition, $ListeningStartTime);
	
	return $EventHubReceiver;
}

function Get-IoTEventHubNextEvent
{
	PARAM ($EventHubListener, $TimeoutInSeconds = $(1))
	
	$EventReceiveTimeout = [System.TimeSpan]::FromSeconds($TimeoutInSeconds);
	$EventData = $EventHubListener.Receive($EventReceiveTimeout);
	return $EventData;
}

function Verify-EventsReceivedByHub
{
	PARAM ($EventHubListener, $EventHubListenTimeoutInSeconds, $VerificationList, $MaxEventReceivedWaitTime);
	
	$Result = $true;
	
	while (($EventData = Get-IoTEventHubNextEvent -EventHubListener $EventHubListener -TimeoutInSeconds $EventHubListenTimeoutInSeconds) -NE $null)
	{
		$MessageRetrievedFromEventHub = [System.Text.Encoding]::UTF8.GetString($EventData.GetBytes());
	
		for ($i = 0; $i -LT $VerificationList.Count; $i++)
		{
			$EventToVerify = $VerificationList.GetElementAt($i);
			
			if ($MessageRetrievedFromEventHub -IEQ $EventToVerify[0])
			{
				$VerificationList.RemoveAt($i);
				break;
			}
		}
	}
	
	for ($i = 0; $i -LT $VerificationList.Count; $i++)
	{
		$EventToVerify = $VerificationList.GetElementAt($i);
		
		if (($(Get-Date) - $EventToVerify[1]).TotalSeconds -GT $MaxEventReceivedWaitTime)
		{
			Write-Message -Error -Message "Event $($EventToVerify[0]) not received by IoT hub within $MaxEventReceivedWaitTime seconds.";
			$Result = $false;
			break;
		}
	}
	
	return $Result;
}