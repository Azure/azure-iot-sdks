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

    /// <summary>
    /// RFC1982: http://tools.ietf.org/html/rfc1982
    /// </summary>
    struct SequenceNumber
    {
        int sequenceNumber;

        public SequenceNumber(uint value)
        {
            this.sequenceNumber = (int)value;
        }

        public static implicit operator SequenceNumber(uint value)
        {
            return new SequenceNumber(value);
        }

        public static implicit operator uint(SequenceNumber value)
        {
            return (uint)value.sequenceNumber;
        }

        public int CompareTo(SequenceNumber value)
        {
            int delta = this.sequenceNumber - value.sequenceNumber;
            if (delta == int.MinValue)
            {
                // Behavior of comparing 0u-2147483648u, 1u-2147483649u, ...
                // is undefined, so we do not allow it.
                throw new AmqpException(ErrorCode.NotAllowed,
                    Fx.Format(SRAmqp.InvalidSequenceNumberComparison, (uint)this.sequenceNumber, (uint)value.sequenceNumber));
            }

            return delta;
        }

        public static SequenceNumber operator ++(SequenceNumber value)
        {
            value.sequenceNumber++;
            return value;
        }

        public static SequenceNumber operator +(SequenceNumber value1, int delta)
        {
            return (uint)unchecked(value1.sequenceNumber + delta);
        }

        public static SequenceNumber operator -(SequenceNumber value1, int delta)
        {
            return (uint)unchecked(value1.sequenceNumber - delta);
        }

        public static int operator -(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.sequenceNumber - value2.sequenceNumber;
        }

        public static bool operator ==(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.sequenceNumber == value2.sequenceNumber;
        }

        public static bool operator !=(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.sequenceNumber != value2.sequenceNumber;
        }

        public static bool operator >(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.CompareTo(value2) > 0;
        }

        public static bool operator >=(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.CompareTo(value2) >= 0;
        }

        public static bool operator <(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.CompareTo(value2) < 0;
        }

        public static bool operator <=(SequenceNumber value1, SequenceNumber value2)
        {
            return value1.CompareTo(value2) <= 0;
        }

        public override int GetHashCode()
        {
            return this.sequenceNumber.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            return obj is SequenceNumber && ((SequenceNumber)obj).sequenceNumber == this.sequenceNumber;
        }

        public override string ToString()
        {
            return this.sequenceNumber.ToString();
        }
    }
}
