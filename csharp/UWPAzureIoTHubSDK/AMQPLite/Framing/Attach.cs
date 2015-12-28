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
    using Amqp.Types;

    /// <summary>
    /// The Attach class defines an attach frame to attach a Link Endpoint to the Session.
    /// </summary>
    public sealed class Attach : DescribedList
    {
        /// <summary>
        /// Initializes an attach object.
        /// </summary>
        public Attach()
            : base(Codec.Attach, 14)
        {
        }

        /// <summary>
        /// Gets or sets the name field.
        /// </summary>
        public string LinkName
        {
            get { return (string)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        /// <summary>
        /// Gets or sets the handle field.
        /// </summary>
        public uint Handle
        {
            get { return this.Fields[1] == null ? uint.MinValue : (uint)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        /// <summary>
        /// Gets or sets the role field.
        /// </summary>
        public bool Role
        {
            get { return this.Fields[2] == null ? false : (bool)this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        /// <summary>
        /// Gets or sets the snd-settle-mode field.
        /// </summary>
        public SenderSettleMode SndSettleMode
        {
            get { return this.Fields[3] == null ? SenderSettleMode.Unsettled : (SenderSettleMode)this.Fields[3]; }
            set { this.Fields[3] = (byte)value; }
        }

        /// <summary>
        /// Gets or sets the rcv-settle-mode field.
        /// </summary>
        public ReceiverSettleMode RcvSettleMode
        {
            get { return this.Fields[4] == null ? ReceiverSettleMode.First : (ReceiverSettleMode)this.Fields[4]; }
            set { this.Fields[4] = (byte)value; }
        }

        /// <summary>
        /// Gets or sets the source field.
        /// </summary>
        public object Source
        {
            get { return this.Fields[5]; }
            set { this.Fields[5] = value; }
        }

        /// <summary>
        /// Gets or sets the target field.
        /// </summary>
        public object Target
        {
            get { return this.Fields[6]; }
            set { this.Fields[6] = value; }
        }

        /// <summary>
        /// Gets or sets the unsettled field.
        /// </summary>
        public Map Unsettled
        {
            get { return (Map)this.Fields[7]; }
            set { this.Fields[7] = value; }
        }

        /// <summary>
        /// Gets or sets the incomplete-unsettled field.
        /// </summary>
        public bool IncompleteUnsettled
        {
            get { return this.Fields[8] == null ? false : (bool)this.Fields[8]; }
            set { this.Fields[8] = value; }
        }

        /// <summary>
        /// Gets or sets the initial-delivery-count field.
        /// </summary>
        public uint InitialDeliveryCount
        {
            get { return this.Fields[9] == null ? uint.MinValue : (uint)this.Fields[9]; }
            set { this.Fields[9] = value; }
        }

        /// <summary>
        /// Gets or sets the max-message-size field.
        /// </summary>
        public ulong MaxMessageSize
        {
            get { return this.Fields[10] == null ? ulong.MaxValue : (ulong)this.Fields[10]; }
            set { this.Fields[10] = value; }
        }

        /// <summary>
        /// Gets or sets the offered-capabilities field.
        /// </summary>
        public Symbol[] OfferedCapabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 11); }
            set { this.Fields[11] = value; }
        }

        /// <summary>
        /// Gets or sets the desired-capabilities field.
        /// </summary>
        public Symbol[] DesiredCapabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 12); }
            set { this.Fields[12] = value; }
        }

        /// <summary>
        /// Gets or sets the properties field.
        /// </summary>
        public Fields Properties
        {
            get { return Amqp.Types.Fields.From(this.Fields, 13); }
            set { this.Fields[13] = value; }
        }
        
#if TRACE
        /// <summary>
        /// Returns a string that represents the current object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "attach",
                new object[] { "name", "handle", "role", "snd-settle-mode", "rcv-settle-mode", "source", "target", "unsettled", "incomplete-unsettled", "initial-delivery-count", "max-message-size", "offered-capabilities", "desired-capabilities", "properties" },
                this.Fields);
        }
#endif
    }
}