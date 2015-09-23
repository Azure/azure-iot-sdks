// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// guids.h: definitions of GUIDs/IIDs/CLSIDs used in this VsPackage

/*
Do not use #pragma once, as this file needs to be included twice.  Once to declare the externs
for the GUIDs, and again right after including initguid.h to actually define the GUIDs.
*/



// package guid
// { ec625337-5c7b-4d7c-b472-98a4e4b1a93d }
#define guidIotClientUnitTestsTemplatePkg { 0xEC625337, 0x5C7B, 0x4D7C, { 0xB4, 0x72, 0x98, 0xA4, 0xE4, 0xB1, 0xA9, 0x3D } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_IotClientUnitTestsTemplate,
0xEC625337, 0x5C7B, 0x4D7C, 0xB4, 0x72, 0x98, 0xA4, 0xE4, 0xB1, 0xA9, 0x3D );
#endif

// Command set guid for our commands (used with IOleCommandTarget)
// { 1fd266ea-d9fd-41df-8fb1-9d5264596356 }
#define guidIotClientUnitTestsTemplateCmdSet { 0x1FD266EA, 0xD9FD, 0x41DF, { 0x8F, 0xB1, 0x9D, 0x52, 0x64, 0x59, 0x63, 0x56 } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_IotClientUnitTestsTemplateCmdSet, 
0x1FD266EA, 0xD9FD, 0x41DF, 0x8F, 0xB1, 0x9D, 0x52, 0x64, 0x59, 0x63, 0x56 );
#endif

//Guid for the image list referenced in the VSCT file
// { 0b50c3c7-92cc-4ffd-b3ef-4b58a26e21fb }
#define guidImages { 0xB50C3C7, 0x92CC, 0x4FFD, { 0xB3, 0xEF, 0x4B, 0x58, 0xA2, 0x6E, 0x21, 0xFB } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_Images, 
0xB50C3C7, 0x92CC, 0x4FFD, 0xB3, 0xEF, 0x4B, 0x58, 0xA2, 0x6E, 0x21, 0xFB );
#endif


