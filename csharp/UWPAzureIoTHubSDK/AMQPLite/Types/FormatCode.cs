//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace Amqp.Types
{
    static class FormatCode
    {
        public const byte
        
        Described = 0x00,

        // fixed width
        Null = 0x40,
        Boolean = 0x56,
        BooleanTrue = 0x41,
        BooleanFalse = 0x42,
        UInt0 = 0x43,
        ULong0 = 0x44,
        UByte = 0x50,
        UShort = 0x60,
        UInt = 0x70,
        ULong = 0x80,
        Byte = 0x51,
        Short = 0x61,
        Int = 0x71,
        Long = 0x81,
        SmallUInt = 0x52,
        SmallULong = 0x53,
        SmallInt = 0x54,
        SmallLong = 0x55,
        Float = 0x72,
        Double = 0x82,
        Decimal32 = 0x74,
        Decimal64 = 0x84,
        Decimal128 = 0x94,
        Char = 0x73,
        TimeStamp = 0x83,
        Uuid = 0x98,

        // variable
        Binary8 = 0xa0,
        Binary32 = 0xb0,
        String8Utf8 = 0xa1,
        String32Utf8 = 0xb1,
        Symbol8 = 0xa3,
        Symbol32 = 0xb3,

        // compound
        List0 = 0x45,
        List8 = 0xc0,
        List32 = 0xd0,
        Map8 = 0xc1,
        Map32 = 0xd1,
        Array8 = 0xe0,
        Array32 = 0xf0;
    }
}
