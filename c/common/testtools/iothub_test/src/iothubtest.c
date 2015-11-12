// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "urlencode.h"
#include "crt_abstractions.h"

#include "threadapi.h"
#include "strings.h"
#include "iothubtest.h"
#include "iot_logging.h"
#include <proton/message.h>
#include <proton/messenger.h>
#include <proton/error.h>

const char* AMQP_RECV_ADDRESS_FMT = "amqps://iothubowner:%s@%s.%s/%s/ConsumerGroups/%s/Partitions/%u";
const char* AMQP_ADDRESS_PATH_FMT = "/devices/%s/messages/deviceBound";
const char* AMQP_SEND_ADDRESS_FMT = "amqps://iothubowner%%40sas.root.%s:%s@%s.%s/messages/deviceBound";

#define PROTON_DEFAULT_TIMEOUT      20*1000
#define THREAD_CONTINUE             0
#define THREAD_END                  1
#define MAX_DRAIN_TIME              1000.0
#define MAX_SHORT_VALUE             32767         /* maximum (signed) short value */

DEFINE_ENUM_STRINGS(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);

typedef struct IOTHUB_VALIDATION_INFO_TAG
{
    char* iotHubName;
    char* hostName;
    char* partnerName;
    char* partnerHost;
    STRING_HANDLE consumerGroup;
    STRING_HANDLE deviceId;
    STRING_HANDLE deviceKey;
    STRING_HANDLE eventhubName;
    STRING_HANDLE iotSharedSig;
    STRING_HANDLE eventhubAccessKey;
    unsigned int partitionCount;
    volatile sig_atomic_t messageThreadExit;
} IOTHUB_VALIDATION_INFO;

unsigned int ConvertToUnsignedInt(const unsigned char data[], int position)
{
    unsigned int result;
    if (data == NULL)
    {
        result = 0;
    }
    else
    {
        result = 0;
        for (int nIndex = 0; nIndex < 4; nIndex++) 
        {
            int nTest = data[nIndex+position];
            nTest <<= (nIndex*8);
            result += nTest;
        }
    }
    return result;
}

static unsigned char* GetByteArray(const char* pszData, size_t* bufferLen)
{
    unsigned char* result;
    if (bufferLen != NULL)
    {
        *bufferLen = strlen(pszData);
        result = (unsigned char*)malloc(*bufferLen);

        size_t nIndex;
        for (nIndex = 0; nIndex < *bufferLen; nIndex++)
        {
            result[nIndex] = (unsigned char)toupper(pszData[nIndex]);
        }
    }
    else
    {
        result = NULL;
    }
    return result;
}

void ComputeHash(const unsigned char pszData[], size_t dataLen, unsigned int nSeed1, unsigned int nSeed2, unsigned int* hash1, unsigned int* hash2)
{
    unsigned int nVal1, nVal2, nVal3;

    nVal1 = nVal2 = nVal3 = (unsigned int)(0xdeadbeef + dataLen + nSeed1);
    nVal3 += nSeed2;

    int nIndex = 0;
    size_t nLen = dataLen;
    while (nLen > 12)
    {
        nVal1 += ConvertToUnsignedInt(pszData, nIndex);
        nVal2 += ConvertToUnsignedInt(pszData, nIndex+4);
        nVal3 += ConvertToUnsignedInt(pszData, nIndex+8);

        nVal1 -= nVal3;
        nVal1 ^= (nVal3 << 4) | (nVal3 >> 28);
        nVal3 += nVal2;

        nVal2 -= nVal1;
        nVal2 ^= (nVal1 << 6) | (nVal1 >> 26);
        nVal1 += nVal3;

        nVal3 -= nVal2;
        nVal3 ^= (nVal2 << 8) | (nVal2 >> 24);
        nVal2 += nVal1;

        nVal1 -= nVal3;
        nVal1 ^= (nVal3 << 16) | (nVal3 >> 16);
        nVal3 += nVal2;

        nVal2 -= nVal1;
        nVal2 ^= (nVal1 << 19) | (nVal1 >> 13);
        nVal1 += nVal3;

        nVal3 -= nVal2;
        nVal3 ^= (nVal2 << 4) | (nVal2 >> 28);
        nVal2 += nVal1;

        nIndex += 12;
        nLen -= 12;
    }
    switch (nLen)
    {
        case 12:
            nVal1 += ConvertToUnsignedInt(pszData, nIndex);
            nVal2 += ConvertToUnsignedInt(pszData, nIndex+4);
            nVal3 += ConvertToUnsignedInt(pszData, nIndex+8);
            break;
        case 11: // No break;
            nVal3 += ((unsigned int)pszData[nIndex + 10]) << 16;
        case 10: // No break;
            nVal3 += ((unsigned int)pszData[nIndex + 9]) << 8;
        case 9: // No break;
            nVal3 += (unsigned int)pszData[nIndex + 8];
        case 8:
            nVal2 += ConvertToUnsignedInt(pszData, nIndex+4);
            nVal1 += ConvertToUnsignedInt(pszData, nIndex);
            break;
        case 7:// No break
            nVal2 += ((unsigned int)pszData[nIndex + 6]) << 16;
        case 6:// No break
            nVal2 += ((unsigned int)pszData[nIndex + 5]) << 8;
        case 5:// No break
            nVal2 += ((unsigned int)pszData[nIndex + 4]);
        case 4:
            nVal1 += ConvertToUnsignedInt(pszData, nIndex);
            break;
        case 3:// No break
            nVal1 += ((unsigned int)pszData[nIndex + 2]) << 16;
        case 2:// No break
            nVal1 += ((unsigned int)pszData[nIndex + 1]) << 8;
        case 1:
            nVal1 += (unsigned int)pszData[nIndex];
            break;
        default:
        case 0:
            *hash1 = nVal3;
            *hash2 = nVal2;
            return;
    }

    nVal3 ^= nVal2;
    nVal3 -= (nVal2 << 14) | (nVal2 >> 18);

    nVal1 ^= nVal3;
    nVal1 -= (nVal3 << 11) | (nVal3 >> 21);

    nVal2 ^= nVal1;
    nVal2 -= (nVal1 << 25) | (nVal1 >> 7);

    nVal3 ^= nVal2;
    nVal3 -= (nVal2 << 16) | (nVal2 >> 16);

    nVal1 ^= nVal3;
    nVal1 -= (nVal3 << 4) | (nVal3 >> 28);

    nVal2 ^= nVal1;
    nVal2 -= (nVal1 << 14) | (nVal1 >> 18);

    nVal3 ^= nVal2;
    nVal3 -= (nVal2 << 24) | (nVal2 >> 8);

    *hash1 = nVal3;
    *hash2 = nVal2;
}

static size_t ResolvePartitionIndex(const char* partitionKey, size_t maxPartition)
{
    size_t result;

    // Normalize the Partition Key to be upper case
    size_t len = 0;
    unsigned char* byteArray = GetByteArray(partitionKey, &len);
    if (byteArray == NULL)
    {
        // On failure look at the zero partition
        result = 0;
    }
    else
    {
        int defaultLogicalPartitionCount = MAX_SHORT_VALUE;
        unsigned int hash1 = 0, hash2 = 0;
        ComputeHash(byteArray, len, 0, 0, &hash1, &hash2);
        unsigned long hashedValue = hash1 ^ hash2;

        // Intended Value truncation from UINT to short
        short sTruncateVal = (short)hashedValue;
        short logicalPartition = (short)abs(sTruncateVal % defaultLogicalPartitionCount);

        double shortRangeWidth = floor(defaultLogicalPartitionCount/maxPartition);
        int remainingLogicalPartitions = defaultLogicalPartitionCount - (maxPartition * (int)shortRangeWidth);
        int largeRangeWidth = ( (int)shortRangeWidth) + 1;
        int largeRangesLogicalPartitions = largeRangeWidth * remainingLogicalPartitions;
        result = logicalPartition < largeRangesLogicalPartitions ? logicalPartition / largeRangeWidth : remainingLogicalPartitions + ((logicalPartition - largeRangesLogicalPartitions) / (int)shortRangeWidth);

        free(byteArray);
    }
    return result;
}

static bool AddPropertyToMessage(pn_messenger_t* messenger, pn_message_t* message, const char* pszDeviceId, const char* pszAddress)
{
    bool result;
    size_t addressLen = strlen(AMQP_ADDRESS_PATH_FMT)+strlen(pszDeviceId)+1;
    char* deviceDest = (char*)malloc(addressLen+1);
    if (deviceDest == NULL)
    {
        result = false;
    }
    else
    {
        sprintf_s(deviceDest, addressLen+1, AMQP_ADDRESS_PATH_FMT, pszDeviceId);
        if (pn_messenger_route(messenger, deviceDest, pszAddress) != 0 ||
            pn_message_set_address(message, deviceDest) != 0
           )
        {
            result = false;
        }
        else
        {
            result = true;
        }
        free(deviceDest);
    }
    return result;
}

static int RetrieveIotHubClientInfo(const char* pszIotConnString, IOTHUB_VALIDATION_INFO* dvhInfo)
{
    int result;
    int beginName, endName;
    int beginHost, endHost;

    if (sscanf(pszIotConnString, "HostName=%n%*[^.]%n.%n%*[^;];%nSharedAccessKeyName=*;SharedAccessKey=*", &beginName, &endName, &beginHost, &endHost) != 0)
    {
        result = __LINE__;
    }
    else
    {
        if ( (dvhInfo->iotHubName = (char*)malloc(endName+beginName+1) ) == NULL)
        {
            result = __LINE__;
        }
        else if ( (dvhInfo->hostName = (char*)malloc(endHost+beginHost+1) ) == NULL)
        {
            free(dvhInfo->iotHubName);
            result = __LINE__;
        }
        else if (sscanf(pszIotConnString, "HostName=%[^.].%[^;];SharedAccessKeyName=*;SharedAccessKey=*", dvhInfo->iotHubName, dvhInfo->hostName) != 2)
        {
            free(dvhInfo->iotHubName);
            free(dvhInfo->hostName);
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static int RetrieveEventHubClientInfo(const char* pszconnString, IOTHUB_VALIDATION_INFO* dvhInfo)
{
    int result;
    int beginName, endName;
    int beginHost, endHost;

    if (sscanf(pszconnString, "Endpoint=sb://%n%*[^.]%n.%n%*[^/]%n/;SharedAccessKeyName=owner;SharedAccessKey=*", &beginName, &endName, &beginHost, &endHost) != 0)
    {
        result = __LINE__;
    }
    else
    {
        if ( (dvhInfo->partnerName = (char*)malloc(endName+beginName+1) ) == NULL)
        {
            result = __LINE__;
        }
        else if ( (dvhInfo->partnerHost = (char*)malloc(endHost+beginHost+1) ) == NULL)
        {
            free(dvhInfo->partnerName);
            result = __LINE__;
        }
        else if (sscanf(pszconnString, "Endpoint=sb://%[^.].%[^/]/;SharedAccessKeyName=owner;SharedAccessKey=*", dvhInfo->partnerName, dvhInfo->partnerHost) != 2)
        {
            free(dvhInfo->partnerName);
            free(dvhInfo->partnerHost);
            result = __LINE__;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

IOTHUB_TEST_HANDLE IoTHubTest_Initialize(const char* eventhubConnString, const char* iothubConnString, const char* deviceId, const char* deviceKey, const char* eventhubName, const char* eventhubAccessKey, const char* sharedSignature, const char* consumerGroup)
{
    IOTHUB_TEST_HANDLE result;
    IOTHUB_VALIDATION_INFO* devhubValInfo;

    if (eventhubConnString == NULL || iothubConnString == NULL || deviceId == NULL || deviceKey == NULL || eventhubName == NULL || sharedSignature == NULL || eventhubAccessKey == NULL)
    {
        result = NULL;
    }
    else if ( (devhubValInfo = malloc(sizeof(IOTHUB_VALIDATION_INFO) ) ) == NULL)
    {
        result = NULL;
    }
    else if ( (devhubValInfo->consumerGroup = STRING_construct(consumerGroup) ) == NULL)
    {
        free(devhubValInfo);
        result = NULL;
    }
    else if ( (devhubValInfo->deviceId = STRING_construct(deviceId) ) == NULL)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        free(devhubValInfo);
        result = NULL;
    }
    else if ( (devhubValInfo->eventhubAccessKey = URL_EncodeString(eventhubAccessKey) ) == NULL)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->deviceId);
        free(devhubValInfo);
        result = NULL;
    }
    else if ( (devhubValInfo->deviceKey = URL_EncodeString(deviceKey) ) == NULL)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->eventhubAccessKey);
        STRING_delete(devhubValInfo->deviceId);
        free(devhubValInfo);
        result = NULL;
    }
    else if ( (devhubValInfo->eventhubName = STRING_construct(eventhubName) ) == NULL)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->eventhubAccessKey);
        STRING_delete(devhubValInfo->deviceId);
        STRING_delete(devhubValInfo->deviceKey);
        free(devhubValInfo);
        result = NULL;
    }
    else if ( (devhubValInfo->iotSharedSig = STRING_construct(sharedSignature) ) == NULL)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->eventhubAccessKey);
        STRING_delete(devhubValInfo->deviceId);
        STRING_delete(devhubValInfo->deviceKey);
        STRING_delete(devhubValInfo->eventhubName);
        free(devhubValInfo);
        result = NULL;
    }
    else if (RetrieveIotHubClientInfo(iothubConnString, devhubValInfo) != 0)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->eventhubAccessKey);
        STRING_delete(devhubValInfo->deviceId);
        STRING_delete(devhubValInfo->deviceKey);
        STRING_delete(devhubValInfo->eventhubName);
        STRING_delete(devhubValInfo->iotSharedSig);
        free(devhubValInfo);
        result = NULL;
    }
    else if (RetrieveEventHubClientInfo(eventhubConnString, devhubValInfo) != 0)
    {
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->eventhubAccessKey);
        STRING_delete(devhubValInfo->deviceId);
        STRING_delete(devhubValInfo->deviceKey);
        STRING_delete(devhubValInfo->eventhubName);
        STRING_delete(devhubValInfo->iotSharedSig);
        free(devhubValInfo->iotHubName);
        free(devhubValInfo->hostName);
        free(devhubValInfo);
        result = NULL;
    }
    else
    {
        result = devhubValInfo;
    }
    return result;
}

void IoTHubTest_Deinit(IOTHUB_TEST_HANDLE devhubHandle)
{
    if (devhubHandle != NULL)
    {
        IOTHUB_VALIDATION_INFO* devhubValInfo = (IOTHUB_VALIDATION_INFO*)devhubHandle;
        STRING_delete(devhubValInfo->consumerGroup);
        STRING_delete(devhubValInfo->eventhubAccessKey);
        STRING_delete(devhubValInfo->deviceId);
        STRING_delete(devhubValInfo->eventhubName);
        STRING_delete(devhubValInfo->deviceKey);
        STRING_delete(devhubValInfo->iotSharedSig);
        free(devhubValInfo->iotHubName);
        free(devhubValInfo->partnerName);
        free(devhubValInfo->partnerHost);
        free(devhubValInfo->hostName);
        free(devhubValInfo);
    }
}

static char* CreateReceiveAddress(IOTHUB_VALIDATION_INFO* devhubValInfo, size_t partitionCount)
{
    char* result;
    size_t addressLen = strlen(AMQP_RECV_ADDRESS_FMT) + STRING_length(devhubValInfo->eventhubAccessKey) + STRING_length(devhubValInfo->eventhubName) + strlen(devhubValInfo->partnerName) + strlen(devhubValInfo->partnerHost) + STRING_length(devhubValInfo->consumerGroup) + 5;
    result = (char*)malloc(addressLen + 1);
    if (result != NULL)
    {
        size_t targetPartition = ResolvePartitionIndex(STRING_c_str(devhubValInfo->deviceId), partitionCount);
        sprintf_s(result, addressLen+1, AMQP_RECV_ADDRESS_FMT, STRING_c_str(devhubValInfo->eventhubAccessKey), devhubValInfo->partnerName, devhubValInfo->partnerHost, STRING_c_str(devhubValInfo->eventhubName), STRING_c_str(devhubValInfo->consumerGroup), targetPartition);
    }
    else
    {
        result = NULL;
    }
    return result;
}

static char* CreateSendAddress(IOTHUB_VALIDATION_INFO* devhubValInfo)
{
    char* result;
    STRING_HANDLE encodedSig = URL_Encode(devhubValInfo->iotSharedSig);
    if (encodedSig != NULL)
    {
        size_t addressLen = strlen(AMQP_SEND_ADDRESS_FMT)+(strlen(devhubValInfo->iotHubName)*2)+strlen(devhubValInfo->hostName)+(STRING_length(encodedSig) );
        result = (char*)malloc(addressLen+1);
        if (result != NULL)
        {
            sprintf_s(result, addressLen+1, AMQP_SEND_ADDRESS_FMT, devhubValInfo->iotHubName, STRING_c_str(encodedSig), devhubValInfo->iotHubName, devhubValInfo->hostName);
        }
        STRING_delete(encodedSig);
    }
    else
    {
        result = NULL;
    }
    return result;
}

IOTHUB_TEST_CLIENT_RESULT IoTHubTest_ListenForEvent(IOTHUB_TEST_HANDLE devhubHandle, pfIoTHubMessageCallback msgCallback, size_t partitionCount, void* context, time_t receiveTimeRangeStart, double maxDrainTimeInSeconds)
{
    IOTHUB_TEST_CLIENT_RESULT result = 0;
    if (devhubHandle == NULL || msgCallback == NULL)
    {
        result = IOTHUB_TEST_CLIENT_ERROR;
    }
    else 
    {
        IOTHUB_VALIDATION_INFO* devhubValInfo = (IOTHUB_VALIDATION_INFO*)devhubHandle;
        time_t beginExecutionTime, nowExecutionTime;
        double timespan;
        pn_messenger_t* messenger;
        if ( (messenger = pn_messenger(NULL) ) == NULL)
        {
            result = IOTHUB_TEST_CLIENT_ERROR;
        }
        else
        {
            // Sets the Messenger Windows
            pn_messenger_set_incoming_window(messenger, 1);

            pn_messenger_start(messenger);
            if (pn_messenger_errno(messenger) )
            {
                result = IOTHUB_TEST_CLIENT_ERROR;
            }
            else
            {
                if (pn_messenger_set_timeout(messenger, PROTON_DEFAULT_TIMEOUT) != 0)
                {
                    result = IOTHUB_TEST_CLIENT_ERROR;
                }
                else
                {
                    result = IOTHUB_TEST_CLIENT_OK;

                    devhubValInfo->partitionCount = partitionCount;
                    devhubValInfo->messageThreadExit = THREAD_CONTINUE;

                    // Allocate the Address
                    char* szAddress = CreateReceiveAddress(devhubValInfo, partitionCount);
                    if (szAddress == NULL)
                    {
                        result = IOTHUB_TEST_CLIENT_ERROR;
                    }
                    else
                    {
						char tempBuffer[256];

                        // subscribe the messenger to all the partitions
                        bool atLeastOneMessageReceived = true;
                        beginExecutionTime = time(NULL);

						const char filter_name[] = "apache.org:selector-filter:string";

						pn_data_t* filter = pn_data(0);
						if (filter == NULL)
						{
							result = IOTHUB_TEST_CLIENT_ERROR;
						}
						else
						{
							/* 330s = 5:30. 5 minutes for clock skew on the service side and 30s lag for communication timeout. */
							int filter_string_length = sprintf(tempBuffer, "amqp.annotation.x-opt-enqueuedtimeutc > %llu", ((unsigned long long)receiveTimeRangeStart - 330) * 1000);

							if ((filter_string_length < 0) ||
								(pn_data_put_map(filter) != 0) ||
								!pn_data_enter(filter) ||
								(pn_data_put_symbol(filter, pn_bytes((sizeof(filter_name) / sizeof(filter_name[0]) - 1), filter_name)) != 0) ||
								(pn_data_put_described(filter) != 0) ||
								!pn_data_enter(filter) ||
								(pn_data_put_symbol(filter, pn_bytes((sizeof(filter_name) / sizeof(filter_name[0]) - 1), filter_name)) != 0) ||
								(pn_data_put_string(filter, pn_bytes(filter_string_length, tempBuffer)) != 0) ||
								!pn_data_exit(filter) ||
								!pn_data_exit(filter))
							{
								result = IOTHUB_TEST_CLIENT_ERROR;
							}
							else
							{
								if (pn_messenger_subscribe_with_filter(messenger, szAddress, filter) == NULL)
								{
									result = IOTHUB_TEST_CLIENT_ERROR;
									LogError("Unable to create a subscription using address %s\r\n", szAddress);
								}
								else
								{

									while (
										(atLeastOneMessageReceived) &&
										(devhubValInfo->messageThreadExit == THREAD_CONTINUE) &&
										((nowExecutionTime = time(NULL)), timespan = difftime(nowExecutionTime, beginExecutionTime), timespan < maxDrainTimeInSeconds)
										)
									{
										atLeastOneMessageReceived = false;
										// Wait for the message to be recieved
										pn_messenger_recv(messenger, -1);
										if (pn_messenger_errno(messenger) == 0)
										{
											while ((devhubValInfo->messageThreadExit == THREAD_CONTINUE) && pn_messenger_incoming(messenger))
											{
												pn_message_t* pnMessage = pn_message();
												if (pnMessage == NULL)
												{
													devhubValInfo->messageThreadExit = THREAD_END;
												}
												else
												{
													pn_messenger_get(messenger, pnMessage);
													if (pn_messenger_errno(messenger) == 0)
													{
														pn_tracker_t tracker = pn_messenger_incoming_tracker(messenger);

														pn_data_t* pBody = pn_message_body(pnMessage);
														if (pBody != NULL)
														{
															if (!pn_data_next(pBody))
															{
																devhubValInfo->messageThreadExit = THREAD_END;
															}
															else
															{
																pn_type_t typeOfBody = pn_data_type(pBody);
																atLeastOneMessageReceived = true;
																if (PN_STRING == typeOfBody)
																{
																	pn_bytes_t descriptor = pn_data_get_string(pBody);
																	if (msgCallback != NULL)
																	{
																		if (msgCallback(context, descriptor.start, descriptor.size) != 0)
																		{
																			devhubValInfo->messageThreadExit = THREAD_END;
																		}
																	}
																}
																else if (PN_BINARY == typeOfBody)
																{
																	pn_bytes_t descriptor = pn_data_get_binary(pBody);
																	if (msgCallback != NULL)
																	{
																		if (msgCallback(context, descriptor.start, descriptor.size) != 0)
																		{
																			devhubValInfo->messageThreadExit = THREAD_END;
																		}
																	}
																}
																else
																{
																	//Unknown Data Item
																}
															}
														}
														pn_messenger_accept(messenger, tracker, 0);
														pn_message_clear(pnMessage);
														pn_messenger_settle(messenger, tracker, 0);
													}
													else
													{
														devhubValInfo->messageThreadExit = THREAD_END;
													}
													pn_message_free(pnMessage);
												}
											}
										}
										else
										{
											devhubValInfo->messageThreadExit = THREAD_END;
											break;
										}
									}
								}
							}

							pn_data_free(filter);
						}

                        free(szAddress);
                    }
                }
            }

            // bring down the messenger
            do
            {
                pn_messenger_stop(messenger);
            } while (!pn_messenger_stopped(messenger) );
            pn_messenger_free(messenger);
        }
    }
    return result;
}

IOTHUB_TEST_CLIENT_RESULT IoTHubTest_ListenForRecentEvent(IOTHUB_TEST_HANDLE devhubHandle, pfIoTHubMessageCallback msgCallback, size_t partitionCount, void* context, double maxDrainTimeInSeconds)
{
	return IoTHubTest_ListenForEvent(devhubHandle, msgCallback, partitionCount, context, time(NULL), maxDrainTimeInSeconds);
}

IOTHUB_TEST_CLIENT_RESULT IoTHubTest_ListenForEventForMaxDrainTime(IOTHUB_TEST_HANDLE devhubHandle, pfIoTHubMessageCallback msgCallback, size_t partitionCount, void* context)
{
    return IoTHubTest_ListenForRecentEvent(devhubHandle, msgCallback, partitionCount, context, MAX_DRAIN_TIME);
}

IOTHUB_TEST_CLIENT_RESULT IoTHubTest_SendMessage(IOTHUB_TEST_HANDLE devhubHandle, const char* data, size_t len)
{
    IOTHUB_TEST_CLIENT_RESULT result;

    if ((devhubHandle == NULL) ||
        ((len == 0) && (data != NULL)) ||
        ((data != NULL) && (len == 0)))
    {
        result = IOTHUB_TEST_CLIENT_ERROR;
    }
    else
    {
        IOTHUB_VALIDATION_INFO* devhubValInfo = (IOTHUB_VALIDATION_INFO*)devhubHandle;
        pn_messenger_t* messenger = pn_messenger(NULL);
        if (messenger == NULL)
        {
            result = IOTHUB_TEST_CLIENT_ERROR;
        }
        else
        {
            // Sets the Messenger Windows
            if ( (pn_messenger_start(messenger) != 0) ||
                 (pn_messenger_set_outgoing_window(messenger, 10) != 0) ||
                 (pn_messenger_set_blocking(messenger, true) ) ||
                 (pn_messenger_set_timeout(messenger, PROTON_DEFAULT_TIMEOUT) != 0)
               )
            {
                result = IOTHUB_TEST_CLIENT_ERROR;
            }
            else
            {
                pn_message_t* message = pn_message();
                char* szAddress = CreateSendAddress(devhubValInfo);
                if (szAddress == NULL || !AddPropertyToMessage(messenger, message, STRING_c_str(devhubValInfo->deviceId), szAddress) )
                {
                    result = IOTHUB_TEST_CLIENT_ERROR;
                }
                else
                {
                    pn_data_t * body;
                    pn_timestamp_t expireTime = LLONG_MAX/2; // Needs to be a sufficiently long time
                    if ( (pn_message_set_inferred(message, true) != 0) ||
                         (pn_message_set_expiry_time(message, expireTime) != 0) ||
                         ( (body = pn_message_body(message) ) == NULL) ||
                         (pn_data_put_binary(body, pn_bytes(len, data) ) != 0) ||
                         (pn_messenger_put(messenger, message) != 0)
                       )
                    {
                        result = IOTHUB_TEST_CLIENT_ERROR;
                    }
                    else
                    {
                        pn_status_t messengerStatus;
                        size_t numberOfAttempts;
                        pn_tracker_t tracker = pn_messenger_outgoing_tracker(messenger);
                        pn_messenger_send(messenger, -1);
                        for (numberOfAttempts = 0; numberOfAttempts < 10; numberOfAttempts++)
                        {
                            messengerStatus = pn_messenger_status(messenger, tracker);
                            if (messengerStatus != PN_STATUS_PENDING)
                            {
                                break;
                            }
                            else
                            {
                                ThreadAPI_Sleep(500);
                            }
                        }
                        if (messengerStatus != PN_STATUS_ACCEPTED)
                        {
                            result = IOTHUB_TEST_CLIENT_ERROR;
                        }
                        else
                        {
                            result = IOTHUB_TEST_CLIENT_OK;
                        }

                        // settle the tracker
                        pn_messenger_settle(messenger, tracker, 0);
                        pn_messenger_stop(messenger);
                    }
                    free(szAddress);
                }
                pn_message_free(message);
            }
            pn_messenger_free(messenger);
        }
    }
    return result;
}
