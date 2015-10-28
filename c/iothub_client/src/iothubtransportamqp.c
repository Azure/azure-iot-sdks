// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include "proton/message.h"
#include "proton/messenger.h"

#include "iot_logging.h"
#include "strings.h"
#include "urlencode.h"
#include "doublylinkedlist.h"
#include "crt_abstractions.h"
#include "sastoken.h"

#include "iothub_client_ll.h"
#include "iothub_client_private.h"
#include "iothubtransportamqp.h"
#include "iothub_client_amqp_internal.h"

static const size_t NUMBER_OF_MESSENGER_STOP_TRIES = PROTON_MESSENGER_STOP_TRIES;
static const size_t MAXIMUM_EVENT_LENGTH = PROTON_MAXIMUM_EVENT_LENGTH;


#define amqp_batch_result_values \
        amqp_batch_nowork,       \
        amqp_batch_batch,        \
        amqp_batch_nobatch,      \
        amqp_batch_error         \

DEFINE_ENUM(amqp_batch_result, amqp_batch_result_values);

static const int IO_PROCESSING_YIELD_IN_MILLISECONDS = PROTON_PROCESSING_YIELD_IN_MILLISECONDS;

static void processReceives(TRANSPORT_HANDLE handle);

typedef struct MESSENGER_CONTAINER_TAG
{
    //
    // This links all of these records onto the messageCorral in the transport state.
    DLIST_ENTRY entry;

    //
    // This is a messenger that couldn't be stopped.  (You have to admire its initiative.)  We'll keep trying to stop it when
    // we have a spare moment.
    pn_messenger_t* messengerThatDidNotStop;
} MESSENGER_CONTAINER, *PMESSENGER_CONTAINER;

//
// Transport specific structure used to contain everything needed to send an event item.
//
typedef struct AMQP_WORK_ITEM_TAG
{
    //
    // At what time should we give up on sending this message.
    size_t expiry;

    //
    // Used by the transport to determine if the IO operation is complete.
    pn_tracker_t tracker;

    //
    // This acts as a listhead for all of the messages that make up one transfer frame.
    // If no batching is being done then this list will have at most one item.  If batching
    // is being done then this could be lengthy!
    DLIST_ENTRY eventMessages;

    //
    // This is used to hold this work item either on the work in progress list or on the available work item list.
    DLIST_ENTRY link;
} AMQP_WORK_ITEM, *PAMQP_WORK_ITEM;

static void rollbackEvent(PAMQP_TRANSPORT_STATE transportState);

static bool checkForErrorsThenWork(PAMQP_TRANSPORT_STATE transportState)
{
    pn_error_t* errorStruct;
    bool result;
    errorStruct = pn_messenger_error(transportState->messenger);
    if (pn_error_code(errorStruct) != 0)
    {
        LogError("An error was detected in the networking manager: %d\r\n", pn_error_code(errorStruct));
        transportState->messengerInitialized = false;
        result = false;
    }
    else
    {
        int protonResult;
        protonResult = pn_messenger_work(transportState->messenger, IO_PROCESSING_YIELD_IN_MILLISECONDS);
        if ((protonResult < 0) && (protonResult != PN_TIMEOUT))
        {
            LogError("A networking error occured. Proton error code: %d\r\n", protonResult);
            transportState->messengerInitialized = false;
            result = false;
        }
        else
        {
            result = true;
        }
    }
    return result;
}

static int setRecvMessageIds(IOTHUB_MESSAGE_HANDLE ioTMessage, pn_message_t* msg)
{
    int result;

    // Function can not fail
    pn_atom_t msgIdInfo = pn_message_get_id(msg);
    if (msgIdInfo.type != PN_NULL)
    {
        if (msgIdInfo.type != PN_STRING)
        {
            LogError("Message Id Failure: Invalid message id Type %d\r\n", msgIdInfo.type);
            result = __LINE__;
        }
        else
        {
            if (IoTHubMessage_SetMessageId(ioTMessage, msgIdInfo.u.as_bytes.start) != IOTHUB_MESSAGE_OK)
            {
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }
    else
    {
        // NULL values mean it's not set
        result = 0;
    }

    if (result == 0)
    {
        pn_atom_t corrIdInfo = pn_message_get_correlation_id(msg);
        if (corrIdInfo.type != PN_NULL)
        {
            if (corrIdInfo.type != PN_STRING)
            {
                LogError("Message Id Failure: Invalid correlation id Type %d\r\n", corrIdInfo.type);
                result = __LINE__;
            }
            else
            {
                if (IoTHubMessage_SetCorrelationId(ioTMessage, corrIdInfo.u.as_bytes.start) != IOTHUB_MESSAGE_OK)
                {
                    result = __LINE__;
                }
                else
                {
                    result = 0;
                }
            }
        }
        else
        {
            // NULL values mean it's not set
            result = 0;
        }
    }
    return result;
}

static int setSendMessageIds(PDLIST_ENTRY messagesEntry, pn_message_t* msg)
{
    int result;

    IOTHUB_MESSAGE_LIST* currentMessage = containingRecord(messagesEntry->Flink, IOTHUB_MESSAGE_LIST, entry);
    const char* messageId = IoTHubMessage_GetMessageId(currentMessage->messageHandle);
    if (messageId != NULL)
    {
        pn_bytes_t dataBytes = pn_bytes(strlen(messageId), messageId);
        pn_atom_t pnMsgId;
        pnMsgId.type = PN_STRING;
        pnMsgId.u.as_bytes = dataBytes;
        if (pn_message_set_id(msg, pnMsgId) == 0)
        {
            result = 0;
        }
        else
        {
            LogError("Failure setting pn_message_set_id.\r\n");
            result = __LINE__;
        }
    }
    else
    {
        result = 0;
    }

    if (result == 0)
    {
        const char* correlationId = IoTHubMessage_GetCorrelationId(currentMessage->messageHandle);
        if (correlationId != NULL)
        {
            pn_bytes_t dataBytes = pn_bytes(strlen(correlationId), correlationId);
            pn_atom_t pnCorrelationId;
            pnCorrelationId.type = PN_STRING;
            pnCorrelationId.u.as_bytes = dataBytes;
            if (pn_message_set_correlation_id(msg, pnCorrelationId) == 0)
            {
                result = 0;
            }
            else
            {
                LogError("Failure setting pn_message_set_correlation_id.\r\n");
                result = __LINE__;
            }
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

/* Code_SRS_IOTHUBTRANSPORTTAMQP_07_001: [All IoTHubMessage_Properties shall be enumerated and entered in the pn_message_properties Map.] */
static int setProperties(PDLIST_ENTRY messagesMakingUpBatch, pn_message_t* msg)
{
    int result = 0;
    size_t index;

    pn_data_t* propData;
    const char*const* keys;
    const char*const* values;
    size_t propertyCount = 0;

    IOTHUB_MESSAGE_LIST* currentMessage = containingRecord(messagesMakingUpBatch->Flink, IOTHUB_MESSAGE_LIST, entry);
    MAP_HANDLE mapProperties = IoTHubMessage_Properties(currentMessage->messageHandle);
    if (mapProperties == NULL)
    {
        LogError("Failure IoTHubMessage_Properties.\r\n");
        result = __LINE__;
    }
    else if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) != MAP_OK)
    {
        LogError("Failure retrieving Map_GetInternals.\r\n");
        result = __LINE__;
    }
    else if (propertyCount > 0)
    {
        if ( (propData  = pn_message_properties(msg) ) == NULL)
        {
            LogError("Failure pn_message_properties.\r\n");
            result = __LINE__;
        }
        else if (pn_data_put_map(propData) != 0)
        {
            LogError("Failure pn_data_put_map.\r\n");
            result = __LINE__;
        }
        else if (pn_data_enter(propData) == false )
        {
            LogError("Failure pn_data_enter.\r\n");
            result = __LINE__;
        }
        else
        {
            for (index = 0; index < propertyCount; index++)
            {
                if (pn_data_put_symbol(propData, pn_bytes(strlen(keys[index]), keys[index])) != 0)
                {
                    LogError("Failure pn_data_put_symbol.\r\n");
                    break;
                }
                else if (pn_data_put_string(propData, pn_bytes(strlen(values[index]), values[index])) != 0)
                {
                    LogError("Failure pn_data_put_string.\r\n");
                    break;
                }
            }
            if (index != propertyCount)
            {
                result = __LINE__;
            }
            else if (!pn_data_exit(propData) )
            {
                LogError("Failure pn_data_exit.\r\n");
                result = __LINE__;
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

static int getMapString(pn_data_t* propData, pn_bytes_t* mapValue)
{
    int result;
    if (pn_data_next(propData) == true)
    {
        if (pn_data_type(propData) == PN_STRING)
        {
            *mapValue = pn_data_get_string(propData);
            result = 0;
        }
        else
        {
            LogError("Failure pn_data_type.\r\n");
            result = __LINE__;
        }
    }
    else
    {
        LogError("Failure pn_data_next.\r\n");
        result = __LINE__;
    }
    return result;
}

static int getMapInt(pn_data_t* propData, int32_t* mapValue)
{
    int result;
    if (pn_data_next(propData) == true)
    {
        if (pn_data_type(propData) == PN_INT)
        {
            *mapValue = pn_data_get_int(propData);
            result = 0;
        }
        else
        {
            LogError("Failure pn_data_type.\r\n");
            result = __LINE__;
        }
    }
    else
    {
        LogError("Failure pn_data_next.\r\n");
        result = __LINE__;
    }
    return result;
}

/* Code_SRS_IOTHUBTRANSPORTTAMQP_07_002: [On messages the properties shall be retrieved from the message using pn_message_properties and will be entered in the IoTHubMessage_Properties Map.] */
static int cloneProperties(IOTHUB_MESSAGE_HANDLE ioTMessage, pn_message_t* msg)
{
    int result = 0;
    size_t index = 0;
    pn_data_t* propData = pn_message_properties(msg);
    if (propData == NULL)
    {
        LogError("Failure pn_message_properties.\r\n");
        result = __LINE__;
    }
    else if (pn_data_next(propData) == false )
    {
        // This should fail if there is no next sibling which means that there
        // are no property and it should continue
        result = 0;
    }
    else
    {
        size_t propertyCount = pn_data_get_map(propData)/2;
        if (propertyCount > 0)
        {
            if (pn_data_enter(propData) == false)
            {
                LogError("Failure pn_data_enter.\r\n");
                result = __LINE__;
            }
            else
            {
                // Get the Properties from the Message
                MAP_HANDLE properties = IoTHubMessage_Properties(ioTMessage);
                if (properties == NULL)
                {
                    result = __LINE__;
                }
                else
                {
                    for (index = 0; index < propertyCount; index++)
                    {
                        pn_bytes_t propValue;
                        pn_bytes_t propName;
                        if (getMapString(propData, &propValue) != 0)
                        {
                            result = __LINE__;
                            break;
                        }
                        if (getMapString(propData, &propName) != 0)
                        {
                            result = __LINE__;
                            break;
                        }

                        if (Map_AddOrUpdate(properties, propValue.start, propName.start) != MAP_OK)
                        {
                            LogError("Failure Map_AddOrUpdate.\r\n");
                            result = __LINE__;
                            break;
                        }
                    }
                    if (index != propertyCount)
                    {
                        result = __LINE__;
                    }
                    else if (!pn_data_exit(propData) )
                    {
                        LogError("Failure pn_data_exit.\r\n");
                        result = __LINE__;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }
    return result;
}

static void putSecondListAfterHeadOfFirst(PDLIST_ENTRY first, PDLIST_ENTRY second)
{
    DList_AppendTailList(first->Flink, second);
    DList_RemoveEntryList(second);
    DList_InitializeListHead(second); // Just to be tidy.
}

static bool stopMessenger(pn_messenger_t* messenger)
{
    bool succeeded;
    int result;
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_021: [Invoke pn_messenger_stop on the messenger.]*/
    result = pn_messenger_stop(messenger);
    if (result == 0)
    {
        succeeded = true;
    }
    else if (result != PN_INPROGRESS)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_023: [If pn_messenger_stop returns any result other than PN_INPROGRESS then place the messenger on a list for attempting to stop later.]*/
        succeeded = false;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_024: [If pn_messenger_stop returns PN_INPROGRESS invoke pn_messenger_stopped a fixed number of time, seeking a return of true.]*/
        size_t numberOfTrys;
        succeeded = false;
        for (numberOfTrys = NUMBER_OF_MESSENGER_STOP_TRIES; numberOfTrys > 0; numberOfTrys--)
        {
            pn_messenger_work(messenger,100);
            if (pn_messenger_stopped(messenger))
            {
                succeeded = true;
                break;
            }
        }
    }
    return succeeded;
}

static void herdTheMessengers(PAMQP_TRANSPORT_STATE state, bool logMessengerStopFailures)
{
    PDLIST_ENTRY currentMessengerLink = state->messengerCorral.Flink;

    //
    // We walk the list of messenger containers.  We try to stop each one in turn.
    //
    // If we are able to stop the contained messenger, we will free the actual messenger.  We will then free the messenger container.
    //
    while (currentMessengerLink != &state->messengerCorral)
    {
        PDLIST_ENTRY nextLink = currentMessengerLink->Flink;
        pn_messenger_t* oldMessenger = containingRecord(currentMessengerLink, MESSENGER_CONTAINER, entry)->messengerThatDidNotStop;
        if (stopMessenger(oldMessenger))
        {
            (void)(DList_RemoveEntryList(currentMessengerLink));
            free(containingRecord(currentMessengerLink, MESSENGER_CONTAINER, entry));
            pn_messenger_free(oldMessenger);
        }
        else
        {
            if (logMessengerStopFailures == true)
            {
                LogError("Unable to free the messenger at address: %p\r\n", oldMessenger);
            }
        }
        currentMessengerLink = nextLink;
    }
}

static void disposeOfOldMessenger(PAMQP_TRANSPORT_STATE state)
{
    if (state->messenger)
    {
        if (stopMessenger(state->messenger))
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_022: [If pn_messenger_stop returns 0 then invoke pn_messenger_free on the messenger.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_025: [If pn_messenger_stopped returns true, then call pn_messenger_free.]*/
            //
            // Yay.  It successfully stopped.
            //
            pn_messenger_free(state->messenger);
        }
        else
        {
            //
            // If we couldn't destroy it, then put it on a list and try to do it later in our "spare" time.
            //
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_023: [If pn_messenger_stop returns any result other than PN_INPROGRESS then place the messenger on a list for attempting to stop later.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_026: [If pn_messenger_stopped never returns true, then place the messenger on a list for attempting to stop later.]*/
            PMESSENGER_CONTAINER newContainer = malloc(sizeof(MESSENGER_CONTAINER));
            if (newContainer == NULL)
            {
                //
                // Darn! This is not good.  These containers are not very big.  If we can't allocate one, things are pretty bad.
                // However, it doesn't make sense to actually free this messenger.  It hasn't been stopped and we can NOT be sure who
                // is referencing the memory that it has allocated.  The safest thing to do in this very bad situation is to drop
                // it on the floor.  It's a leak, but a leak is better than anything else.  There is a pretty good likelyhood that
                // the app is exiting soon.  The dropped memory will be cleaned up by exit.
                ;
            }
            else
            {
                newContainer->messengerThatDidNotStop = state->messenger;
                DList_InsertTailList(&state->messengerCorral, &newContainer->entry);
            }
        }
        state->messenger = NULL;
    }
}

static void clientTransportAMQP_Destroy(TRANSPORT_HANDLE handle)
{
    PAMQP_TRANSPORT_STATE transportState = (PAMQP_TRANSPORT_STATE)handle;

    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_010: [clientTransportAMQP_Destroy shall do nothing if the handle is NULL.]*/
    if (transportState)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_018: [clientTransportAMQP_Destroy shall free all the resources currently in use.]*/
        //
        // Go through all of the active work items.  Complete their associated messages.
        // 
        while (!DList_IsListEmpty(&transportState->workInProgress))
        {
            PDLIST_ENTRY currentEntry = DList_RemoveHeadList(&transportState->workInProgress);
            PAMQP_WORK_ITEM currentItem = containingRecord(currentEntry, AMQP_WORK_ITEM, link);
            IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &currentItem->eventMessages, IOTHUB_BATCHSTATE_FAILED);
            DList_InsertTailList(&transportState->availableWorkItems, &currentItem->link);
        }

        //
        // Get rid of work items that aren't being used, and that should be everything given the above loop!
        //
        while (!DList_IsListEmpty(&transportState->availableWorkItems))
        {
            PDLIST_ENTRY currentEntry = DList_RemoveHeadList(&transportState->availableWorkItems);
            free(containingRecord(currentEntry, AMQP_WORK_ITEM, link));
        }

        disposeOfOldMessenger(transportState);
        
        //
        // herdTheMessengers will NOT free a messenger that couldn't be stopped.  Therefore the following
        // function could result in a memory leak of old messengers.  This is by FAR less of a problem then
        // having memory returned in to pool that something else might still be referring to.  (To get all folksy
        // here, don't put a sick chicken back in the hen house.)
        //

        herdTheMessengers(transportState, true);
        if (transportState->message)
        {
            pn_message_free(transportState->message);
        }
        STRING_delete(transportState->messageAddress);
        STRING_delete(transportState->eventAddress);
        STRING_delete(transportState->urledDeviceId);
        STRING_delete(transportState->devicesPortionPath);
        STRING_delete(transportState->cbsAddress);
        STRING_delete(transportState->deviceKey);
        STRING_delete(transportState->zeroLengthString);
        if (transportState->trustedCertificates != NULL)
        {
            free(transportState->trustedCertificates);
        }

        free(transportState);
    }
}

static int putToken(PAMQP_TRANSPORT_STATE transportState, STRING_HANDLE token)
{
    int result = 0;

    pn_data_t *properties;
    pn_atom_t messageId;

    messageId.u.as_ulong = transportState->sendTokenMessageId;
    messageId.type = PN_ULONG;
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_130: [putToken clears the single messages that is used for all communication via proton.]*/
    pn_message_clear(transportState->message);
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_131: [putToken obtains the properties of the message.]*/
    properties = pn_message_properties(transportState->message);
    if (properties == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_132: [If this fails then putToken fails.]*/
        LogError("unable to pn_message_properties\r\n");
        result = __LINE__;
    }
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_133: [putToken constructs a proton map which describes the put token operation.  If this construction fails then putToken fails.]*/
    else if (!((pn_data_put_map(properties) == 0) &&
        (pn_data_enter(properties) == true) &&
        (pn_data_put_string(properties, pn_bytes(strlen(PROTON_MAP_OPERATIONS_KEY), PROTON_MAP_OPERATIONS_KEY)) == 0) && // key
        (pn_data_put_string(properties, pn_bytes(strlen(PROTON_MAP_PUT_TOKEN_OPERATION), PROTON_MAP_PUT_TOKEN_OPERATION)) == 0) &&  // value
        (pn_data_put_string(properties, pn_bytes(strlen(PROTON_MAP_TYPE_KEY), PROTON_MAP_TYPE_KEY)) == 0) && // key
        (pn_data_put_string(properties, pn_bytes(strlen(PROTON_MAP_TOKEN_TYPE), PROTON_MAP_TOKEN_TYPE)) == 0) && // value
        (pn_data_put_string(properties, pn_bytes(strlen(PROTON_MAP_NAME_KEY), PROTON_MAP_NAME_KEY)) == 0) && // key
        (pn_data_put_string(properties, pn_bytes(STRING_length(transportState->devicesPortionPath), STRING_c_str(transportState->devicesPortionPath))) == 0) && // value
        (pn_data_exit(properties))
        ))
    {
        LogError("unable to build CBS put token map\r\n");
        result = __LINE__;
    }
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_134: [putToken sets the address (CBS endpoint) that this put token operation is targeted to.]*/
    else if (pn_message_set_reply_to(transportState->message, CBS_REPLY_TO) != 0)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_135: [If this fails then putToken fails.]*/
        LogError("unable to pn_message_set_reply_to\r\n");
        result = __LINE__;
    }
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_136: [putToken sets the reply to address for the put token operation.]*/
    else if (pn_message_set_address(transportState->message, STRING_c_str(transportState->cbsAddress)) != 0)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_137: [If this fails then putToken fails.]   */
        LogError("unable to pn_message_set_address\r\n");
        result = __LINE__;
    }
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_138: [putToken sets the message to not be inferred.]*/
    else if (pn_message_set_inferred(transportState->message, false) != 0)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_139: [If this fails then putToken fails.]*/
        LogError("unable to pn_message_set_inferred\r\n");
        result = __LINE__;
    }
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_140: [putToken sets the messageId of the message to the expiry of the token.]*/
    else if (pn_message_set_id(transportState->message, messageId) != 0)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_141: [If this fails then putToken fails.]*/
        LogError("Unable to set the message id on the transfer of the token to the CBS\r\n");
        result = __LINE__;
    }
    else
    {
        pn_data_t* body;
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_142: [putToken obtains the body of the message.]*/
        body = pn_message_body(transportState->message);
        if (body == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_143: [If this fails then putToken fails.]*/
            LogError("Unable to pn_message_body\r\n");
            result = __LINE__;
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_144: [putToken places the actual SAS token into the body of the message.]*/
        else if (pn_data_put_string(body, pn_bytes(STRING_length(token), STRING_c_str(token))) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_145: [If this fails then putToken fails.]*/
            LogError("Unable to pn_data_put_string\r\n");
            result = __LINE__;
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_146: [putToken invokes pn_messenger_put.]*/
        else if (pn_messenger_put(transportState->messenger, transportState->message) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_147: [If this fails then putToken fails.]*/
            LogError("Unable to pn_messenger_put\r\n");
            result = __LINE__;
            transportState->messengerInitialized = false;
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_148: [A proton tracker is obtained.]*/
            pn_tracker_t tracker = pn_messenger_outgoing_tracker(transportState->messenger);
            time_t beginningOfWaitLoop = get_time(NULL);
            time_t currentTime;
            result = __LINE__;
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_149: [putToken will loop invoking pn_messenger_work waiting for the message to reach a terminal delivery state.]*/
            do
            {
                int protonResult;
                if (checkForErrorsThenWork(transportState) == false)
                {
                    break;
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_150: [If a terminal delivery state is not reached then putToken fails.]*/
                    if ((protonResult = pn_messenger_status(transportState->messenger, tracker)) != PN_STATUS_PENDING)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_151: [On reaching a terminal delivery state, if the message status is NOT accepted then putToken fails.]*/
                        if (protonResult != PN_STATUS_ACCEPTED)
                        {
                            LogError("Error sending the token: %s\r\n", pn_error_text(pn_messenger_error(transportState->messenger)));
                            result = __LINE__;
                            transportState->messengerInitialized = false;
                        }
                        else
                        {
                            result = 0;
                        }
                        break;
                    }
                }
                currentTime = get_time(NULL);
            } while (difftime(currentTime, beginningOfWaitLoop) < transportState->cbsRequestAcceptTime);

            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_152: [The tracker is settled.]*/
            pn_messenger_settle(transportState->messenger, tracker, 0);
            if (result == 0)
            {
                //
                // Successfully sent the token.  Wait around for a reply.
                //
                transportState->waitingForPutTokenReply = true;
                beginningOfWaitLoop = get_time(NULL);
                result = __LINE__;
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_153: [putToken will again loop waiting for a reply.]*/
                do
                {
                    processReceives(transportState);
                    if (transportState->waitingForPutTokenReply == true)
                    {
                        if (checkForErrorsThenWork(transportState) == false)
                        {
                            break;
                        }
                    }
                    else
                    {
                        result = 0;
                        break;
                    }
                    currentTime = get_time(NULL);
                } while (difftime(currentTime, beginningOfWaitLoop) < transportState->cbsReplyTime);
            }
            else
            {
                LogError("An network error occured while waiting for a CBS reply.\r\n");
                transportState->messengerInitialized = false;
            }
        }
    }
    pn_message_clear(transportState->message);
    return result;
}

static void renewIfNecessaryTheCBS(PAMQP_TRANSPORT_STATE transportState)
{

    //
    // There is an expiry stored in the state.  It has the last expiry value used to define a
    // sas token.  If we are within 20 minutes OR we're PAST that expiration generate a new
    // expiry time and create a new sas token and "put" it to the cbs.
    //

    size_t secondsSinceEpoch;
    int differenceWithLastExpiry;

    transportState->putTokenWasSuccessful = false;
    //
    // Get the number of seconds since the epoch.
    //
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_163: [Invocation of renewIfNecessaryTheCBS will first obtain the current number of seconds since the epoch.*/
    secondsSinceEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE)+0); // adding zero because a compiler feels it's necessary.
    differenceWithLastExpiry = transportState->lastExpiryUsed - secondsSinceEpoch;
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_164: [If the difference between lastExpiryUsed and the current value is less than refresh seconds then renewIfNecessaryTheCBS will attempt to renew the SAS.]*/
    if ((differenceWithLastExpiry <= 0) ||
        (((size_t)differenceWithLastExpiry) < transportState->sasRefreshLine)) // Within refresh minutes (or past if negative) of the expiration
    {
        //
        // We already have the seconds since the epoch.  Add an hour to it and generate the new SAS.
        //
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_165: [If SAS renewal is to occur then lifetime seconds is added to the current number of seconds.]*/
        size_t possibleNewExpiry = secondsSinceEpoch + transportState->sasTokenLifetime;
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_169: [SASToken_Create is invoked.]   */
        STRING_HANDLE newSASToken = SASToken_Create(transportState->deviceKey, transportState->devicesPortionPath, transportState->zeroLengthString, possibleNewExpiry);
        if (newSASToken == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_170: [If SASToken_Create returns NULL then renewal is considered a failure and we move on to the next phase of DoWork.]*/
            LogError("Could not generate a new SAS token for the CBS\r\n");
        }
        else
        {
            //
            // We are going to save off in the transport state this POSSIBLE expiry value.  Sending token and response processing code
            // will use it as the message id and correlation id for the request/response.  This will work
            // because the code that sends the token will wait for AT LEAST one second for a valid response.  If a valid response comes
            // back in that period of time then even if for some bizarre reason we need to renew again in less than a second, we know
            // there won't be a spurious response (because we just got the resonse!).
            //
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_166: [This value is saved in the state as sendTokenMessageId.]*/
            transportState->sendTokenMessageId = possibleNewExpiry;
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_171: [renewIfNecessaryTheCBS will invoke a local static function putToken.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_172: [If putToken fails then the renewal is considered a failure and we move on to the next phase of DoWork.]*/
            if (putToken(transportState, newSASToken) == 0)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_173: [Note that putToken actually sets a state value which will indicate whether the CBS accepted the token and responded with success.]*/
                if (transportState->putTokenWasSuccessful == true)
                {
                    transportState->lastExpiryUsed = possibleNewExpiry;
                }
            }
            STRING_delete(newSASToken);
        }
    }
}

static bool protonMessengerInit(PAMQP_TRANSPORT_STATE amqpState)
{
    if (!amqpState->messengerInitialized)
    {
        rollbackEvent(amqpState);
        disposeOfOldMessenger(amqpState);
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_027: [DoWork shall invoke pn_messenger to create a new messenger.]*/
        if ((amqpState->messenger = pn_messenger(NULL)) == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_028: [If pn_messenger returns NULL then the messenger initialization fails.]*/
            LogError("The messenger is not able to be created.\r\n");
        }
        /* Codes_SRS_IOTHUBTRANSPORTTAMQP_01_001: [If the option "TrustedCerts" has been set (length greater than 0), the trusted certificates string shall be passed to Proton by a call to pn_messenger_set_trusted_certificates.] */
        else if ((amqpState->trustedCertificates != NULL) &&
            (strlen(amqpState->trustedCertificates) > 0) &&
            (pn_messenger_set_trusted_certificates(amqpState->messenger, amqpState->trustedCertificates) != 0))
        {
            /* Codes_SRS_IOTHUBTRANSPORTTAMQP_01_002: [If pn_messenger_set_trusted_certificates fails, then messenger initialization shall fail.] */
            LogError("unable to pass certificate information via pn_messenger_set_trusted_certificates\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_029: [pn_messenger_start shall be invoked.]*/
        else if (pn_messenger_start(amqpState->messenger) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_030: [If pn_messenger_start returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to pn_messenger_start\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_031: [pn_messenger_set_blocking shall be invoked.]*/
        else if (pn_messenger_set_blocking(amqpState->messenger, false) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_032: [If pn_messenger_set_blocking returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to pn_messenger_set_blocking\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_033: [pn_messenger_set_snd_settle_mode shall be invoked.]*/
        else if (pn_messenger_set_snd_settle_mode(amqpState->messenger, PN_SND_UNSETTLED) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_034: [If pn_messenger_set_snd_settle_mode returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to set the send settle mode\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_035: [pn_messenger_set_rcv_settle_mode shall be invoked.]*/
        else if (pn_messenger_set_rcv_settle_mode(amqpState->messenger, PN_RCV_FIRST) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_036: [If pn_messenger_set_rcv_settle_mode returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to set the receive settle mode\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_037: [pn_messenger_set_outgoing_window shall be invoked.]*/
        else if (pn_messenger_set_outgoing_window(amqpState->messenger, PROTON_OUTGOING_WINDOW_SIZE) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_038: [If pn_messenger_set_outgoing_window returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to pn_messenger_set_outgoing_window\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_039: [pn_messenger_set_incoming_window shall be invoked.]*/
        else if (pn_messenger_set_incoming_window(amqpState->messenger, PROTON_INCOMING_WINDOW_SIZE) != 0)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_040: [If pn_messenger_set_incoming_window returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to pn_messenger_set_incoming_window\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_156: [pn_messenger_subscribe will be invoked on the cbs address.]*/
        else if ((amqpState->cbsSubscription = pn_messenger_subscribe(amqpState->messenger, (const char*)STRING_c_str(amqpState->cbsAddress))) == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_157: [If the pn_messenger_subscribe returns a non-zero value then messenger initialization fails.]*/
            LogError("unable to create a subscription to the cbs address\r\n");
        }
        else
        {
            //We've got the subscription.  Now we attempt to connect to the cbs.
            /*Codes_During messenger initialization a state variable known as lastExpiryUsed will be set to zero.*/
            amqpState->lastExpiryUsed = 0;
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_159: [renewIfNecessaryCBS will be invoked.]*/
            renewIfNecessaryTheCBS(amqpState);
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_160: [If after renewIfNecessaryCBS is invoked, the state variable putTokenWasSuccessful is false then the messenger initialization fails.]*/
            if (amqpState->putTokenWasSuccessful == true)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_041: [pn_messenger_subscribe shall be invoked.]*/
                if ((amqpState->messageSubscription = pn_messenger_subscribe(amqpState->messenger, (const char*)STRING_c_str(amqpState->messageAddress))) == NULL)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_042: [If pn_messenger_subscribe returns a non-zero value then messenger initialization fails.]*/
                    LogError("unable to create a subscription to the message address\r\n");
                }
                else
                {
                    amqpState->messengerInitialized = true;
                }
            }
        }
    }
    return amqpState->messengerInitialized;
}

static amqp_batch_result prepareBatch(size_t maximumPayload, PDLIST_ENTRY waitingToSend, PDLIST_ENTRY messagesMakingUpBatch, size_t* payloadSize, const unsigned char** payload)
{
    amqp_batch_result result;
    if (DList_IsListEmpty(waitingToSend))
    {
        result = amqp_batch_nowork;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
        IOTHUB_MESSAGE_LIST* currentMessage;
        IOTHUBMESSAGE_CONTENT_TYPE contentType;
        size_t messageLength;
        const unsigned char* messagePayload;
        DList_InitializeListHead(messagesMakingUpBatch);
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_086: [If the pn_messenger_put succeeds then DoWork will save off a tracker obtained by invoking pn_messenger_outgoing_tracker and save it the head of the availableWorkItems.  The eventMessages in the AMQP_WORK_ITEM head will also contain the IOTHUB_MESSAGE_LIST.]*/
        DList_InsertTailList(messagesMakingUpBatch, DList_RemoveHeadList(waitingToSend));
        currentMessage = containingRecord(messagesMakingUpBatch->Flink, IOTHUB_MESSAGE_LIST, entry);
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetByteArray.]*/
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_008: [If the message type is IOTHUBMESSAGE_STRING then get the data by using IoTHubMesage_GetString and set the size to the length of the string.] */
        contentType = IoTHubMessage_GetContentType(currentMessage->messageHandle);
        if (!(
            ((contentType == IOTHUBMESSAGE_BYTEARRAY) && (IoTHubMessage_GetByteArray(currentMessage->messageHandle, &messagePayload, &messageLength) == IOTHUB_MESSAGE_OK)) ||
            ((contentType == IOTHUBMESSAGE_STRING) && (
                messagePayload = (const unsigned char*)IoTHubMessage_GetString(currentMessage->messageHandle),
                (messageLength = (messagePayload == NULL) ? 0 : strlen((const char*)messagePayload)),
                messagePayload != NULL)
                )
            ))
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_076: [If that fails then fail that event.]*/
            LogError("Failure in getting message content. Type had decimal value = %d\r\n", contentType);
            result = amqp_batch_error;
        }
        else if (messageLength > maximumPayload)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_077: [If the size of the payload is greater than the maximum (currently 256*1024) fail that event.]*/
            LogError("Batch length is too large.\r\n");
            result = amqp_batch_error;
        }
        else
        {
            *payloadSize = messageLength;
            *payload = messagePayload;
            result = amqp_batch_nobatch;
        }
    }
    return result;
}

static bool prepareSimpleProtonMessage(pn_message_t* message, STRING_HANDLE endpoint, size_t payloadLength, const unsigned char* payload)
{
    bool result = false;
    pn_data_t * body;

    pn_message_clear(message);
    if (pn_message_set_address(message, STRING_c_str(endpoint)) != 0)
    {
        LogError("Unable to set amqp address for proton message.\r\n");
    }
    else if (pn_message_set_inferred(message, true) != 0)
    {
        LogError("Unable to set inferred to true for proton message.\r\n");
    }
    else if ((body = pn_message_body(message)) == NULL)
    {
        LogError("Unable to set proton message body.\r\n");
    }
    else if (pn_data_put_binary(body, pn_bytes(payloadLength, (const char*)payload)) != 0)
    {
        LogError("Unable to set binary data for message body.\r\n");
    }
    else
    {
        result = true;
    }

    return result;
}

static void rollbackEvent(PAMQP_TRANSPORT_STATE transportState)
{
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_175: [The list of work in progress is tested to see if it is empty.]*/
    while (!DList_IsListEmpty(&transportState->workInProgress))
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_176: [If it is not empty then the list is traversed.]*/
        int settleResult;
        int protonResult;
        PDLIST_ENTRY newestEntry = transportState->workInProgress.Blink;
        PAMQP_WORK_ITEM currentWork = containingRecord(newestEntry, AMQP_WORK_ITEM, link);
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_179: [The status of the work item is acquired.]*/
        protonResult = pn_messenger_status(transportState->messenger, currentWork->tracker);
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_180: [The item is settled.]*/
        if ((settleResult = pn_messenger_settle(transportState->messenger, currentWork->tracker, 0)) != 0)
        {
            LogError("Attempt to settle a tracker produced an error: %d\r\n", settleResult);
        }
        if ((protonResult == PN_STATUS_ACCEPTED) ||
            (protonResult == PN_STATUS_REJECTED) ||
            (protonResult == PN_STATUS_RELEASED))
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_182: [Items that have been settled by the service will have their IOTHUB_MESSAGE_LIST completed.]*/
            IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &currentWork->eventMessages, (protonResult == PN_STATUS_ACCEPTED) ? IOTHUB_BATCHSTATE_SUCCESS : IOTHUB_BATCHSTATE_FAILED);
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_178: [Items that are not settled by the service will have their IOTHUB_MESSAGE_LIST removed from the work item and placed back on the waiting to send.]*/
            putSecondListAfterHeadOfFirst(transportState->waitingToSend->Flink, &currentWork->eventMessages);
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_181: [The work item is removed from the in progress list and placed on the available list.]*/
        DList_RemoveEntryList(newestEntry);
        DList_InsertTailList(&transportState->availableWorkItems, newestEntry);
    }
}
static void reclaimEventResources(PAMQP_TRANSPORT_STATE transportState)
{
    PDLIST_ENTRY currentListEntry;
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_064: [Obtain the head of the workInProgress]*/
    currentListEntry = transportState->workInProgress.Flink;
    while (currentListEntry != &transportState->workInProgress)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_065: [While head != listhead reclaimEventResources will do as follows.] */
        int protonResult;
        PAMQP_WORK_ITEM currentWork = containingRecord(currentListEntry, AMQP_WORK_ITEM, link);
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_066: [Check the current status via pn_messenger_status via the tracker stored in the AMQP_WORK_ITEM.]*/
        if ((protonResult = pn_messenger_status(transportState->messenger, currentWork->tracker)) == PN_STATUS_PENDING)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_183: [if the amount of time that the work item has been pending exceeds a timeout stored in the work item, the messenger will be marked as not initialized.]*/
            size_t secondsSinceTheEpoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
            if (currentWork->expiry < secondsSinceTheEpoch)
            {
                LogError("A event operation timed out.\r\n");
                transportState->messengerInitialized = false;
            }
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_067: [reclaimEventResources shall go onto the next work item if the proton status is PN_STATUS_PENDING.]*/
            currentListEntry = currentListEntry->Flink;
        }
        else
        {
            DLIST_ENTRY savedFromCurrentListEntry;
            int settleResult;
            // Yay! It's finished.  Take it off the in progress list.
            savedFromCurrentListEntry.Flink = currentListEntry->Flink; // We need to save this because it will be stomped on when we remove it from the work in progress.

            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_068: [Otherwise reclaimEventResources will use the result of IOTHUB_BATCHSTATE_SUCCESS if the proton status was PN_STATUS_ACCEPTED.]*/
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_069: [Otherwise reclaimEventResources will use the status of IOTHUB_BATCHSTATE_FAILED if the proton status was NOT PN_STATUS_ACCEPTED.] */
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_070: [reclaimEventResources will invoke IotHubClient_SendBatchComplete with the IOTHUB_CLIENT_LL_HANDLE, supplied to reclaimEventResources, also pass the eventMessages stored in the AMQP_WORK_ITEM, and finally pass the above IOTHUB_BATCH status.]*/
            IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &currentWork->eventMessages, (protonResult == PN_STATUS_ACCEPTED) ? IOTHUB_BATCHSTATE_SUCCESS : IOTHUB_BATCHSTATE_FAILED);
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_090: [reclaimEventResources will release the tracker by invoking pn_messenger_settle.]*/
            if ((settleResult = pn_messenger_settle(transportState->messenger, currentWork->tracker, 0)) != 0)
            {
                LogError("Attempt to settle a tracker produced an error: %d\r\n", settleResult);
            }
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_071: [reclaimEventResources will take the current work item from the workInProgress list and insert it on the tail of the availableWorkItems list.]*/
            DList_RemoveEntryList(currentListEntry);
            DList_InsertTailList(&transportState->availableWorkItems, currentListEntry);
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_072: [reclaimEventResources will continue to iterate over the workInProgress list until it returns to the list head.]*/
            currentListEntry = savedFromCurrentListEntry.Flink;
        }
    }
}

static void sendEvent(PAMQP_TRANSPORT_STATE transportState)
{
    while (!DList_IsListEmpty(&transportState->availableWorkItems))
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
        PAMQP_WORK_ITEM headOfAvailable = containingRecord(transportState->availableWorkItems.Flink, AMQP_WORK_ITEM, link);
        size_t payloadSize;
        const unsigned char* payloadAddress;
        amqp_batch_result amqpBatchResult;
        amqpBatchResult = prepareBatch(MAXIMUM_EVENT_LENGTH, transportState->waitingToSend, &headOfAvailable->eventMessages, &payloadSize, &payloadAddress);
        if (amqpBatchResult == amqp_batch_nowork)
        {
            // no work to do.  Stop trying.
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_089: [If there is nothing on the list then go on to the message.]*/
            break;
        }
        else if (amqpBatchResult == amqp_batch_error)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_078: [If the event failed call invoke IotHubClient_SendBatchComplete as above utilizing the eventMessages list of the head of the availableWorkItems and a status of IOTHUB_BATCHSTATE_FAILED.]*/
            IoTHubClient_LL_SendComplete(transportState->savedClientHandle, &headOfAvailable->eventMessages, IOTHUB_BATCHSTATE_FAILED);
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_079: [Continue this send loop.]*/
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
            if (prepareSimpleProtonMessage(transportState->message, transportState->eventAddress, payloadSize, payloadAddress) == false)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
                putSecondListAfterHeadOfFirst(transportState->waitingToSend, &headOfAvailable->eventMessages);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
                break;
            }
            else if (setProperties(&headOfAvailable->eventMessages, transportState->message) != 0)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
                putSecondListAfterHeadOfFirst(transportState->waitingToSend, &headOfAvailable->eventMessages);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
                break;
            }
            else if (setSendMessageIds(&headOfAvailable->eventMessages, transportState->message))
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
                putSecondListAfterHeadOfFirst(transportState->waitingToSend, &headOfAvailable->eventMessages);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
                break;
            }
            else
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_083: [If the proton API is successful then invoke pn_messenger_put.]*/
                int protonResult;
                protonResult = pn_messenger_put(transportState->messenger, transportState->message);
                if (protonResult != 0)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_084: [If that fails, sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
                    putSecondListAfterHeadOfFirst(transportState->waitingToSend, &headOfAvailable->eventMessages);
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_085: [sendEvent will then break out of the send loop.]*/
                    transportState->messengerInitialized = false;
                    break;
                }
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_184: [The work item will also contain a timeout that shall denote how long the transport will wait before initiating error recovery.]*/
                headOfAvailable->expiry = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + transportState->eventTimeout);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_086: [If the pn_messenger_put succeeds then sendEvent will save off a tracker obtained by invoking pn_messenger_outgoing_tracker and save it the head of the availableWorkItems.  The eventMessages in the AMQP_WORK_ITEM head will also contain the IOTHUB_MESSAGE_LIST.]*/
                headOfAvailable->tracker = pn_messenger_outgoing_tracker(transportState->messenger);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_087: [The head of the availableWorkItems will be removed and placed on the workInProgress list.]*/
                DList_InsertTailList(&transportState->workInProgress, DList_RemoveHeadList(&transportState->availableWorkItems));
            }
        }
    }
}
static void processCBSReply(PAMQP_TRANSPORT_STATE transportState, pn_tracker_t tracker)
{
    bool goodMessage = false;
    pn_atom_t correlationId;

    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_112: [The correlationId message property is obtained via a call to pn_message_get_correlation_id.]*/
    correlationId = pn_message_get_correlation_id(transportState->message);

    //
    // If it isn't the one we're looking for, simply accept it, because we don't want to see it and we can't do anything with it.
    //

    if (correlationId.u.as_ulong != transportState->sendTokenMessageId)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_113: [The id is compared to the state variable sendTokenMessageId.  If there is no match, an error is logged, the message is abandoned and processCBSReply returns.]*/
        LogError("An old reply from a CBS renewal drifed in.  Moving on\r\n");
    }
    else
    {
        //
        // This is the response we're looking for!
        //

        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_114: [Obtain the properties of the message by invoking pn_message_properties.]*/
        pn_data_t* propertyData = pn_message_properties(transportState->message);

        transportState->waitingForPutTokenReply = false;
        if (propertyData == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_115: [If NULL then abandon the message and return.]*/
            LogError("Invalid response back from the CBS - no application properties\r\n");
        }
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_116: [Advance to the first property of the message via pn_data_next.]*/
        else if (pn_data_next(propertyData) == false)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_117: [If this fails, abandon and return.]*/
            LogError("Invalid response back from the CBS - application properties malformed\r\n");
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_118: [Invoke pn_data_get_map to obtain the number of properties.]*/
            size_t numberOfProperties = pn_data_get_map(propertyData) / 2;
            if (numberOfProperties < 1) // We MUST have at least the status property
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_119: [If the number of properties is 0 then abandon and return.]*/
                LogError("Invalid response back from the CBS - invalid number of properties: %zu\r\n", pn_data_get_map(propertyData));
            }
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_120: [Enter the property map by invoking pn_data_enter.]*/
            else if (pn_data_enter(propertyData) == false)
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_121: [If that fails then abandon and return.]*/
                LogError("Invalid response back from the CBS - unable to access the sent properties\r\n");
            }
            else
            {
                pn_bytes_t propertyName;
                int32_t resultStatus;
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_122: [Get the property name.]*/
                if (getMapString(propertyData, &propertyName) != 0)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_123: [If that fails then abandon and return.]*/
                    LogError("Unable to acquire the status value from the cbs request\r\n");
                }
                else if (strcmp(propertyName.start, PROTON_MAP_STATUS_CODE) != 0)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_124: [If the property name is not "status-code" than abandon and return.]*/
                    LogError("Unknown application property returned: %s\r\n", propertyName.start);
                }
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_125: [Get the actual status code.]*/
                else if (getMapInt(propertyData, &resultStatus) != 0)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_126: [If that fails then abandon and return.]*/
                    LogError("Unable to acquire the actual status of the CBS put token\r\n");
                }
                else if (resultStatus != 200)
                {
                    pn_bytes_t statusDescription;
                    //
                    // Yeah, it's not what we want to hear but it's not as though anything was wrong with
                    // the message.
                    //
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_127: [If the status code is not equal to 200 then attempt to acquire the explanation from the properties.]*/
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_128: [Accept the message and return.]*/
                    goodMessage = true;
                    LogError("The CBS put token operation failed: %d\r\n", resultStatus);
                    if (numberOfProperties >= 2)
                    {
                        //
                        // Well we have an error.  See if the next property is the status description.
                        // If it is, then log the value as further explanation
                        //
                        if (getMapString(propertyData, &propertyName) == 0)
                        {
                            if (strcmp(propertyName.start, PROTON_MAP_STATUS_DESCRIPTION) == 0)
                            {
                                if (getMapString(propertyData, &statusDescription) == 0)
                                {
                                    LogError("The CBS put token operation failed due to: %s\r\n", statusDescription.start);
                                }
                            }
                        }
                    }
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_129: [If the status code was 200 then set the state variable putTokenWasSuccessful, accept the message and return.]*/
                    goodMessage = true;
                    transportState->putTokenWasSuccessful = true;
                }
            }
        }
    }
    if (goodMessage == true)
    {
        pn_messenger_accept(transportState->messenger, tracker, 0);
    }
    else
    {
        pn_messenger_release(transportState->messenger, tracker, 0);
    }
    pn_messenger_settle(transportState->messenger, tracker, 0);
}

static void processMessage(PAMQP_TRANSPORT_STATE transportState, pn_tracker_t tracker)
{
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_111: [If the state variable DoWork_PullMessage is false then the message shall be rejected and processMessage shall return.]*/
    if (transportState->DoWork_PullMessages == false)
    {
        //
        // Nothing to do with the message.  We reject it.  In that way it can come back later.
        //
        pn_messenger_reject(transportState->messenger, tracker, 0);
        pn_messenger_settle(transportState->messenger, tracker, 0);
    }
    else
    {
        pn_data_t *body;

        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_050: [processMessage will acquire the body of the message by invoking pn_message_body.]*/
        if ((body = pn_message_body(transportState->message)) == NULL)
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_051: [If the body is NULL then the message will be abandoned.]*/
            LogError("Failed to get the proton message body\r\n");
            pn_messenger_release(transportState->messenger, tracker, 0);
            pn_messenger_settle(transportState->messenger, tracker, 0);
        }
        else
        {
            pn_bytes_t sizeAndData;
            bool keepProcessingThisMessage = true;
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_054: [processMessage will check for a null (note the lowercase) body by invoking pn_data_next.]*/
            if (pn_data_next(body) == false)
            {
                //
                // We have a null body.  This is reasonable.  It should also be pointed out that
                // pn_data_next call shouldn't be thought of as checking for the existence of something.
                // It should be thought of as actually MOVING through a tree structure in the message.
                //
                sizeAndData.size = 0;
                sizeAndData.start = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_055: [If the body is NOT null then processMessage will check that the body is of type PN_BINARY.]*/
                if (PN_BINARY != pn_data_type(body))
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_056: [processMessage will abandon the message with body types that are NOT PN_BINARY, the IOTHUB_MESSAGE_HANDLE will be destroyed and the loop will be continued.]*/
                    LogError("Message received via AMQP was not PN_BINARY\r\n");
                    sizeAndData.size = 0;           // Compilers get upset.
                    sizeAndData.start = NULL;
                    pn_messenger_release(transportState->messenger, tracker, 0);
                    pn_messenger_settle(transportState->messenger, tracker, 0);
                    keepProcessingThisMessage = false;
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_057: [The length and a pointer to the payload will be acquired and saved by invoking pn_data_get_binary.]*/
                    sizeAndData = pn_data_get_binary(body);
                }
            }
            if (keepProcessingThisMessage == true)
            {
                IOTHUB_MESSAGE_HANDLE ioTMessage;
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_058: [The IOTHUB_MESSAGE_HANDLE will be set by invoking IotHubMessage_SetData with the previously saved length and pointer to payload.]*/
                if ((ioTMessage = IoTHubMessage_CreateFromByteArray((const unsigned char*)sizeAndData.start, sizeAndData.size)) == NULL)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_059: [processMessage will abandon the message if IotHubMessage_SetData returns any status other than IOTHUB_MESSAGE_OK.]*/
                    LogError("Failed to set data for IoT hub message\r\n");
                    pn_messenger_release(transportState->messenger, tracker, 0);
                    pn_messenger_settle(transportState->messenger, tracker, 0);
                }
                else if (cloneProperties(ioTMessage, transportState->message) != 0)
                {
                    LogError("Failed to clone properties for IoT hub message\r\n");
                    pn_messenger_release(transportState->messenger, tracker, 0);
                    pn_messenger_settle(transportState->messenger, tracker, 0);
                }
                /* Codes_SRS_IOTHUBTRANSPORTTAMQP_07_003: [If the pn_message_get_id value is not NULL then the value will be set by calling IotHubMessage_SetMessageId.] */
                else if (setRecvMessageIds(ioTMessage, transportState->message) != 0)
                {
                    LogError("Failed to set MessageId for IoT hub message\r\n");
                    pn_messenger_release(transportState->messenger, tracker, 0);
                    pn_messenger_settle(transportState->messenger, tracker, 0);
                }
                else
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_060: [If IOTHUB_MESSAGE_OK had been returned IoTHubClient_LL_MessageCallback will be invoked with the IOTHUB_MESSAGE_HANDLE.]*/
                    IOTHUBMESSAGE_DISPOSITION_RESULT upperLayerDisposition = IoTHubClient_LL_MessageCallback(transportState->savedClientHandle, ioTMessage);

                    if (upperLayerDisposition == IOTHUBMESSAGE_ACCEPTED)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_006: [If IoTHubClient_MessageCallback returns IOTHUBMESSAGE_ACCEPTED value then the message will be accepted.] */
                        pn_messenger_accept(transportState->messenger, tracker, 0);
                    }
                    else if (upperLayerDisposition == IOTHUBMESSAGE_ABANDONED)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_007: [If IoTHubClient_MessageCallback returns IOTHUBMESSAGE_ABANDONED value then the message will be abandoned.] */
                        pn_messenger_release(transportState->messenger, tracker, 0);
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_061: [If IoTHubClient_MessageCallback returns IOTHUBMESSAGE_REJECTED value then the message will be rejected.]*/
                        pn_messenger_reject(transportState->messenger, tracker, 0);
                    }
                    pn_messenger_settle(transportState->messenger, tracker, 0);
                }
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_063: [processMessage will destroy the IOTHUB_MESSAGE_HANDLE by invoking IoTHubMessage_Destroy.]*/
                IoTHubMessage_Destroy(ioTMessage);
            }
        }
    }
}

static void processReceives(TRANSPORT_HANDLE handle)
{
    PAMQP_TRANSPORT_STATE transportState = (PAMQP_TRANSPORT_STATE)handle;
    int receiveResult;

    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_043: [If DoWork_PullMessage is false then DoWork will cancel any preceding messages by invoking pn_messenger_recv.]*/

    if ((transportState->DoWork_PullMessages == false) && (transportState->waitingForPutTokenReply == false))
    {
        if ((receiveResult = pn_messenger_recv(transportState->messenger, 0)) != 0)
        {
            if ((receiveResult != PN_INPROGRESS) && (receiveResult != PN_TIMEOUT))
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_046: [If pn_messenger_recv fails it will be logged.]*/
                LogError("Error attempting to receive messages: %d\r\n", receiveResult);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_105: [processReceives will then go on to the next action of DoWork.]*/
                transportState->messengerInitialized = false;
            }
        }
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_044: [If the DoWork_PullMessage or the waitingForPutTokenReply flag is true then processReceives will pull messages with pn_messenger_recv.]*/
        if ((receiveResult = pn_messenger_recv(transportState->messenger, PROTON_INCOMING_WINDOW_SIZE)) != 0)
        {
            if ((receiveResult != PN_INPROGRESS) && (receiveResult != PN_TIMEOUT))
            {
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_046: [If pn_messenger_recv fails it will be logged.]*/
                LogError("Error attempting to receive messages: %d\r\n", receiveResult);
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_105: [processReceives will then go on to the next action of DoWork.]*/
                transportState->messengerInitialized = false;
            }
        }
        else
        {
            //
            // If any are present we need to dispose of messages in the incoming queue.
            //
            size_t depthOfReceiveQueue;
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_045: [processReceives will check the number of messages received by invoking pn_messenger_incoming.]*/
            for (depthOfReceiveQueue = pn_messenger_incoming(transportState->messenger); depthOfReceiveQueue > 0; depthOfReceiveQueue--)
            {
                int result;
                pn_message_clear(transportState->message);
                //
                // Ok something is there.  First thing is to get it.
                //
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_047: [processReceives will retrieve a message with pn_messenger_get.]*/
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_048: [If pn_messenger_get fails we log and break the receive loop.]*/
                if ((result = pn_messenger_get(transportState->messenger, transportState->message)) != 0)
                {
                    //
                    // Well that's pretty odd.  We shouldn't have gotten here if the message queue was empty!
                    //
                    LogError("Message reception queue unexpectedly empty! Code: %d\r\n", result);
                    break;
                }
                else
                {
                    //
                    // We'll need a tracker to dispose of the message.  We need the subscription to determine who should process it.
                    //
                    pn_subscription_t* theMessageSource;
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_049: [processReceives shall acquire a tracker by invoking pn_messenger_incoming_tracker.]*/
                    pn_tracker_t tracker = pn_messenger_incoming_tracker(transportState->messenger);

                    //
                    // Currently we can receive two types of messages.  We can recieve status messages from the $CBS that indicates the success
                    // or failure of renewing the SAS for the device.
                    //
                    // Otherwise we can recieve a simple message.  We can get a pointer to the subscription from the tracker.  We can compare
                    // that pointer to the subscriptions that we stored when we initialized the messenger.  We then dispatch appropriately.
                    //

                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_106: [processReceives acquires the subscription of the message by invoking pn_messneger_incoming_subscription.] This value is henceforth known as theMessageSource.*/
                    theMessageSource = pn_messenger_incoming_subscription(transportState->messenger);
                    if (theMessageSource == transportState->messageSubscription)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_108: [If theMessageSource is equal to the state variable cbsSubscription invoke processCBSReply.]*/
                        processMessage(transportState, tracker);
                    }
                    else if (theMessageSource == transportState->cbsSubscription)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_108: [If theMessageSource is equal to the state variable cbsSubscription invoke processCBSReply.]*/
                        processCBSReply(transportState, tracker);
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_109: [Otherwise, log an error and abandon the message so that it never returns.]*/
                        LogError("Message received from an unknown sender\r\n");
                        pn_messenger_release(transportState->messenger, tracker, 0);
                        pn_messenger_settle(transportState->messenger, tracker, 0);
                    }
                }
            }
            pn_message_clear(transportState->message);
        }
    }
}

static void clientTransportAMQP_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    PAMQP_TRANSPORT_STATE transportState = (PAMQP_TRANSPORT_STATE)handle;

    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_019: [clientTransportAMQP_Dowork shall do no work if handle is NULL.]*/
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_020: [clientTransportAMQP_Dowork shall do no work if iotHubClientHandle is NULL.]*/
    if (transportState && iotHubClientHandle)
    {
        transportState->savedClientHandle = iotHubClientHandle;
        if (protonMessengerInit(transportState))
        {
            renewIfNecessaryTheCBS(transportState);
            if (transportState->messengerInitialized == true) reclaimEventResources(transportState);
            if (transportState->messengerInitialized == true) sendEvent(transportState);
            if (transportState->messengerInitialized == true) processReceives(transportState);

            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_088: [pn_messenger_work shall be invoked following all the above actions.]*/
            if (transportState->messengerInitialized == true) checkForErrorsThenWork(transportState);
        }

        //
        // Try to get rid of any old messengers that we were NOT able to stop.
        //
        herdTheMessengers(transportState, false);
    }
    return;
}

static bool createUrledDeviceId(PAMQP_TRANSPORT_STATE state, const IOTHUBTRANSPORT_CONFIG* config)
{
    return ((state->urledDeviceId = URL_EncodeString(config->upperConfig->deviceId)) == NULL) ? (false) : (true);
}
static bool createDevicesPortionPath(PAMQP_TRANSPORT_STATE state, const char* host)
{
    return (((state->devicesPortionPath = STRING_construct(host)) == NULL) ||
            (STRING_concat(state->devicesPortionPath, "/devices/") != 0) ||
            (STRING_concat_with_STRING(state->devicesPortionPath, state->urledDeviceId) != 0)) ? (false) : (true);
}
static bool createEventAddress(PAMQP_TRANSPORT_STATE state)
{
    return (((state->eventAddress = STRING_construct(AMQPS_SCHEME)) == NULL) ||
        (STRING_concat_with_STRING(state->eventAddress, state->devicesPortionPath) != 0) ||
        (STRING_concat(state->eventAddress, EVENT_ENDPOINT) != 0)) ? (false) : (true);
}

static bool createMessageAddress(PAMQP_TRANSPORT_STATE state)
{
    return (((state->messageAddress = STRING_construct(AMQPS_SCHEME)) == NULL) ||
        (STRING_concat_with_STRING(state->messageAddress, state->devicesPortionPath) != 0) ||
        (STRING_concat(state->messageAddress, MESSAGE_ENDPOINT) != 0)) ? (false) : (true);
}

static bool createCbsAddress(PAMQP_TRANSPORT_STATE state, const char* host)
{
    return (((state->cbsAddress = STRING_construct(AMQPS_SCHEME)) == NULL) ||
        (STRING_concat(state->cbsAddress,host) != 0) ||
        (STRING_concat(state->cbsAddress, CBS_ENDPOINT) != 0)) ? (false) : (true);
}

static STRING_HANDLE createHost(const IOTHUBTRANSPORT_CONFIG* config)
{
    STRING_HANDLE result = NULL;
    if (((result = STRING_construct(config->upperConfig->iotHubName)) == NULL) ||
        (STRING_concat(result, ".") != 0) ||
        (STRING_concat(result, config->upperConfig->iotHubSuffix) != 0))
    {
        if (result != NULL)
        {
            STRING_delete(result);
            result = NULL;
        }
    }
    return result;
}

static TRANSPORT_HANDLE clientTransportAMQP_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    PAMQP_TRANSPORT_STATE amqpState = NULL;
    if (!config)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_001: [If parameter config is NULL then clientTransportAMQP_Create shall fail and return NULL.]*/
        LogError("IoTHub amqp client tranport null configuration parameter.\r\n");
    }
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
    else if (config->upperConfig->protocol == NULL)
    {
        LogError("invalid configuration (NULL protocol detected)\r\n");
    }
    else if (config->upperConfig->deviceId == NULL)
    {
        LogError("invalid configuration (NULL deviceId detected)\r\n");
    }
    else if (config->upperConfig->deviceKey == NULL)
    {
        LogError("invalid configuration (NULL deviceKey detected)\r\n");
    }
    else if (config->upperConfig->iotHubName == NULL)
    {
        LogError("invalid configuration (NULL iotHubName detected)\r\n");
    }
    else if (config->upperConfig->iotHubSuffix == NULL)
    {
        LogError("invalid configuration (NULL iotHubSuffix detected)\r\n");
    }
    else if (!config->waitingToSend)
    {
        LogError("invalid configuration (NULL waitingToSend list detected)\r\n");
    }
    else if (strlen(config->upperConfig->deviceId) > 128U)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_016: [clientTransportAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.]*/
        LogError("deviceName is too long\r\n");
    }
    else if ((strlen(config->upperConfig->deviceId) == 0) ||
             (strlen(config->upperConfig->deviceKey) == 0) ||
             (strlen(config->upperConfig->iotHubName) == 0) ||
             (strlen(config->upperConfig->iotHubSuffix) == 0))
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_017: [clientTransportAMQP_Create shall fail and return NULL if any string config field is zero length.]*/
        LogError("zero length config parameter\r\n");
    }
    else
    {
        amqpState = malloc(sizeof(AMQP_TRANSPORT_STATE));
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
        if (amqpState == NULL)
        {
            LogError("Could not allocate AMQP transport state\r\n");
        }
        else
        {
            amqpState->messageAddress = NULL;
            amqpState->eventAddress = NULL;
            amqpState->urledDeviceId = NULL;
            amqpState->devicesPortionPath = NULL;
            amqpState->cbsAddress = NULL;
            amqpState->deviceKey = NULL;
            amqpState->savedClientHandle = NULL;
            amqpState->messenger = NULL;
            amqpState->messageSubscription = NULL;
            amqpState->cbsSubscription = NULL;
            amqpState->zeroLengthString = NULL;
            amqpState->messengerInitialized = false;
            amqpState->waitingForPutTokenReply = false;
            amqpState->cbsRequestAcceptTime = CBS_DEFAULT_REQUEST_ACCEPT_TIME;
            amqpState->cbsReplyTime = CBS_DEFAULT_REPLY_TIME;
            amqpState->sasTokenLifetime = SAS_TOKEN_DEFAULT_LIFETIME;
            amqpState->sasRefreshLine = SAS_TOKEN_DEFAULT_REFRESH_LINE;
            amqpState->eventTimeout = EVENT_TIMEOUT_DEFAULT;
            amqpState->waitingToSend = config->waitingToSend;
            DList_InitializeListHead(&amqpState->workInProgress);
            DList_InitializeListHead(&amqpState->availableWorkItems);
            DList_InitializeListHead(&amqpState->messengerCorral);
            amqpState->DoWork_PullMessages = false;
            amqpState->trustedCertificates = NULL;
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
            if ((amqpState->message = pn_message()) == NULL)
            {
                LogError("Unable to preallocate the proton message!\r\n");
                clientTransportAMQP_Destroy(amqpState);
                amqpState = NULL;
            }
            else
            {
                size_t workItemsToPreallocate;
                /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
                for (workItemsToPreallocate = PROTON_EVENT_OUTGOING_WINDOW_SIZE; workItemsToPreallocate > 0; workItemsToPreallocate--)
                {
                    PAMQP_WORK_ITEM currentItem = malloc(sizeof(AMQP_WORK_ITEM));
                    if (!currentItem)
                    {
                        //
                        // Not a particularly good sign.  Fail the create.
                        //
                        LogError("Unable to preallocate work item!\r\n");
                        clientTransportAMQP_Destroy(amqpState);
                        amqpState = NULL;
                        break;
                    }
                    else
                    {
                        DList_InsertTailList(&amqpState->availableWorkItems, &currentItem->link);
                    }
                }
                if (amqpState != NULL)
                {
                    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_095: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as host, from the following pieces: "config->iotHubName + "." + config->iotHubSuffix.]*/
                    STRING_HANDLE host = createHost(config);
                    if (host != NULL)
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_091: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as urledDeviceId, by url encoding the config->deviceId.]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_093: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as devicePortionOfPath, from the following pieces: host + "/devices/" + urledDeviceId.]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_097: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as eventAddress, from the following pieces: "amqps://" + devicesPortionOfPath + "/messages/events".]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_099: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as messageAddress, from the following pieces: "amqps://" + devicesPortionOfPath + "/messages/devicebound".]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_101: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as cbsAddress, from the following pieces: "amqps://" + host + "/$cbs".]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_103: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as deviceKey, from the config->deviceKey.]*/
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_186: [An empty string shall be created.  If this fails then clientTransportAMQP shall fail and return NULL.]*/
                        if ((!createUrledDeviceId(amqpState, config)) ||
                            (!createDevicesPortionPath(amqpState, STRING_c_str(host))) ||
                            (!createEventAddress(amqpState)) ||
                            (!createMessageAddress(amqpState)) ||
                            (!createCbsAddress(amqpState, STRING_c_str(host))) ||
                            ((amqpState->deviceKey = STRING_construct(config->upperConfig->deviceKey)) == NULL) ||
                            ((amqpState->zeroLengthString = STRING_new()) == NULL))
                        {
                            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_092: [If creating the urledDeviceId fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_094: [If creating the devicePortionOfPath fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_098: [If creating the eventAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_100: [If creating the messageAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_102: [If creating the cbsAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_104: [If creating the deviceKey fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                            clientTransportAMQP_Destroy(amqpState);
                            amqpState = NULL;
                        }
                        STRING_delete(host);
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_096: [If creating the schemeAndHost fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
                        clientTransportAMQP_Destroy(amqpState);
                        amqpState = NULL;
                    }
                }
            }
        }
    }
    return amqpState;
}

static int clientTransportAMQP_Subscribe(TRANSPORT_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_013: [If the parameter handle is NULL then clientTransportAMQP_Subscribe shall fail and return a non-zero value.]*/
        LogError("Invalid handle to IoTHubClient amqp subscribe.\r\n");
        result = 1;
    }
    else
    {
        /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_014: [clientTransportAMQP_Subscribe shall succeed and return a value of zero.]*/
        PAMQP_TRANSPORT_STATE transportState = (PAMQP_TRANSPORT_STATE)handle;
        transportState->DoWork_PullMessages = true;
        result = 0;
    }
    return result;
}

static void clientTransportAMQP_Unsubscribe(TRANSPORT_HANDLE handle)
{
    PAMQP_TRANSPORT_STATE transportState = (PAMQP_TRANSPORT_STATE)handle;
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_015: [clientTransportAMQP_Unsubscribe shall do nothing if handle is NULL.]*/
    if (transportState)
    {
        transportState->DoWork_PullMessages = false;
    }
}

static IOTHUB_CLIENT_RESULT clientTransportAMQP_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    IOTHUB_CLIENT_RESULT result;

    /* Codes_SRS_IOTHUBTRANSPORTTAMQP_09_001: [clientTransportAMQP_GetSendStatus  IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
    if (handle == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid handle to IoTHubClient AMQP transport instance.\r\n");
    }
    else if (iotHubClientStatus == NULL)
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("Invalid pointer to output parameter IOTHUB_CLIENT_STATUS.\r\n");
    }
    else
    {
        PAMQP_TRANSPORT_STATE handleData = (PAMQP_TRANSPORT_STATE)handle;

        /* Codes_SRS_IOTHUBTRANSPORTTAMQP_09_002: [clientTransportAMQP_GetSendStatus  shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent] */
        if (!DList_IsListEmpty(handleData->waitingToSend) || !DList_IsListEmpty(&(handleData->workInProgress)))
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        }
        /* Codes_SRS_IOTHUBTRANSPORTTAMQP_09_003: [clientTransportAMQP_GetSendStatus  shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent] */
        else
        {
            *iotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
        }

        result = IOTHUB_CLIENT_OK;
    }

    return result;
}

static IOTHUB_CLIENT_RESULT clientTransportAMQP_SetOption(TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    IOTHUB_CLIENT_RESULT result;
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_001: [If handle parameter is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_002: [If parameter optionName is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_003: [If parameter value is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */

    if (
        (handle == NULL) ||
        (option == NULL) ||
        (value == NULL)
        )
    {
        result = IOTHUB_CLIENT_INVALID_ARG;
        LogError("invalid parameter (NULL) passed to clientTransportAMQP_SetOption\r\n");
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORTTAMQP_02_005: [clientTransportAMQP_SetOption shall set the option "optionName" to *value.] */
        /* Codes_SRS_IOTHUBTRANSPORTTAMQP_01_003: ["TrustedCerts"] */
        if (strcmp("TrustedCerts", option) == 0)
        {
            PAMQP_TRANSPORT_STATE handleData = (PAMQP_TRANSPORT_STATE)handle;
            char* newTrsutedCertificates;

            if (mallocAndStrcpy_s(&newTrsutedCertificates, (const char*)value) != 0)
            {
                /* Codes_SRS_IOTHUBTRANSPORTTAMQP_01_006: [If any other error occurs while setting the option, clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_ERROR.] */
                result = IOTHUB_CLIENT_ERROR;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORTTAMQP_01_004: [Sets a string that should be used as trusted certificates by the transport, freeing any previous TrustedCerts option value.] */
                if (handleData->trustedCertificates != NULL)
                {
                    free(handleData->trustedCertificates);
                }

                handleData->trustedCertificates = newTrsutedCertificates;
                result = IOTHUB_CLIENT_OK;
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBTRANSPORTTAMQP_02_004: [If optionName is not an option supported then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]*/
            result = IOTHUB_CLIENT_INVALID_ARG;
        }
    }

    return result;
}

static TRANSPORT_PROVIDER myfunc = {
    clientTransportAMQP_SetOption, 
    clientTransportAMQP_Create, clientTransportAMQP_Destroy, clientTransportAMQP_Subscribe, clientTransportAMQP_Unsubscribe, clientTransportAMQP_DoWork, clientTransportAMQP_GetSendStatus
};

extern const void* AMQP_Protocol(void)
{
    return &myfunc;
}
