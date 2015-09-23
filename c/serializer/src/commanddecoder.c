// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <stddef.h>

#include "commanddecoder.h"
#include "multitree.h"
#include "crt_abstractions.h"
#include "iot_logging.h"
#include "schema.h"
#include "codefirst.h"
#include "jsondecoder.h"

DEFINE_ENUM_STRINGS(COMMANDDECODER_RESULT, COMMANDDECODER_RESULT_VALUES);

typedef struct COMMAND_DECODER_INSTANCE_TAG
{
    SCHEMA_MODEL_TYPE_HANDLE ModelHandle;
    ACTION_CALLBACK_FUNC ActionCallback;
    void* ActionCallbackContext;
} COMMAND_DECODER_INSTANCE;

static int DecodeValueFromNode(SCHEMA_HANDLE schemaHandle, AGENT_DATA_TYPE* agentDataType, MULTITREE_HANDLE node, const char* edmTypeName)
{
    /* because "pottentially uninitialized variable on MS compiler" */
    int result = 0;
    const char* argStringValue;
    AGENT_DATA_TYPE_TYPE primitiveType;

    /* Codes_SRS_COMMAND_DECODER_99_029:[ If the argument type is complex then a complex type value shall be built from the child nodes.] */
    if ((primitiveType = CodeFirst_GetPrimitiveType(edmTypeName)) == EDM_NO_TYPE)
    {
        SCHEMA_STRUCT_TYPE_HANDLE structTypeHandle;
        size_t propertyCount;

        /* Codes_SRS_COMMAND_DECODER_99_033:[ In order to determine which are the members of a complex types, Schema APIs for structure types shall be used.] */
        if (((structTypeHandle = Schema_GetStructTypeByName(schemaHandle, edmTypeName)) == NULL) ||
            (Schema_GetStructTypePropertyCount(structTypeHandle, &propertyCount) != SCHEMA_OK))
        {
            /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
            result = __LINE__;
            LogError("Getting Struct information failed.\r\n");
        }
        else
        {
            if (propertyCount == 0)
            {
                /* Codes_SRS_COMMAND_DECODER_99_034:[ If Schema APIs indicate that a complex type has 0 members then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                result = __LINE__;
                LogError("Struct type with 0 members is not allowed\r\n");
            }
            else
            {
                AGENT_DATA_TYPE* memberValues = (AGENT_DATA_TYPE*)malloc(sizeof(AGENT_DATA_TYPE)* propertyCount);
                if (memberValues == NULL)
                {
                    /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                    result = __LINE__;
                    LogError("Failed allocating member values for command argument\r\n");
                }
                else
                {
                    const char** memberNames = (const char**)malloc(sizeof(const char*)* propertyCount);
                    if (memberNames == NULL)
                    {
                        /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                        result = __LINE__;
                        LogError("Failed allocating member names for command argument.\r\n");
                    }
                    else
                    {
                        size_t j;
                        size_t k;

                        for (j = 0; j < propertyCount; j++)
                        {
                            SCHEMA_PROPERTY_HANDLE propertyHandle;
                            MULTITREE_HANDLE memberNode;
                            const char* propertyName;
                            const char* propertyType;

                            if ((propertyHandle = Schema_GetStructTypePropertyByIndex(structTypeHandle, j)) == NULL)
                            {
                                /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
                                result = __LINE__;
                                LogError("Getting struct member failed.\r\n");
                                break;
                            }
                            else if (((propertyName = Schema_GetPropertyName(propertyHandle)) == NULL) ||
                                     ((propertyType = Schema_GetPropertyType(propertyHandle)) == NULL))
                            {
                                /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
                                result = __LINE__;
                                LogError("Getting the struct member information failed.\r\n");
                                break;
                            }
                            else
                            {
                                memberNames[j] = propertyName;
                                
                                /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
                                if (MultiTree_GetChildByName(node, memberNames[j], &memberNode) != MULTITREE_OK)
                                {
                                    /* Codes_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                                    result = __LINE__;
                                    LogError("Getting child %s failed\r\n", propertyName);
                                    break;
                                }
                                /* Codes_SRS_COMMAND_DECODER_99_032:[ Nesting shall be supported for complex type.] */
                                else if ((result = DecodeValueFromNode(schemaHandle, &memberValues[j], memberNode, propertyType)) != 0)
                                {
                                    break;
                                }
                            }
                        }

                        if (j == propertyCount)
                        {
                            /* Codes_SRS_COMMAND_DECODER_99_031:[ The complex type value that aggregates the children shall be built by using the Create_AGENT_DATA_TYPE_from_Members.] */
                            if (Create_AGENT_DATA_TYPE_from_Members(agentDataType, edmTypeName, propertyCount, (const char* const*)memberNames, memberValues) != AGENT_DATA_TYPES_OK)
                            {
                                /* Codes_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                                result = __LINE__;
                                LogError("Creating the agent data type from members failed.\r\n");
                            }
                            else
                            {
                                result = 0;
                            }
                        }

                        for (k = 0; k < j; k++)
                        {
                            Destroy_AGENT_DATA_TYPE(&memberValues[k]);
                        }

                        free((void*)memberNames);
                    }

                    free(memberValues);
                }
            }
        }
    }
    else
    {
        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
        if (MultiTree_GetValue(node, (const void **)&argStringValue) != MULTITREE_OK)
        {
            /* Codes_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            result = __LINE__;
            LogError("Getting the string from the multitree failed.\r\n");
        }
        /* Codes_SRS_COMMAND_DECODER_99_027:[ The value for an argument of primitive type shall be decoded by using the CreateAgentDataType_From_String API.] */
        else if (CreateAgentDataType_From_String(argStringValue, primitiveType, agentDataType) != AGENT_DATA_TYPES_OK)
        {
            /* Codes_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            result = __LINE__;
            LogError("Failed parsing node %s.\r\n", argStringValue);
        }
    }

    return result;
}

static EXECUTE_COMMAND_RESULT DecodeAndExecuteModelAction(COMMAND_DECODER_INSTANCE* commandDecoderInstance, SCHEMA_HANDLE schemaHandle, SCHEMA_MODEL_TYPE_HANDLE modelHandle, const char* relativeActionPath, const char* actionName, MULTITREE_HANDLE commandNode)
{
    EXECUTE_COMMAND_RESULT result;
    char tempStr[128];
    size_t strLength = strlen(actionName);

    if (strLength <= 1)
    {
        /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
        LogError("Invalid action name\r\n");
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        /* Codes_SRS_COMMAND_DECODER_99_006:[ The action name shall be decoded from the element "Name" of the command JSON.] */
        SCHEMA_ACTION_HANDLE modelActionHandle;
        size_t argCount;
        MULTITREE_HANDLE parametersTreeNode;

#ifdef _MSC_VER
#pragma warning(suppress: 6324) /* We intentionally use here strncpy */ 
#endif
        if (strncpy(tempStr, actionName, strLength - 1) == NULL)
        {
            /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
            LogError("Invalid action name.\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
        else if (MultiTree_GetChildByName(commandNode, "Parameters", &parametersTreeNode) != MULTITREE_OK)
        {
            /* Codes_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            LogError("Error getting Parameters node.\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            tempStr[strLength - 1] = 0;

            /* Codes_SRS_COMMAND_DECODER_99_009:[ CommandDecoder shall call Schema_GetModelActionByName to obtain the information about a specific action.] */
            if (((modelActionHandle = Schema_GetModelActionByName(modelHandle, tempStr)) == NULL) ||
                (Schema_GetModelActionArgumentCount(modelActionHandle, &argCount) != SCHEMA_OK))
            {
                /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
                LogError("Failed reading action %s from the schema\r\n", tempStr);
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                AGENT_DATA_TYPE* arguments = NULL;

                if (argCount > 0)
                {
                    arguments = (AGENT_DATA_TYPE*)malloc(sizeof(AGENT_DATA_TYPE)* argCount);
                }

                if ((argCount > 0) &&
                    (arguments == NULL))
                {
                    /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                    LogError("Failed allocating arguments array\r\n");
                    result = EXECUTE_COMMAND_ERROR;
                }
                else
                {
                    size_t i;
                    size_t j;

                    /* Codes_SRS_COMMAND_DECODER_99_011:[ CommandDecoder shall attempt to extract from the command text the value for each action argument.] */
                    for (i = 0; i < argCount; i++)
                    {
                        SCHEMA_ACTION_ARGUMENT_HANDLE actionArgumentHandle;
                        MULTITREE_HANDLE argumentNode;
                        const char* argName;
                        const char* argType;

                        if (((actionArgumentHandle = Schema_GetModelActionArgumentByIndex(modelActionHandle, i)) == NULL) ||
                            ((argName = Schema_GetActionArgumentName(actionArgumentHandle)) == NULL) ||
                            ((argType = Schema_GetActionArgumentType(actionArgumentHandle)) == NULL))
                        {
                            LogError("Failed getting the argument information from the schema\r\n");
                            result = EXECUTE_COMMAND_ERROR;
                            break;
                        }
                        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
                        /* Codes_SRS_COMMAND_DECODER_01_008: [Each argument shall be looked up as a field, member of the "Parameters" node.]  */
                        else if (MultiTree_GetChildByName(parametersTreeNode, argName, &argumentNode) != MULTITREE_OK)
                        {
                            /* Codes_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                            LogError("Missing argument %s\r\n", argName);
                            result = EXECUTE_COMMAND_ERROR;
                            break;
                        }
                        else if (DecodeValueFromNode(schemaHandle, &arguments[i], argumentNode, argType) != 0)
                        {
                            result = EXECUTE_COMMAND_ERROR;
                            break;
                        }
                    }

                    if (i == argCount)
                    {
                        /* Codes_SRS_COMMAND_DECODER_99_005:[ If an Invoke Action is decoded successfully then the callback actionCallback shall be called, passing to it the callback action context, decoded name and arguments.] */
                        result = commandDecoderInstance->ActionCallback(commandDecoderInstance->ActionCallbackContext, relativeActionPath, tempStr, argCount, arguments);
                    }

                    for (j = 0; j < i; j++)
                    {
                        Destroy_AGENT_DATA_TYPE(&arguments[j]);
                    }

                    if (arguments != NULL)
                    {
                        free(arguments);
                    }
                }
            }
        }
    }
    return result;
}

static EXECUTE_COMMAND_RESULT ScanActionPathAndExecuteAction(COMMAND_DECODER_INSTANCE* commandDecoderInstance, SCHEMA_HANDLE schemaHandle, const char* actionPath, MULTITREE_HANDLE commandNode)
{
    EXECUTE_COMMAND_RESULT result;
    char* relativeActionPath;
    const char* actionName = actionPath;
    SCHEMA_MODEL_TYPE_HANDLE modelHandle = commandDecoderInstance->ModelHandle;

    /* Codes_SRS_COMMAND_DECODER_99_035:[ CommandDecoder_ExecuteCommand shall support paths to actions that are in child models (i.e. ChildModel/SomeAction.] */
    do
    {
        /* find the slash */
        const char* slashPos = strchr(actionName, '/');
        if (slashPos == NULL)
        {
            size_t relativeActionPathLength;

            /* Codes_SRS_COMMAND_DECODER_99_037:[ The relative path passed to the actionCallback shall be in the format "childModel1/childModel2/.../childModelN".] */
            if (actionName == actionPath)
            {
                relativeActionPathLength = 0;
            }
            else
            {
                relativeActionPathLength = actionName - actionPath - 1;
            }

            relativeActionPath = (char*)malloc(relativeActionPathLength + 1);
            if (relativeActionPath == NULL)
            {
                /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                LogError("Failed allocating relative action path\r\n");
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                strncpy(relativeActionPath, actionPath, relativeActionPathLength);
                relativeActionPath[relativeActionPathLength] = 0;

                /* no slash found, this must be an action */
                result = DecodeAndExecuteModelAction(commandDecoderInstance, schemaHandle, modelHandle, relativeActionPath, actionName, commandNode);

                free(relativeActionPath);
                actionName = NULL;
            }
            break;
        }
        else
        {
            /* found a slash, get the child model name */
            size_t modelLength = slashPos - actionName;
            char* childModelName = (char*)malloc(modelLength + 1);
            if (childModelName == NULL)
            {
                /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
                LogError("Failed allocating child model name\r\n");
                result = EXECUTE_COMMAND_ERROR;
                break;
            }
            else
            {
                strncpy(childModelName, actionName, modelLength);
                childModelName[modelLength] = 0;

                /* find the model */
                modelHandle = Schema_GetModelModelByName(modelHandle, childModelName);
                if (modelHandle == NULL)
                {
                    /* Codes_SRS_COMMAND_DECODER_99_036:[ If a child model cannot be found by using Schema APIs then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                    LogError("Getting the model %s failed\r\n", childModelName);
                    free(childModelName);
                    result = EXECUTE_COMMAND_ERROR;
                    break;
                }
                else
                {
                    free(childModelName);
                    actionName = slashPos + 1;
                    result = EXECUTE_COMMAND_ERROR; /*this only exists to quench a compiler warning about returning an uninitialized variable, which is not possible by design*/
                }
            }
        }
    } while (actionName != NULL);
    return result;
}

static EXECUTE_COMMAND_RESULT DecodeCommand(COMMAND_DECODER_INSTANCE* commandDecoderInstance, MULTITREE_HANDLE commandNode)
{
    EXECUTE_COMMAND_RESULT result;
    SCHEMA_HANDLE schemaHandle;

    /* Codes_SRS_COMMAND_DECODER_99_022:[ CommandDecoder shall use the Schema APIs to obtain the information about the entity set name and namespace] */
    if ((schemaHandle = Schema_GetSchemaForModelType(commandDecoderInstance->ModelHandle)) == NULL)
    {
        /* Codes_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
        LogError("Getting schema information failed\r\n");
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        const char* actionName;
        MULTITREE_HANDLE nameTreeNode;

        /* Codes_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
        /* Codes_SRS_COMMAND_DECODER_99_006:[ The action name shall be decoded from the element "name" of the command JSON.] */
        if ((MultiTree_GetChildByName(commandNode, "Name", &nameTreeNode) != MULTITREE_OK) ||
            (MultiTree_GetValue(nameTreeNode, (const void **)&actionName) != MULTITREE_OK))
        {
            /* Codes_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
            LogError("Getting action name failed.\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        else if (strlen(actionName) < 2)
        {
            /* Codes_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.] */
            LogError("Invalid action name.\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            actionName++;
            result = ScanActionPathAndExecuteAction(commandDecoderInstance, schemaHandle, actionName, commandNode);
        }
    }
    return result;
}

/*Codes_SRS_COMMAND_DECODER_01_009: [Whenever CommandDecoder_ExecuteCommand is the command shall be decoded and further dispatched to the actionCallback passed in CommandDecoder_Create.]*/
EXECUTE_COMMAND_RESULT CommandDecoder_ExecuteCommand(COMMAND_DECODER_HANDLE handle, const char* command)
{
    EXECUTE_COMMAND_RESULT result;
    COMMAND_DECODER_INSTANCE* commandDecoderInstance = (COMMAND_DECODER_INSTANCE*)handle;
    /*Codes_SRS_COMMAND_DECODER_01_010: [If either the buffer or the receiveCallbackContext argument is NULL, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    if (
        (command == NULL) ||
        (commandDecoderInstance == NULL) 
    )
    {
        LogError("Invalid argument, COMMAND_DECODER_HANDLE handle=%p, const char* command=%p\r\n", handle, command);
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        size_t size = strlen(command);
        char* commandJSON;

        /* Codes_SRS_COMMAND_DECODER_01_011: [If the size of the command is 0 then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
        if (
            (size == 0)
            )
        {
            LogError("Failed because command size is zero\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        /*Codes_SRS_COMMAND_DECODER_01_013: [If parsing the JSON to a multi tree fails, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
        else if ((commandJSON = (char*)malloc(size + 1)) == NULL)
        {
            LogError("Failed to allocate temporary storage for the commands JSON\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            MULTITREE_HANDLE commandsTree;

            (void)memcpy(commandJSON, command, size);
            commandJSON[size] = '\0';

            /* Codes_SRS_COMMAND_DECODER_01_012: [CommandDecoder shall decode the command JSON contained in buffer to a multi-tree by using JSONDecoder_JSON_To_MultiTree.] */
            if (JSONDecoder_JSON_To_MultiTree(commandJSON, &commandsTree) != JSON_DECODER_OK)
            {
                /* Codes_SRS_COMMAND_DECODER_01_013: [If parsing the JSON to a multi tree fails, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
                LogError("Decoding JSON to a multi tree failed\r\n");
                result = EXECUTE_COMMAND_ERROR;
            }
            else
            {
                result = DecodeCommand(commandDecoderInstance, commandsTree);

                /* Codes_SRS_COMMAND_DECODER_01_016: [CommandDecoder shall ensure that the multi-tree resulting from JSONDecoder_JSON_To_MultiTree is freed after the commands are executed.] */
                MultiTree_Destroy(commandsTree);
            }

            free(commandJSON);
        }
    }
    return result;
}

COMMAND_DECODER_HANDLE CommandDecoder_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, ACTION_CALLBACK_FUNC actionCallback, void* actionCallbackContext)
{
    COMMAND_DECODER_INSTANCE* result;
    /* Codes_SRS_COMMAND_DECODER_99_019:[ For all exposed APIs argument validity checks shall precede other checks.] */
    /* Codes_SRS_COMMAND_DECODER_01_003: [If any of the arguments modelHandle is NULL, CommandDecoder_Create shall return NULL.]*/
    if (
        (modelHandle == NULL)
        )
    {
        LogError("Invalid arguments modelHandle=%p, actionCallback=%p, actionCallbackContext=%p",
            modelHandle, actionCallback, actionCallbackContext);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_COMMAND_DECODER_01_001: [CommandDecoder_Create shall create a new instance of a CommandDecoder.] */
        result = malloc(sizeof(COMMAND_DECODER_INSTANCE));
        if (result == NULL)
        {
            /* Codes_SRS_COMMAND_DECODER_01_004: [If any error is encountered during CommandDecoder_Create CommandDecoder_Create shall return NULL.] */
            result = NULL;
        }
        else
        {
            result->ModelHandle = modelHandle;
            result->ActionCallback = actionCallback;
            result->ActionCallbackContext = actionCallbackContext;
        }
    }

    return result;
}

void CommandDecoder_Destroy(COMMAND_DECODER_HANDLE commandDecoderHandle)
{
    /* Codes_SRS_COMMAND_DECODER_01_007: [If CommandDecoder_Destroy is called with a NULL handle, CommandDecoder_Destroy shall do nothing.] */
    if (commandDecoderHandle != NULL)
    {
        COMMAND_DECODER_INSTANCE* commandDecoderInstance = (COMMAND_DECODER_INSTANCE*)commandDecoderHandle;

        /* Codes_SRS_COMMAND_DECODER_01_005: [CommandDecoder_Destroy shall free all resources associated with the commandDecoderHandle instance.] */
        free(commandDecoderInstance);
    }
}