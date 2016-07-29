// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CONNECTION_STRING_PARSER_H
#define CONNECTION_STRING_PARSER_H

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/map.h"

#ifdef __cplusplus
extern "C" 
{
#endif

    extern MAP_HANDLE connectionstringparser_parse(STRING_HANDLE connection_string);

#ifdef __cplusplus
}
#endif

#endif /* CONNECTION_STRING_PARSER_H */
