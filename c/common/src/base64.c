// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <stdlib.h>
#ifndef WINCE
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif
#include "gballoc.h"

#include <stddef.h>
#include <string.h>
//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "base64.h"
#include "iot_logging.h"

static const char base64char[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

static const char base64b16[16] = {
    'A', 'E', 'I', 'M', 'Q', 'U', 'Y', 'c', 'g', 'k',
    'o', 's', 'w', '0', '4', '8'
};

static const char base64b8[4] = {
    'A', 'Q', 'g', 'w'
};

static int base64toValue(char base64character, unsigned char* value)
{
    int result = 0;
    if (('A' <= base64character) && (base64character <= 'Z'))
    {
        *value = base64character - 'A';
    }
    else if (('a' <= base64character) && (base64character <= 'z'))
    {
        *value = ('Z' - 'A') + 1 + (base64character - 'a');
    }
    else if (('0' <= base64character) && (base64character <= '9'))
    {
        *value = ('Z' - 'A') + 1 + ('z' - 'a') + 1 + (base64character - '0');
    }
    else if ('+' == base64character)
    {
        *value = 62;
    }
    else if ('/' == base64character)
    {
        *value = 63;
    }
    else
    {
        *value = 0;
        result = -1;
    }
    return result;
}

static size_t numberOfBase64Characters(const char* encodedString)
{
    size_t length = 0;
    unsigned char junkChar;
    while (base64toValue(encodedString[length],&junkChar) != -1)
    {
        length++;
    }
    return length;
}
static size_t Base64decode_len(const char *encodedString)
{
    size_t absoluteLengthOfString;
    size_t numberOfBytesToAdd = 0;

    absoluteLengthOfString = strlen(encodedString);
    if (absoluteLengthOfString)
    {
        if (encodedString[absoluteLengthOfString - 1] == '=')
        {
            // See if there are two.
            absoluteLengthOfString--;
            if (absoluteLengthOfString)
            {
                if (encodedString[absoluteLengthOfString - 1] == '=')
                {
                    absoluteLengthOfString--;
                    numberOfBytesToAdd = 1;
                }
                else
                {
                    numberOfBytesToAdd = 2;
                }
            }
            else
            {
                numberOfBytesToAdd = 2;
            }
        }
    }
    return ((absoluteLengthOfString / 4)*3) + numberOfBytesToAdd;
}

static void Base64decode(unsigned char *decodedString, const char *base64String)
{

    size_t numberOfEncodedChars;
    size_t indexOfFirstEncodedChar;
    size_t decodedIndex;

    //
    // We can only operate on individual bytes.  If we attempt to work
    // on anything larger we could get an alignment fault on some
    // architectures
    //

    numberOfEncodedChars = numberOfBase64Characters(base64String);
    indexOfFirstEncodedChar = 0;
    decodedIndex = 0;
    while (numberOfEncodedChars >= 4)
    {
        unsigned char c1;
        unsigned char c2;
        unsigned char c3;
        unsigned char c4;
        (void)base64toValue(base64String[indexOfFirstEncodedChar], &c1);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 1], &c2);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 2], &c3);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 3], &c4);
        decodedString[decodedIndex] = (c1 << 2) | (c2 >> 4);
        decodedIndex++;
        decodedString[decodedIndex] = ((c2 & 0x0f) << 4) | (c3 >> 2);
        decodedIndex++;
        decodedString[decodedIndex] = ((c3 & 0x03) << 6) | c4;
        decodedIndex++;
        numberOfEncodedChars -= 4;
        indexOfFirstEncodedChar += 4;

    }

    if (numberOfEncodedChars == 2)
    {
        unsigned char c1;
        unsigned char c2;
        (void)base64toValue(base64String[indexOfFirstEncodedChar], &c1);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 1], &c2);
        decodedString[decodedIndex] = (c1 << 2) | (c2 >> 4);
    }
    else if (numberOfEncodedChars == 3)
    {
        unsigned char c1;
        unsigned char c2;
        unsigned char c3;
        (void)base64toValue(base64String[indexOfFirstEncodedChar], &c1);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 1], &c2);
        (void)base64toValue(base64String[indexOfFirstEncodedChar + 2], &c3);
        decodedString[decodedIndex] = (c1 << 2) | (c2 >> 4);
        decodedIndex++;
        decodedString[decodedIndex] = ((c2 & 0x0f) << 4) | (c3 >> 2);
    }
}

BUFFER_HANDLE Base64_Decoder(const char* source)
{
    BUFFER_HANDLE result = NULL;
    /*Codes_SRS_BASE64_06_008: [If source is NULL then Base64_Decode shall return NULL.]*/
    if (source)
    {
        size_t lengthOfSource = numberOfBase64Characters(source);
        if ((lengthOfSource % 4) == 1)
        {
            /*Codes_SRS_BASE64_06_011: [If the source string has an invalid length for a base 64 encoded string then Base64_Decode shall return NULL.]*/
            LogError("Invalid length Base64 string!\r\n");
        }
        else
        {
            if ((result = BUFFER_new()) == NULL)
            {
                /*Codes_SRS_BASE64_06_010: [If there is any memory allocation failure during the decode then Base64_Decode shall return NULL.]*/
                LogError("Could not create a buffer to decoding.\r\n");
            }
            else
            {
                size_t sizeOfOutputBuffer = Base64decode_len(source);
                /*Codes_SRS_BASE64_06_009: [If the string pointed to by source is zero length then the handle returned shall refer to a zero length buffer.]*/
                if (sizeOfOutputBuffer > 0)
                {
                    if (BUFFER_pre_build(result, sizeOfOutputBuffer) != 0)
                    {
                        /*Codes_SRS_BASE64_06_010: [If there is any memory allocation failure during the decode then Base64_Decode shall return NULL.]*/
                        LogError("Could not prebuild a buffer for base 64 decoding.\r\n");
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        Base64decode(BUFFER_u_char(result), source);
                    }
                }
            }
        }
    }
    return result;
}


static STRING_HANDLE Base64_Encode_Internal(const unsigned char* source, size_t size)
{
    STRING_HANDLE result;
    size_t neededSize = 0;
    char* encoded;
    size_t currentPosition = 0;
    neededSize += (size == 0) ? (0) : ((((size - 1) / 3) + 1) * 4);
    neededSize += 1; /*+1 because \0 at the end of the string*/
    /*Codes_SRS_BASE64_06_006: [If when allocating memory to produce the encoding a failure occurs then Base64_Encode shall return NULL.]*/
    encoded = (char*)malloc(neededSize);
    if (encoded == NULL)
    {
        result = NULL;
        LogError("Base64_Encode:: Allocation failed.\r\n");
    }
    else
    {
        /*b0            b1(+1)          b2(+2)
        7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
        |----c1---| |----c2---| |----c3---| |----c4---|
        */

        size_t destinationPosition = 0;
        while (size - currentPosition >= 3)
        {
            char c1 = base64char[source[currentPosition] >> 2];
            char c2 = base64char[
                ((source[currentPosition] & 3) << 4) |
                    (source[currentPosition + 1] >> 4)
            ];
            char c3 = base64char[
                ((source[currentPosition + 1] & 0x0F) << 2) |
                    ((source[currentPosition + 2] >> 6) & 3)
            ];
            char c4 = base64char[
                source[currentPosition + 2] & 0x3F
            ];
            currentPosition += 3;
            encoded[destinationPosition++] = c1;
            encoded[destinationPosition++] = c2;
            encoded[destinationPosition++] = c3;
            encoded[destinationPosition++] = c4;

        }
        if (size - currentPosition == 2)
        {
            char c1 = base64char[source[currentPosition] >> 2];
            char c2 = base64char[
                ((source[currentPosition] & 0x03) << 4) |
                    (source[currentPosition + 1] >> 4)
            ];
            char c3 = base64b16[source[currentPosition + 1] & 0x0F];
            encoded[destinationPosition++] = c1;
            encoded[destinationPosition++] = c2;
            encoded[destinationPosition++] = c3;
            encoded[destinationPosition++] = '=';
        }
        else if (size - currentPosition == 1)
        {
            char c1 = base64char[source[currentPosition] >> 2];
            char c2 = base64b8[source[currentPosition] & 0x03];
            encoded[destinationPosition++] = c1;
            encoded[destinationPosition++] = c2;
            encoded[destinationPosition++] = '=';
            encoded[destinationPosition++] = '=';
        }

        /*null terminating the string*/
        encoded[destinationPosition] = '\0';
        /*Codes_SRS_BASE64_06_007: [Otherwise Base64_Encode shall return a pointer to STRING, that string contains the base 64 encoding of input.]*/
        result = STRING_new_with_memory(encoded);
        if (result == NULL)
        {
            free(encoded);
            LogError("Base64_Encode:: Allocation failed for return value.\r\n");
        }
    }
    return result;
}

STRING_HANDLE Base64_Encode_Bytes(const unsigned char* source, size_t size)
{
    STRING_HANDLE result;
    /*Codes_SRS_BASE64_02_001: [If source is NULL then Base64_Encode_Bytes shall return NULL.] */
    if (source == NULL)
    {
        result = NULL;
    }
    /*Codes_SRS_BASE64_02_002: [If source is not NULL and size is zero, then Base64_Encode_Bytes shall produce an empty STRING_HANDLE.] */
    else if (size == 0)
    {
        result = STRING_new(); /*empty string*/
    }
    else
    {
        result = Base64_Encode_Internal(source, size);
    }
    return result;
}

STRING_HANDLE Base64_Encode(BUFFER_HANDLE input)
{
    STRING_HANDLE result;
    /*the following will happen*/
    /*1. the "data" of the binary shall be "eaten" 3 characters at a time and produce 4 base64 encoded characters for as long as there are more than 3 characters still to process*/
    /*2. the remaining characters (1 or 2) shall be encoded.*/
    /*there's a level of assumption that 'a' corresponds to 0b000000 and that '_' corresponds to 0b111111*/
    /*the encoding will use the optional [=] or [==] at the end of the encoded string, so that other less standard aware libraries can do their work*/
    /*these are the bits of the 3 normal bytes to be encoded*/

    /*Codes_SRS_BASE64_06_001: [If input is NULL then Base64_Encode shall return NULL.]*/
    if (input == NULL)
    {
        result = NULL;
        LogError("Base64_Encode:: NULL input\r\n");
    }
    else
    {
        size_t inputSize;
        const unsigned char* inputBinary;
        if ((BUFFER_content(input, &inputBinary) != 0) ||
            (BUFFER_size(input, &inputSize) != 0))
        {
            result = NULL;
            LogError("Base64_Encode:: BUFFER_routines failure.\r\n");
        }
        else
        {
            result = Base64_Encode_Internal(inputBinary, inputSize);
        }
    }
    return result;
}
