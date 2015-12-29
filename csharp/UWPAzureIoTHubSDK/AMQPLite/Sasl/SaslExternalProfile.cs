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

namespace Amqp.Sasl
{
    using Amqp.Framing;
    using Amqp.Types;

    sealed class SaslExternalProfile : SaslProfile
    {
        public const string Name = "EXTERNAL";

        public SaslExternalProfile()
        {
        }

        protected override ITransport UpgradeTransport(ITransport transport)
        {
            return transport;
        }

        protected override DescribedList GetStartCommand(string hostname)
        {
            return new SaslInit() { Mechanism = Name };
        }

        protected override DescribedList OnCommand(DescribedList command)
        {
            if (command.Descriptor.Code == Codec.SaslInit.Code)
            {
                return new SaslOutcome() { Code = SaslCode.Ok };
            }
            else if (command.Descriptor.Code == Codec.SaslMechanisms.Code)
            {
                return null;
            }

            throw new AmqpException(ErrorCode.NotAllowed, command.ToString());
        }
    }
}