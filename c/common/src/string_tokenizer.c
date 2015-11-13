// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE
//
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE
//

#include "string_tokenizer.h"
#include "iot_logging.h"
#include "crt_abstractions.h"
#include <stdbool.h>

typedef struct STRING_TOKEN_TAG
{
	const char* inputString;
	const char* currentPos;
	size_t sizeOfinputString;
} STRING_TOKEN;

STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create(STRING_HANDLE handle)
{
	STRING_TOKEN *result;
	char* inputStringToMalloc;

	/* Codes_SRS_STRING_04_001: [STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER_HANDLE if parameter handle is NULL] */
	if (handle == NULL)
	{
		LogError("Invalid Argument. Handle cannot be NULL.\r\n");
		result = NULL;
	}
	/* Codes_SRS_STRING_04_002: [STRING_TOKENIZER_create shall allocate a new STRING_TOKENIZER_HANDLE having the content of the STRING_HANDLE copied and current position pointing at the beginning of the string] */
	else if((result = (STRING_TOKEN *)malloc(sizeof(STRING_TOKEN))) == NULL)
	{
		LogError("Memory Allocation failed. Cannot allocate STRING_TOKENIZER.\r\n");
	}
	else if ((mallocAndStrcpy_s(&inputStringToMalloc, STRING_c_str(handle))) != 0)
	{
		LogError("Memory Allocation Failed. Cannot allocate and copy string Content.\r\n");
		free(result);
		result = NULL;
	}
	else
	{
		result->inputString = inputStringToMalloc;
		result->currentPos = result->inputString; //Current Pos will point to the initial position of Token.
		result->sizeOfinputString = strlen(result->inputString); //Calculate Size of Current String
	}
    
	return (STRING_TOKENIZER_HANDLE)result;
}


int STRING_TOKENIZER_get_next_token(STRING_TOKENIZER_HANDLE tokenizer, STRING_HANDLE output, const char* delimiters)
{
	int result;
	/* Codes_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
	if (tokenizer == NULL || output == NULL || delimiters == NULL)
	{
		result = __LINE__;
	}
	else 
	{
		STRING_TOKEN* token = (STRING_TOKEN*)tokenizer;
		/* Codes_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
		size_t remainingInputStringSize = token->sizeOfinputString - (token->currentPos - token->inputString);
		size_t delimitterSize = strlen(delimiters);

		/* First Check if we reached the end of the string*/
		/* Codes_SRS_STRING_TOKENIZER_04_014: [STRING_TOKENIZER_get_next_token shall return nonzero value if t contains an empty string.] */
		if (remainingInputStringSize == 0)
		{
			result = __LINE__;
		}
		else if (delimitterSize == 0)
		{
			LogError("Empty delimiters parameter.\r\n");
			result = __LINE__;
		}
		else
		{
			size_t i;
			/* Codes_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
			for (i = 0; i < remainingInputStringSize; i++)
			{
				size_t j;
				bool foundDelimitter = false;
				for (j = 0; j < delimitterSize; j++)
				{
					if (token->currentPos[i] == delimiters[j])
					{
						foundDelimitter = true;
						break;
					}
				}

				/* Codes_SRS_STRING_04_007: [If such a character is found, STRING_TOKENIZER_get_next_token consider it as the start of a token.] */
				if (!foundDelimitter)
				{
					break;
				}
			}

			/* Codes_SRS_STRING_04_006: [If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You've reach the end of the string or the string consists with only delimiters).] */
			//At this point update Current Pos to the character of the last token found or end of String.
			token->currentPos += i; 
			
			//Update the remainingInputStringSize
			remainingInputStringSize -= i;
			
			/* Codes_SRS_STRING_04_006: [If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You've reach the end of the string or the string consists with only delimiters).] */
			if (remainingInputStringSize == 0)
			{
				result = __LINE__;
			}
			else
			{
				bool foundDelimitter = false;
				char* endOfTokenPosition=NULL;
				size_t amountOfCharactersToCopy;
				size_t j;
				//At this point the Current Pos is pointing to a character that is point to a nonDelimiter. So, now search for a Delimiter, till the end of the String. 
				/*Codes_SRS_STRING_04_008: [STRING_TOKENIZER_get_next_token than searches from the start of a token for a character that is contained in the delimiters string.] */
				/* Codes_SRS_STRING_04_009: [If no such character is found, STRING_TOKENIZER_get_next_token extends the current token to the end of the string inside t, copies the token to output and returns 0.] */
				/* Codes_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
				for (j = 0; j < delimitterSize; j++)
				{
					if ((endOfTokenPosition = strchr(token->currentPos, delimiters[j])) != NULL)
					{
						foundDelimitter = true;
						break;
					}
				}

				//If token not found, than update the EndOfToken to the end of the inputString;
				if (endOfTokenPosition == NULL)
				{
					amountOfCharactersToCopy = remainingInputStringSize;
				}
				else
				{
					amountOfCharactersToCopy = endOfTokenPosition - token->currentPos;
				}
				
				//copy here the string to output. 
				if (STRING_copy_n(output, token->currentPos, amountOfCharactersToCopy) != 0)
				{
					LogError("Problem copying token to output String.\r\n");
					result = __LINE__;
				}
				else
				{
					//Update the Current position.
					//Check if end of String reached so, currentPos points to the end of String.
					if (foundDelimitter)
					{
						token->currentPos += amountOfCharactersToCopy + 1;
					}
					else
					{
						token->currentPos += amountOfCharactersToCopy;
					}
					
					result = 0; //Result will be on the output. 
				}
			}
		}
	}

	return result;
}


/* Codes_SRS_STRING_TOKENIZER_04_012: [STRING_TOKENIZER_destroy shall free the memory allocated by the STRING_TOKENIZER_create ] */
void STRING_TOKENIZER_destroy(STRING_TOKENIZER_HANDLE t)
{
	/* Codes_SRS_STRING_TOKENIZER_04_013: [When the t argument is NULL, then STRING_TOKENIZER_destroy shall not attempt to free] */
	if (t != NULL)
	{
		STRING_TOKEN* value = (STRING_TOKEN*)t;
		free((char*)value->inputString);
		value->inputString = NULL;
		free(value);
	}
}

