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
    using Amqp.Sasl;
#if DOTNET
    using Amqp.Transactions;
#endif
    using Amqp.Types;
    using System.Collections;

    static class Codec
    {
        // transport performatives
        public static readonly Descriptor Open = new Descriptor(0x0000000000000010, "amqp:open:list");
        public static readonly Descriptor Begin = new Descriptor(0x0000000000000011, "amqp:begin:list");
        public static readonly Descriptor Attach = new Descriptor(0x0000000000000012, "amqp:attach:list");
        public static readonly Descriptor Flow = new Descriptor(0x0000000000000013, "amqp:flow:list");
        public static readonly Descriptor Transfer = new Descriptor(0x0000000000000014, "amqp:transfer:list");
        public static readonly Descriptor Dispose = new Descriptor(0x0000000000000015, "amqp:disposition:list");
        public static readonly Descriptor Detach = new Descriptor(0x0000000000000016, "amqp:detach:list");
        public static readonly Descriptor End = new Descriptor(0x0000000000000017, "amqp:end:list");
        public static readonly Descriptor Close = new Descriptor(0x0000000000000018, "amqp:close:list");

        public static readonly Descriptor Error = new Descriptor(0x000000000000001d, "amqp:error:list");

        // outcome
        public static readonly Descriptor Received = new Descriptor(0x0000000000000023, "amqp:received:list");
        public static readonly Descriptor Accepted = new Descriptor(0x0000000000000024, "amqp:accepted:list");
        public static readonly Descriptor Rejected = new Descriptor(0x0000000000000025, "amqp:rejected:list");
        public static readonly Descriptor Released = new Descriptor(0x0000000000000026, "amqp:released:list");
        public static readonly Descriptor Modified = new Descriptor(0x0000000000000027, "amqp:modified:list");

        public static readonly Descriptor Source =   new Descriptor(0x0000000000000028, "amqp:source:list");
        public static readonly Descriptor Target =   new Descriptor(0x0000000000000029, "amqp:target:list");

        // sasl
        public static readonly Descriptor SaslMechanisms = new Descriptor(0x0000000000000040, "amqp:sasl-mechanisms:list");
        public static readonly Descriptor SaslInit = new Descriptor(0x0000000000000041, "amqp:sasl-init:list");
        public static readonly Descriptor SaslChallenge = new Descriptor(0x0000000000000042, "amqp:sasl-challenge:list");
        public static readonly Descriptor SaslResponse = new Descriptor(0x0000000000000043, "amqp:sasl-response:list");
        public static readonly Descriptor SaslOutcome = new Descriptor(0x0000000000000044, "amqp:sasl-outcome:list");

        // message
        public static readonly Descriptor Header = new Descriptor(0x0000000000000070, "amqp:header:list");
        public static readonly Descriptor DeliveryAnnotations = new Descriptor(0x0000000000000071, "amqp:delivery-annotations:map");
        public static readonly Descriptor MessageAnnotations = new Descriptor(0x0000000000000072, "amqp:message-annotations:map");
        public static readonly Descriptor Properties = new Descriptor(0x0000000000000073, "amqp:properties:list");
        public static readonly Descriptor ApplicationProperties = new Descriptor(0x0000000000000074, "amqp:application-properties:map");
        public static readonly Descriptor Data = new Descriptor(0x0000000000000075, "amqp:data:binary");
        public static readonly Descriptor AmqpSequence = new Descriptor(0x0000000000000076, "amqp:amqp-sequence:list");
        public static readonly Descriptor AmqpValue = new Descriptor(0x0000000000000077, "amqp:amqp-value:*");
        public static readonly Descriptor Footer = new Descriptor(0x0000000000000078, "amqp:footer:map");

        // transactions
#if DOTNET
        public static readonly Descriptor Coordinator = new Descriptor(0x0000000000000030, "amqp:coordinator:list");
        public static readonly Descriptor Declare = new Descriptor(0x0000000000000031, "amqp:declare:list");
        public static readonly Descriptor Discharge = new Descriptor(0x0000000000000032, "amqp:discharge:list");
        public static readonly Descriptor Declared = new Descriptor(0x0000000000000033, "amqp:declared:list");
        public static readonly Descriptor TransactionalState = new Descriptor(0x0000000000000034, "amqp:transactional-state:list");
#endif

        static Codec()
        {
            Encoder.Initialize();

            Encoder.AddKnownDescribed(Codec.Open, () => new Open());
            Encoder.AddKnownDescribed(Codec.Begin, () => new Begin());
            Encoder.AddKnownDescribed(Codec.Attach, () => new Attach());
            Encoder.AddKnownDescribed(Codec.Detach, () => new Detach());
            Encoder.AddKnownDescribed(Codec.End, () => new End());
            Encoder.AddKnownDescribed(Codec.Close, () => new Close());
            Encoder.AddKnownDescribed(Codec.Flow, () => new Flow());
            Encoder.AddKnownDescribed(Codec.Dispose, () => new Dispose());
            Encoder.AddKnownDescribed(Codec.Transfer, () => new Transfer());

            Encoder.AddKnownDescribed(Codec.Error, () => new Error());
            Encoder.AddKnownDescribed(Codec.Source, () => new Source());
            Encoder.AddKnownDescribed(Codec.Target, () => new Target());

            Encoder.AddKnownDescribed(Codec.Accepted, () => new Accepted());
            Encoder.AddKnownDescribed(Codec.Rejected, () => new Rejected());
            Encoder.AddKnownDescribed(Codec.Released, () => new Released());
            Encoder.AddKnownDescribed(Codec.Modified, () => new Modified());
            Encoder.AddKnownDescribed(Codec.Received, () => new Received());

            Encoder.AddKnownDescribed(Codec.SaslMechanisms, () => new SaslMechanisms());
            Encoder.AddKnownDescribed(Codec.SaslInit, () => new SaslInit());
            Encoder.AddKnownDescribed(Codec.SaslChallenge, () => new SaslChallenge());
            Encoder.AddKnownDescribed(Codec.SaslResponse, () => new SaslResponse());
            Encoder.AddKnownDescribed(Codec.SaslOutcome, () => new SaslOutcome());

            Encoder.AddKnownDescribed(Codec.Header, () => new Header());
            Encoder.AddKnownDescribed(Codec.DeliveryAnnotations, () => new DeliveryAnnotations());
            Encoder.AddKnownDescribed(Codec.MessageAnnotations, () => new MessageAnnotations());
            Encoder.AddKnownDescribed(Codec.Properties, () => new Properties());
            Encoder.AddKnownDescribed(Codec.ApplicationProperties, () => new ApplicationProperties());
            Encoder.AddKnownDescribed(Codec.Data, () => new Data());
            Encoder.AddKnownDescribed(Codec.AmqpSequence, () => new AmqpSequence());
            Encoder.AddKnownDescribed(Codec.AmqpValue, () => new AmqpValue());
            Encoder.AddKnownDescribed(Codec.Footer, () => new Footer());

#if DOTNET
            Encoder.AddKnownDescribed(Codec.Coordinator, () => new Coordinator());
            Encoder.AddKnownDescribed(Codec.Declare, () => new Declare());
            Encoder.AddKnownDescribed(Codec.Discharge, () => new Discharge());
            Encoder.AddKnownDescribed(Codec.Declared, () => new Declared());
            Encoder.AddKnownDescribed(Codec.TransactionalState, () => new TransactionalState());
#endif
        }

        // Transport layer should call Codec to encode/decode frames. It ensures that
        // all dependant static fields in other class are initialized correctly.
        // NETMF does not track cross-class static field/ctor dependancies

        public static void Encode(RestrictedDescribed command, ByteBuffer buffer)
        {
            Fx.Assert(command != null, "command is null!");
            command.Encode(buffer);
        }

        public static object Decode(ByteBuffer buffer)
        {
            return Encoder.ReadDescribed(buffer, Encoder.ReadFormatCode(buffer));
        }

        public static Symbol[] GetSymbolMultiple(object[] fields, int index)
        {
            if (fields[index] == null)
            {
                return null;
            }

            Symbol[] symbols = fields[index] as Symbol[];
            if (symbols != null)
            {
                return symbols;
            }

            Symbol symbol = fields[index] as Symbol;
            if (symbol != null)
            {
                symbols = new Symbol[] { symbol };
                fields[index] = symbols;
                return symbols;
            }

            throw new AmqpException(ErrorCode.InvalidField, Fx.Format("{0} {1}", index, fields[index].GetType().Name));
        }
    }
}