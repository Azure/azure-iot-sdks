// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "connection_string_parser.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/xlogging.h"

/* Codes_SRS_CONNECTIONSTRINGPARSER_01_001: [connectionstringparser_parse shall parse all key value pairs from the connection_string passed in as argument and return a new map that holds the key/value pairs.] */
MAP_HANDLE connectionstringparser_parse(STRING_HANDLE connection_string)
{
    MAP_HANDLE result;

    if (connection_string == NULL)
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_002: [If connection_string is NULL then connectionstringparser_parse shall fail and return NULL.] */
        result = NULL;
        LogError("NULL connection string passed to tokenizer");
    }
    else
    {
        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_003: [connectionstringparser_parse shall create a STRING tokenizer to be used for parsing the connection string, by calling STRING_TOKENIZER_create.] */
        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_004: [connectionstringparser_parse shall start scanning at the beginning of the connection string.] */
        STRING_TOKENIZER_HANDLE tokenizer = STRING_TOKENIZER_create(connection_string);
        if (tokenizer == NULL)
        {
            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_015: [If STRING_TOKENIZER_create fails, connectionstringparser_parse shall fail and return NULL.] */
            result = NULL;
            LogError("Error creating STRING tokenizer");
        }
        else
        {
            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_016: [2 STRINGs shall be allocated in order to hold the to be parsed key and value tokens.] */
            STRING_HANDLE token_key_string = STRING_new();
            if (token_key_string == NULL)
            {
                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_017: [If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.] */
                result = NULL;
                LogError("Error creating key token STRING");
            }
            else
            {
                STRING_HANDLE token_value_string = STRING_new();
                if (token_value_string == NULL)
                {
                    /* Codes_SRS_CONNECTIONSTRINGPARSER_01_017: [If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.] */
                    result = NULL;
                    LogError("Error creating value token STRING");
                }
                else
                {
                    result = Map_Create(NULL);
                    if (result == NULL)
                    {
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_018: [If creating the result map fails, then connectionstringparser_parse shall return NULL.] */
                        LogError("Error creating Map\r\n");
                    }
                    else
                    {
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_005: [The following actions shall be repeated until parsing is complete:] */
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_006: [connectionstringparser_parse shall find a token (the key of the key/value pair) delimited by the "=" character, by calling STRING_TOKENIZER_get_next_token.] */
                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_007: [If STRING_TOKENIZER_get_next_token fails, parsing shall be considered complete.] */
                        while (STRING_TOKENIZER_get_next_token(tokenizer, token_key_string, "=") == 0)
                        {
                            bool is_error = false;

                            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_008: [connectionstringparser_parse shall find a token (the value of the key/value pair) delimited by the ";" character, by calling STRING_TOKENIZER_get_next_token.] */
                            if (STRING_TOKENIZER_get_next_token(tokenizer, token_value_string, ";") != 0)
                            {
                                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_009: [If STRING_TOKENIZER_get_next_token fails, connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                is_error = true;
                                LogError("Error reading value token from the connection string");
                            }
                            else
                            {
                                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_011: [The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.] */
                                const char* token = STRING_c_str(token_key_string);
                                /* Codes_SRS_CONNECTIONSTRINGPARSER_01_013: [If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                if ((token == NULL) ||
                                    /* Codes_SRS_CONNECTIONSTRINGPARSER_01_019: [If the key length is zero then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                    (strlen(token) == 0))
                                {
                                    is_error = true;
                                    LogError("The key token is NULL or empty");
                                }
                                else
                                {
                                    /* Codes_SRS_CONNECTIONSTRINGPARSER_01_011: [The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.] */
                                    const char* value = STRING_c_str(token_value_string);
                                    if (value == NULL)
                                    {
                                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_013: [If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                        is_error = true;
                                        LogError("Could not get C string for value token");
                                    }
                                    else
                                    {
                                        /* Codes_SRS_CONNECTIONSTRINGPARSER_01_010: [The key and value shall be added to the result map by using Map_Add.] */
                                        if (Map_Add(result, token, value) != 0)
                                        {
                                            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_012: [If Map_Add fails connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
                                            is_error = true;
                                            LogError("Could not add the key/value pair to the result map");
                                        }
                                    }
                                }
                            }

                            if (is_error)
                            {
                                LogError("Error parsing connection string");
                                Map_Destroy(result);
                                result = NULL;
                                break;
                            }
                        }
                    }

                    STRING_delete(token_value_string);
                }

                STRING_delete(token_key_string);
            }

            /* Codes_SRS_CONNECTIONSTRINGPARSER_01_014: [After the parsing is complete the previously allocated STRINGs and STRING tokenizer shall be freed by calling STRING_TOKENIZER_destroy.] */
            STRING_TOKENIZER_destroy(tokenizer);
        }
    }

    return result;
}