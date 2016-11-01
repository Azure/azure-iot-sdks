/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.iothub.ws;

public interface WebSocketHeader
{
    //  RFC6455
    //  +---------------------------------------------------------------+
    //  0                   1                   2                   3   |
    //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 |
    //  +-+-+-+-+-------+-+-------------+-------------------------------+
    //  |F|R|R|R| opcode|M| Payload len |   Extended payload length     |
    //  |I|S|S|S|  (4)  |A|     (7)     |            (16/64)            |
    //  |N|V|V|V|       |S|             |  (if payload len==126/127)    |
    //  | |1|2|3|       |K|             |                               |
    //  +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    //  |     Extended payload length continued, if payload len == 127  |
    //  + - - - - - - - - - - - - - - - +-------------------------------+
    //  |                               | Masking-key, if MASK set to 1 |
    //  +-------------------------------+-------------------------------+
    //  | Masking-key (continued)       |          Payload Data         |
    //  +-------------------------------- - - - - - - - - - - - - - - - +
    //  :                     Payload Data continued ...                :
    //  + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    //  |                     Payload Data continued ...                |
    //  +---------------------------------------------------------------+

    public static final byte MIN_HEADER_LENGTH = 2;
    public static final byte MIN_HEADER_LENGTH_MASKED = 6;

    public static final byte MED_HEADER_LENGTH_NOMASK = 4;
    public static final byte MED_HEADER_LENGTH_MASKED = 8;

    public static final byte MAX_HEADER_LENGTH_NOMASK = 10;
    public static final byte MAX_HEADER_LENGTH_MASKED = 14;

    // Masks
    public static final byte FINBIT_MASK = (byte) 0x80;
    public static final byte OPCODE_MASK = (byte) 0x0F;
    public static final byte OPCODE_CONTINUATION = (byte) 0x00;
    public static final byte OPCODE_BINARY = (byte) 0x02;
    public static final byte OPCODE_CLOSE = (byte) 0x08;
    public static final byte OPCODE_PING = (byte) 0x09;
    public static final byte OPCODE_PONG = (byte) 0x0A;
    public static final byte MASKBIT_MASK = (byte) 0x80;
    public static final byte PAYLOAD_MASK = (byte) 0x7F;

    public static final byte FINAL_OPCODE_BINARY = FINBIT_MASK | OPCODE_BINARY;

    public static final byte PAYLOAD_SHORT_MAX = 0x7D;
    public static final int PAYLOAD_MEDIUM_MAX = 0xFFFF;
    public static final int PAYLOAD_LARGE_MAX = 0x7FFFFFFF;
    public static final byte PAYLOAD_EXTENDED_16 = 0x7E;
    public static final byte PAYLOAD_EXTENDED_64 = 0x7F;


}
