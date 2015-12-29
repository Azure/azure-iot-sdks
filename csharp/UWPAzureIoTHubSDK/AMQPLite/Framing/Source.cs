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
    /// The source is comprised of an address coupled with properties that determine
    /// message transfer behavior.
    /// </summary>
    public sealed class Source : DescribedList
    {
        /// <summary>
        /// Initializes a source object.
        /// </summary>
        public Source()
            : base(Codec.Source, 11)
        {
        }

        /// <summary>
        /// Gets or sets the address field.
        /// </summary>
        public string Address
        {
            get { return (string)this.Fields[0]; }
            set { this.Fields[0] = value; }
        }

        /// <summary>
        /// Gets or sets the durable field.
        /// </summary>
        public uint Durable
        {
            get { return this.Fields[1] == null ? 0u : (uint)this.Fields[1]; }
            set { this.Fields[1] = value; }
        }

        /// <summary>
        /// Gets or sets the expiry-policy field.
        /// </summary>
        public Symbol ExpiryPolicy
        {
            get { return (Symbol)this.Fields[2]; }
            set { this.Fields[2] = value; }
        }

        /// <summary>
        /// Gets or sets the timeout field.
        /// </summary>
        public uint Timeout
        {
            get { return this.Fields[3] == null ? 0u : (uint)this.Fields[3]; }
            set { this.Fields[3] = value; }
        }

        /// <summary>
        /// Gets or sets the dynamic field.
        /// </summary>
        public bool Dynamic
        {
            get { return this.Fields[4] == null ? false : (bool)this.Fields[4]; }
            set { this.Fields[4] = value; }
        }

        /// <summary>
        /// Gets or sets the dynamic-node-properties field.
        /// </summary>
        public Fields DynamicNodeProperties
        {
            get { return Amqp.Types.Fields.From(this.Fields, 5); }
            set { this.Fields[5] = value; }
        }

        /// <summary>
        /// Gets or sets the distribution-mode field.
        /// </summary>
        public Symbol DistributionMode
        {
            get { return (Symbol)this.Fields[6]; }
            set { this.Fields[6] = value; }
        }

        /// <summary>
        /// Gets or sets the filter field.
        /// </summary>
        public Map FilterSet
        {
            get { return (Map)this.Fields[7]; }
            set { this.Fields[7] = value; }
        }

        /// <summary>
        /// Gets or sets the default-outcome field.
        /// </summary>
        public Outcome DefaultOutcome
        {
            get { return (Outcome)this.Fields[8]; }
            set { this.Fields[8] = value; }
        }

        /// <summary>
        /// Gets or sets the outcomes field.
        /// </summary>
        public Symbol[] Outcomes
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 9); }
            set { this.Fields[9] = value; }
        }

        /// <summary>
        /// Gets or sets the capabilities field.
        /// </summary>
        public Symbol[] Capabilities
        {
            get { return Codec.GetSymbolMultiple(this.Fields, 10); }
            set { this.Fields[10] = value; }
        }

#if TRACE
        /// <summary>
        /// Returns a string that represents the current source object.
        /// </summary>
        public override string ToString()
        {
            return this.GetDebugString(
                "source",
                new object[] { "address", "durable", "expiry-policy", "timeout", "dynamic", "dynamic-node-properties", "distribution-mode", "filter", "default-outcome", "outcomes", "capabilities" },
                this.Fields);
        }
#endif
    }
}