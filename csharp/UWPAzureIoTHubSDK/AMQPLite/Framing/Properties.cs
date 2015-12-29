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

    /// <summary>
    /// The Properties class defines the Immutable properties of the Message.
    /// </summary>
    public sealed class Properties : DescribedList
    {
        /// <summary>
        /// Initializes a properties section.
        /// </summary>
        public Properties()
            : base(Codec.Properties, 13)
        {
        }

        /// <summary>
        /// Gets or sets the message-id field.
        /// </summary>
        public string MessageId
        {
            get { return (string)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        /// <summary>
        /// Gets or sets the user-id field.
        /// </summary>
        public byte[] UserId
        {
            get { return (byte[])this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        /// <summary>
        /// Gets or sets the to field.
        /// </summary>
        public string To
        {
            get { return (string)this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        /// <summary>
        /// Gets or sets the subject field.
        /// </summary>
        public string Subject
        {
            get { return (string)this.Fields[3]; }
            set { this.Fields[3] = value; }
        }

        /// <summary>
        /// Gets or sets the reply-to field.
        /// </summary>
        public string ReplyTo
        {
            get { return (string)this.Fields[4]; }
            set { this.Fields[4] = value; }
        }

        /// <summary>
        /// Gets or sets the correlation-id field.
        /// </summary>
        public string CorrelationId
        {
            get { return (string)this.Fields[5]; }
            set { this.Fields[5] = value; }
        }

        /// <summary>
        /// Gets or sets the content-type field.
        /// </summary>
        public Symbol ContentType
        {
            get { return (Symbol)this.Fields[6]; }
            set { this.Fields[6] = value; }
        }

        /// <summary>
        /// Gets or sets the content-encoding field.
        /// </summary>
        public Symbol ContentEncoding
        {
            get { return (Symbol)this.Fields[7]; }
            set { this.Fields[7] = value; }
        }

        /// <summary>
        /// Gets or sets the absolute-expiry-time field.
        /// </summary>
        public DateTime AbsoluteExpiryTime
        {
            get { return this.Fields[8] == null ? DateTime.MinValue : (DateTime)this.Fields[8]; }
            set { this.Fields[8] = value; }
        }

        /// <summary>
        /// Gets or sets the creation-time field.
        /// </summary>
        public DateTime CreationTime
        {
            get { return this.Fields[9] == null ? DateTime.MinValue : (DateTime)this.Fields[9]; }
            set { this.Fields[9] = value; }
        }

        /// <summary>
        /// Gets or sets the group-id field.
        /// </summary>
        public string GroupId
        {
            get { return (string)this.Fields[10]; }
            set { this.Fields[10] = value; }
        }

        /// <summary>
        /// Gets or sets the group-sequence field.
        /// </summary>
        public uint GroupSequence
        {
            get { return this.Fields[11] == null ? uint.MinValue : (uint)this.Fields[11]; }
            set { this.Fields[11] = value; }
        }

        /// <summary>
        /// Gets or sets the reply-to-group-id field.
        /// </summary>
        public string ReplyToGroupId
        {
            get { return (string)this.Fields[12]; }
            set { this.Fields[12] = value; }
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current properties object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "properties",
                new object[] { "message-id", "user-id", "to", "subject", "reply-to", "correlation-id", "content-type", "content-encoding", "absolute-expiry-time", "creation-time", "group-id", "group-sequence", "reply-to-group-id" },
                this.Fields);
        }
#endif
    }
}