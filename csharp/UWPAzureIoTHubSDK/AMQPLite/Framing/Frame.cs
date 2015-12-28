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

namespace Amqp.Framing
{
    using System;
    using Amqp.Types;

    enum FrameType : byte
    {
        Amqp = 0,
        Sasl = 1
    }

    static class Frame
    {
        const byte DOF = 2;
        const int cmdBufferSize = 128;

        public static ByteBuffer Encode(FrameType type, ushort channel, DescribedList command)
        {
            ByteBuffer buffer = new ByteBuffer(cmdBufferSize, true);
            EncodeFrame(buffer, type, channel, command);
            AmqpBitConverter.WriteInt(buffer.Buffer, 0, buffer.Length);
            return buffer;
        }

        public static ByteBuffer Encode(FrameType type, ushort channel, Transfer transfer,
            ByteBuffer payload, int maxFrameSize, out int payloadSize)
        {
            int bufferSize = cmdBufferSize + payload.Length;
            if (bufferSize > maxFrameSize)
            {
                bufferSize = maxFrameSize;
            }

            bool more = false;   // estimate it first
            if (payload.Length > bufferSize - 32)
            {
                transfer.More = more = true;
            }

            ByteBuffer buffer = new ByteBuffer(bufferSize, false);
            EncodeFrame(buffer, type, channel, transfer);

            if (more && payload.Length <= buffer.Size)
            {
                // guessed it wrong. correct it
                transfer.More = false;
                buffer.Reset();
                EncodeFrame(buffer, type, channel, transfer);
            }

            payloadSize = Math.Min(payload.Length, buffer.Size);
            AmqpBitConverter.WriteBytes(buffer, payload.Buffer, payload.Offset, payloadSize);
            payload.Complete(payloadSize);
            AmqpBitConverter.WriteInt(buffer.Buffer, 0, buffer.Length);
            return buffer;
        }

        public static void GetFrame(ByteBuffer buffer, out ushort channel, out DescribedList command)
        {
            AmqpBitConverter.ReadUInt(buffer);
            AmqpBitConverter.ReadUByte(buffer);
            AmqpBitConverter.ReadUByte(buffer);
            channel = AmqpBitConverter.ReadUShort(buffer);
            if (buffer.Length > 0)
            {
                command = (DescribedList)Codec.Decode(buffer);
            }
            else
            {
                command = null;
            }
        }

        static void EncodeFrame(ByteBuffer buffer, FrameType type, ushort channel, DescribedList command)
        {
            AmqpBitConverter.WriteUInt(buffer, 0u);
            AmqpBitConverter.WriteUByte(buffer, DOF);
            AmqpBitConverter.WriteUByte(buffer, (byte)type);
            AmqpBitConverter.WriteUShort(buffer, channel);
            Codec.Encode(command, buffer);
        }
    }
}