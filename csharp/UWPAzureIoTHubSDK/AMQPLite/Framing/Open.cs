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
    /// The Open class defines the connection negotiation parameters.
    /// </summary>
    public sealed class Open : DescribedList
    {
        /// <summary>
        /// Initializes the Open object.
        /// </summary>
        public Open()
            : base(Codec.Open, 10)
        {
        }

        /// <summary>
        /// Gets or sets the container-id field.
        /// </summary>
        public string ContainerId
        {
            get { return (string)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        /// <summary>
        /// Gets or sets the hostname field.
        /// </summary>
        public string HostName
        {
            get { return (string)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        /// <summary>
        /// Gets or sets the max-frame-size field.
        /// </summary>
        public uint MaxFrameSize
        {
            get { return this.Fields[2] == null ? uint.MaxValue : (uint)this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        /// <summary>
        /// Gets or sets the channel-max field.
        /// </summary>
        public ushort ChannelMax
        {
            get { return this.Fields[3] == null ? ushort.MaxValue : (ushort)this.Fields[3]; }
            set { this.Fields[3] = value; }
        }

        /// <summary>
        /// Gets or sets the idle-time-out field.
        /// </summary>
        public uint IdleTimeOut
        {
            get { return this.Fields[4] == null ? uint.MaxValue : (uint)this.Fields[4]; }
            set { this.Fields[4] = value; }
        }

        /// <summary>
        /// Gets or sets the outgoing-locales field.
        /// </summary>
        public Symbol[] OutgoingLocales
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 5); }
            set { this.Fields[5] = value; }
        }

        /// <summary>
        /// Gets or sets the incoming-locales field.
        /// </summary>
        public Symbol[] IncomingLocales
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 6); }
            set { this.Fields[6] = value; }
        }

        /// <summary>
        /// Gets or sets the offered-capabilities field.
        /// </summary>
        public Symbol[] OfferedCapabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 7); }
            set { this.Fields[7] = value; }
        }

        /// <summary>
        /// Gets or sets the desired-capabilities field.
        /// </summary>
        public Symbol[] DesiredCapabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 8); }
            set { this.Fields[8] = value; }
        }

        /// <summary>
        /// Gets or sets the properties field.
        /// </summary>
        public Fields Properties
        {
            get { return Amqp.Types.Fields.From(this.Fields, 9); }
            set { this.Fields[9] = value; }
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current open object.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return this.GetDebugString(
                "open",
                new object[] { "container-id", "host-name", "max-frame-size", "channel-max", "idle-time-out", "outgoing-locales", "incoming-locales", "offered-capabilities", "desired-capabilities", "properties" },
                this.Fields);
#endif
        }
    }
}