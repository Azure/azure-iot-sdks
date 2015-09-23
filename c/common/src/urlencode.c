// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#include <stddef.h>
#include <string.h>
//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "urlencode.h"
#include "iot_logging.h"
#include "strings.h"

static const struct {
    size_t numberOfChars;
    const char* encoding;
} urlEncoding[] = {
    { 1, "\0"  },
    { 3, "%01" },
    { 3, "%02" },
    { 3, "%03" },
    { 3, "%04" },
    { 3, "%05" },
    { 3, "%06" },
    { 3, "%07" },
    { 3, "%08" },
    { 3, "%09" },
    { 3, "%0a" },
    { 3, "%0b" },
    { 3, "%0c" },
    { 3, "%0d" },
    { 3, "%0e" },
    { 3, "%0f" },
    { 3, "%10" },
    { 3, "%11" },
    { 3, "%12" },
    { 3, "%13" },
    { 3, "%14" },
    { 3, "%15" },
    { 3, "%16" },
    { 3, "%17" },
    { 3, "%18" },
    { 3, "%19" },
    { 3, "%1a" },
    { 3, "%1b" },
    { 3, "%1c" },
    { 3, "%1d" },
    { 3, "%1e" },
    { 3, "%1f" },
    { 3, "%20" },
    { 1, "!" },
    { 3, "%22" },
    { 3, "%23" },
    { 3, "%24" },
    { 3, "%25" },
    { 3, "%26" },
    { 3, "%27" },
    { 1, "(" },
    { 1, ")" },
    { 1, "*" },
    { 3, "%2b" },
    { 3, "%2c" },
    { 1, "-" },
    { 1, "." },
    { 3, "%2f" },
    { 1, "0" },
    { 1, "1" },
    { 1, "2" },
    { 1, "3" },
    { 1, "4" },
    { 1, "5" },
    { 1, "6" },
    { 1, "7" },
    { 1, "8" },
    { 1, "9" },
    { 3, "%3a" },
    { 3, "%3b" },
    { 3, "%3c" },
    { 3, "%3d" },
    { 3, "%3e" },
    { 3, "%3f" },
    { 3, "%40" },
    { 1, "A" },
    { 1, "B" },
    { 1, "C" },
    { 1, "D" },
    { 1, "E" },
    { 1, "F" },
    { 1, "G" },
    { 1, "H" },
    { 1, "I" },
    { 1, "J" },
    { 1, "K" },
    { 1, "L" },
    { 1, "M" },
    { 1, "N" },
    { 1, "O" },
    { 1, "P" },
    { 1, "Q" },
    { 1, "R" },
    { 1, "S" },
    { 1, "T" },
    { 1, "U" },
    { 1, "V" },
    { 1, "W" },
    { 1, "X" },
    { 1, "Y" },
    { 1, "Z" },
    { 3, "%5b" },
    { 3, "%5c" },
    { 3, "%5d" },
    { 3, "%5e" },
    { 1, "_" },
    { 3, "%60" },
    { 1, "a" },
    { 1, "b" },
    { 1, "c" },
    { 1, "d" },
    { 1, "e" },
    { 1, "f" },
    { 1, "g" },
    { 1, "h" },
    { 1, "i" },
    { 1, "j" },
    { 1, "k" },
    { 1, "l" },
    { 1, "m" },
    { 1, "n" },
    { 1, "o" },
    { 1, "p" },
    { 1, "q" },
    { 1, "r" },
    { 1, "s" },
    { 1, "t" },
    { 1, "u" },
    { 1, "v" },
    { 1, "w" },
    { 1, "x" },
    { 1, "y" },
    { 1, "z" },
    { 3, "%7b" },
    { 3, "%7c" },
    { 3, "%7d" },
    { 3, "%7e" },
    { 3, "%7f" },
    { 6, "%c2%80" },
    { 6, "%c2%81" },
    { 6, "%c2%82" },
    { 6, "%c2%83" },
    { 6, "%c2%84" },
    { 6, "%c2%85" },
    { 6, "%c2%86" },
    { 6, "%c2%87" },
    { 6, "%c2%88" },
    { 6, "%c2%89" },
    { 6, "%c2%8a" },
    { 6, "%c2%8b" },
    { 6, "%c2%8c" },
    { 6, "%c2%8d" },
    { 6, "%c2%8e" },
    { 6, "%c2%8f" },
    { 6, "%c2%90" },
    { 6, "%c2%91" },
    { 6, "%c2%92" },
    { 6, "%c2%93" },
    { 6, "%c2%94" },
    { 6, "%c2%95" },
    { 6, "%c2%96" },
    { 6, "%c2%97" },
    { 6, "%c2%98" },
    { 6, "%c2%99" },
    { 6, "%c2%9a" },
    { 6, "%c2%9b" },
    { 6, "%c2%9c" },
    { 6, "%c2%9d" },
    { 6, "%c2%9e" },
    { 6, "%c2%9f" },
    { 6, "%c2%a0" },
    { 6, "%c2%a1" },
    { 6, "%c2%a2" },
    { 6, "%c2%a3" },
    { 6, "%c2%a4" },
    { 6, "%c2%a5" },
    { 6, "%c2%a6" },
    { 6, "%c2%a7" },
    { 6, "%c2%a8" },
    { 6, "%c2%a9" },
    { 6, "%c2%aa" },
    { 6, "%c2%ab" },
    { 6, "%c2%ac" },
    { 6, "%c2%ad" },
    { 6, "%c2%ae" },
    { 6, "%c2%af" },
    { 6, "%c2%b0" },
    { 6, "%c2%b1" },
    { 6, "%c2%b2" },
    { 6, "%c2%b3" },
    { 6, "%c2%b4" },
    { 6, "%c2%b5" },
    { 6, "%c2%b6" },
    { 6, "%c2%b7" },
    { 6, "%c2%b8" },
    { 6, "%c2%b9" },
    { 6, "%c2%ba" },
    { 6, "%c2%bb" },
    { 6, "%c2%bc" },
    { 6, "%c2%bd" },
    { 6, "%c2%be" },
    { 6, "%c2%bf" },
    { 6, "%c3%80" },
    { 6, "%c3%81" },
    { 6, "%c3%82" },
    { 6, "%c3%83" },
    { 6, "%c3%84" },
    { 6, "%c3%85" },
    { 6, "%c3%86" },
    { 6, "%c3%87" },
    { 6, "%c3%88" },
    { 6, "%c3%89" },
    { 6, "%c3%8a" },
    { 6, "%c3%8b" },
    { 6, "%c3%8c" },
    { 6, "%c3%8d" },
    { 6, "%c3%8e" },
    { 6, "%c3%8f" },
    { 6, "%c3%90" },
    { 6, "%c3%91" },
    { 6, "%c3%92" },
    { 6, "%c3%93" },
    { 6, "%c3%94" },
    { 6, "%c3%95" },
    { 6, "%c3%96" },
    { 6, "%c3%97" },
    { 6, "%c3%98" },
    { 6, "%c3%99" },
    { 6, "%c3%9a" },
    { 6, "%c3%9b" },
    { 6, "%c3%9c" },
    { 6, "%c3%9d" },
    { 6, "%c3%9e" },
    { 6, "%c3%9f" },
    { 6, "%c3%a0" },
    { 6, "%c3%a1" },
    { 6, "%c3%a2" },
    { 6, "%c3%a3" },
    { 6, "%c3%a4" },
    { 6, "%c3%a5" },
    { 6, "%c3%a6" },
    { 6, "%c3%a7" },
    { 6, "%c3%a8" },
    { 6, "%c3%a9" },
    { 6, "%c3%aa" },
    { 6, "%c3%ab" },
    { 6, "%c3%ac" },
    { 6, "%c3%ad" },
    { 6, "%c3%ae" },
    { 6, "%c3%af" },
    { 6, "%c3%b0" },
    { 6, "%c3%b1" },
    { 6, "%c3%b2" },
    { 6, "%c3%b3" },
    { 6, "%c3%b4" },
    { 6, "%c3%b5" },
    { 6, "%c3%b6" },
    { 6, "%c3%b7" },
    { 6, "%c3%b8" },
    { 6, "%c3%b9" },
    { 6, "%c3%ba" },
    { 6, "%c3%bb" },
    { 6, "%c3%bc" },
    { 6, "%c3%bd" },
    { 6, "%c3%be" },
    { 6, "%c3%bf" }
};

STRING_HANDLE URL_EncodeString(const char* textEncode)
{
    STRING_HANDLE result;
    if (textEncode == NULL)
    {
        result = NULL;
    }
    else
    {
        STRING_HANDLE tempString = STRING_construct(textEncode);
        if (tempString == NULL)
        {
            result = NULL;
        }
        else
        {
            result = URL_Encode(tempString);
            STRING_delete(tempString);
        }
    }
    return result;
}

STRING_HANDLE URL_Encode(STRING_HANDLE input)
{
    STRING_HANDLE result;
    if (input == NULL)
    {
        /*Codes_SRS_URL_ENCODE_06_001: [If input is NULL then URL_Encode will return NULL.]*/
        result = NULL;
        LogError("URL_Encode:: NULL input\r\n");
    }
    else
    {
        size_t lengthOfResult = 0;
        char* encodedURL;
        const char* currentInput;
        unsigned char currentUnsignedChar;
        currentInput = STRING_c_str(input);
        /*Codes_SRS_URL_ENCODE_06_003: [If input is a zero length string then URL_Encode will return a zero length string.]*/
        do
        {
            currentUnsignedChar = (unsigned char)(*currentInput++);
            lengthOfResult += urlEncoding[currentUnsignedChar].numberOfChars;
        } while (currentUnsignedChar != 0);
        if ((encodedURL = malloc(lengthOfResult)) == NULL)
        {
            /*Codes_SRS_URL_ENCODE_06_002: [If an error occurs during the encoding of input then URL_Encode will return NULL.]*/
            result = NULL;
            LogError("URL_Encode:: MALLOC failure on encode.\r\n");
        }
        else
        {
            size_t currentEncodePosition = 0;
            currentInput = STRING_c_str(input);
            do
            {
                currentUnsignedChar = (unsigned char)(*currentInput++);
                if (urlEncoding[currentUnsignedChar].numberOfChars == 1)
                {
                    encodedURL[currentEncodePosition++] = *(urlEncoding[currentUnsignedChar].encoding);
                }
                else
                {
                    memcpy(encodedURL + currentEncodePosition, urlEncoding[currentUnsignedChar].encoding, urlEncoding[currentUnsignedChar].numberOfChars);
                    currentEncodePosition += urlEncoding[currentUnsignedChar].numberOfChars;
                }
            } while (currentUnsignedChar != 0);
            result = STRING_new_with_memory(encodedURL);
        }
    }
    return result;
}
