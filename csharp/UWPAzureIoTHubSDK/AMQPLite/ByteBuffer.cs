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

namespace Amqp
{
    using Amqp.Types;
    using System;

    /// <summary>
    /// A byte array wrapper that has read and write cursors.
    /// </summary>
    public sealed class ByteBuffer
    {
        //
        //   +---------+--------------+----------------+
        // start      read          write             end
        //
        // read - start: already consumed
        // write - read: Length (bytes to be consumed)
        // end - write: Size (free space to write)
        // end - start: Capacity
        //
        byte[] buffer;
        int start;
        int read;
        int write;
        int end;
        bool autoGrow;

        /// <summary>
        /// Initializes a new buffer from a byte array.
        /// </summary>
        /// <param name="buffer">The byte array.</param>
        /// <param name="offset">The start position.</param>
        /// <param name="count">The number of bytes.</param>
        /// <param name="capacity">The total size of the byte array from offset.</param>
        public ByteBuffer(byte[] buffer, int offset, int count, int capacity)
            : this(buffer, offset, count, capacity, false)
        {
        }

        /// <summary>
        /// Initializes a new buffer of a specified size.
        /// </summary>
        /// <param name="size">The size in bytes.</param>
        /// <param name="autoGrow">If the buffer should auto-grow when a write size is larger than the buffer size.</param>
        public ByteBuffer(int size, bool autoGrow)
            : this(new byte[size], 0, 0, size, autoGrow)
        {
        }

        ByteBuffer(byte[] buffer, int offset, int count, int capacity, bool autoGrow)
        {
            this.buffer = buffer;
            this.start = offset;
            this.read = offset;
            this.write = offset + count;
            this.end = offset + capacity;
            this.autoGrow = autoGrow;
        }

        /// <summary>
        /// Gets the byte array.
        /// </summary>
        public byte[] Buffer
        {
            get { return this.buffer; }
        }

        /// <summary>
        /// Gets the capacity.
        /// </summary>
        public int Capacity
        {
            get { return this.end - this.start; }
        }

        /// <summary>
        /// Gets the current offset (read position).
        /// </summary>
        public int Offset
        {
            get { return this.read; }
        }

        /// <summary>
        /// Gets the remaining size for write.
        /// </summary>
        public int Size
        {
            get { return this.end - this.write; }
        }

        /// <summary>
        /// Gets the available size for read.
        /// </summary>
        public int Length
        {
            get { return this.write - this.read; }
        }

        /// <summary>
        /// Gets the write position.
        /// </summary>
        public int WritePos
        {
            get { return this.write; }
        }

        /// <summary>
        /// Verifies that if the buffer has enough bytes for read or enough room for write and grow the buffer if needed.
        /// </summary>
        /// <param name="write">Operation to verify. True for write and false for read.</param>
        /// <param name="dataSize">The size to read or write.</param>
        public void Validate(bool write, int dataSize)
        {
            bool valid = false;
            if (write)
            {
                if (this.Size < dataSize && this.autoGrow)
                {
                    int newSize = Math.Max(this.Capacity * 2, this.Capacity + dataSize);
                    byte[] newBuffer = new byte[newSize];
                    Array.Copy(this.buffer, this.start, newBuffer, 0, this.Length);
                    this.buffer = newBuffer;
                    int consumed = this.read - this.start;
                    int written = this.write - this.start;
                    this.start = 0;
                    this.read = consumed;
                    this.write = written;
                    this.end = newSize;
                }

                valid = this.Size >= dataSize;
            }
            else
            {
                valid = this.Length >= dataSize;
            }

            if (!valid)
            {
                throw new AmqpException(ErrorCode.DecodeError, "buffer too small");
            }
        }

        /// <summary>
        /// Advances the write position. As a result, length is increased by size.
        /// </summary>
        /// <param name="size">Size to advance.</param>
        public void Append(int size)
        {
            Fx.Assert(size >= 0, "size must be positive.");
            Fx.Assert((this.write + size) <= this.end, "Append size too large.");
            this.write += size;
        }

        /// <summary>
        /// Advances the read position.
        /// </summary>
        /// <param name="size">Size to advance.</param>
        public void Complete(int size)
        {
            Fx.Assert(size >= 0, "size must be positive.");
            Fx.Assert((this.read + size) <= this.write, "Complete size too large.");
            this.read += size;
        }

        /// <summary>
        /// Sets the read position.
        /// </summary>
        /// <param name="seekPosition">Position to set.</param>
        public void Seek(int seekPosition)
        {
            Fx.Assert(seekPosition >= 0, "seekPosition must not be negative.");
            Fx.Assert((this.start + seekPosition) <= this.write, "seekPosition too large.");
            this.read = this.start + seekPosition;
        }

        /// <summary>
        /// Moves back the write position. As a result, length is decreased by size.
        /// </summary>
        /// <param name="size"></param>
        public void Shrink(int size)
        {
            Fx.Assert(size >= 0 && size <= this.Length, "size must be positive and not greater then length.");
            this.write -= size;
        }

        /// <summary>
        /// Resets read and write position to the initial state.
        /// </summary>
        public void Reset()
        {
            this.read = this.start;
            this.write = this.start;
        }

        /// <summary>
        /// Adjusts the read and write position.
        /// </summary>
        /// <param name="offset">Read position to set.</param>
        /// <param name="length">Length from read position to set the write position.</param>
        public void AdjustPosition(int offset, int length)
        {
            Fx.Assert(offset >= this.start, "Invalid offset!");
            Fx.Assert(offset + length <= this.end, "length too large!");
            this.read = offset;
            this.write = this.read + length;
        }
    }
}
