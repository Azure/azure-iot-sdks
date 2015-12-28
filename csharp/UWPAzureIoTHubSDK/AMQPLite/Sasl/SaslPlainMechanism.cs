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
    using System;
    using System.Text;
    using Amqp.Framing;
    using Amqp.Types;

    class SaslPlainMechanism : SaslMechanism
    {
        readonly string user;
        readonly string password;

        public SaslPlainMechanism(string user, string password)
        {
            this.user = user;
            this.password = password;
        }

        public override string Name
        {
            get { return Amqp.Sasl.SaslPlainProfile.Name; }
        }

        public override SaslProfile CreateProfile()
        {
            return new SaslPlainProfile(this);
        }

        class SaslPlainProfile : SaslProfile
        {
            readonly SaslPlainMechanism mechanism;

            public SaslPlainProfile(SaslPlainMechanism mechanism)
            {
                this.mechanism = mechanism;
            }

            protected override ITransport UpgradeTransport(ITransport transport)
            {
                return transport;
            }

            protected override DescribedList GetStartCommand(string hostname)
            {
                throw new NotImplementedException();
            }

            protected override DescribedList OnCommand(DescribedList command)
            {
                if (command.Descriptor.Code == Codec.SaslInit.Code)
                {
                    SaslInit init = (SaslInit)command;
                    SaslCode code = this.ValidateCredentials(init);
                    return new SaslOutcome() { Code = code };
                }

                throw new AmqpException(ErrorCode.NotAllowed, command.ToString());
            }

            SaslCode ValidateCredentials(SaslInit init)
            {
                byte[] response = init.InitialResponse;
                if (response.Length > 0)
                {
                    string message = Encoding.UTF8.GetString(response, 0, response.Length);
                    string[] items = message.Split('\0');
                    if (items.Length == 3 &&
                        string.Equals(this.mechanism.user, items[1], StringComparison.OrdinalIgnoreCase) &&
                        string.Equals(this.mechanism.password, items[2], StringComparison.Ordinal))
                    {
                        return SaslCode.Ok;
                    }
                }

                return SaslCode.Auth;
            }
        }
    }
}
