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
    static class FixedWidth
    {
        public const int FormatCode = 1;    // ext type is not used for encoding

        public const int Null = 0;
        public const int Boolean = 0;
        public const int BooleanVar = 1;
        public const int Zero = 0;
        public const int UByte = 1;
        public const int UShort = 2;
        public const int UInt = 4;
        public const int ULong = 8;
        public const int Byte = 1;
        public const int Short = 2;
        public const int Int = 4;
        public const int Long = 8;
        public const int Float = 4;
        public const int Double = 8;
        public const int Decimal32 = 4;
        public const int Decimal64 = 8;
        public const int Decimal128 = 16;
        public const int Char = 4;
        public const int TimeStamp = 8;
        public const int Uuid = 16;

        public const int NullEncoded = FormatCode + Null;
        public const int BooleanEncoded = FormatCode + Boolean;
        public const int BooleanVarEncoded = FormatCode + BooleanVar;
        public const int ZeroEncoded = FormatCode + Zero;
        public const int UByteEncoded = FormatCode + UByte;
        public const int UShortEncoded = FormatCode + UShort;
        public const int UIntEncoded = FormatCode + UInt;
        public const int ULongEncoded = FormatCode + ULong;
        public const int ByteEncoded = FormatCode + Byte;
        public const int ShortEncoded = FormatCode + Short;
        public const int IntEncoded = FormatCode + Int;
        public const int LongEncoded = FormatCode + Long;
        public const int FloatEncoded = FormatCode + Float;
        public const int DoubleEncoded = FormatCode + Double;
        public const int Decimal32Encoded = FormatCode + Decimal32;
        public const int Decimal64Encoded = FormatCode + Decimal64;
        public const int Decimal128Encoded = FormatCode + Decimal128;
        public const int CharEncoded = FormatCode + Char;
        public const int TimeStampEncoded = FormatCode + TimeStamp;
        public const int UuidEncoded = FormatCode + Uuid;
    }
}
