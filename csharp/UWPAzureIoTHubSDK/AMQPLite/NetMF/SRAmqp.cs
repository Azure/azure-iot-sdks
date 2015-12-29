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
    internal partial class SRAmqp
    {
        public static string AmqpHandleExceeded
        {
            get { return SRAmqp.GetString(StringResources.AmqpHandleExceeded); }
        }

        public static string InvalidMapKeyType
        {
            get { return SRAmqp.GetString(StringResources.InvalidMapKeyType); }
        }

        public static string LinkNotFound
        {
            get { return SRAmqp.GetString(StringResources.LinkNotFound); }
        }

        public static string AmqpUnknownDescriptor
        {
            get { return SRAmqp.GetString(StringResources.AmqpUnknownDescriptor); }
        }

        public static string SaslNegoFailed
        {
            get { return SRAmqp.GetString(StringResources.SaslNegoFailed); }
        }

        public static string AmqpTimeout
        {
            get { return SRAmqp.GetString(StringResources.AmqpTimeout); }
        }

        public static string InvalidAddressFormat
        {
            get { return SRAmqp.GetString(StringResources.InvalidAddressFormat); }
        }

        public static string InvalidDeliveryIdOnTransfer
        {
            get { return SRAmqp.GetString(StringResources.InvalidDeliveryIdOnTransfer); }
        }

        public static string AmqpOperationNotSupported
        {
            get { return SRAmqp.GetString(StringResources.AmqpOperationNotSupported); }
        }

        public static string AmqpHandleNotFound
        {
            get { return SRAmqp.GetString(StringResources.AmqpHandleNotFound); }
        }

        public static string InvalidFrameSize
        {
            get { return SRAmqp.GetString(StringResources.InvalidFrameSize); }
        }

        public static string InvalidSequenceNumberComparison
        {
            get { return SRAmqp.GetString(StringResources.InvalidSequenceNumberComparison); }
        }

        public static string AmqpInvalidFormatCode
        {
            get { return SRAmqp.GetString(StringResources.AmqpInvalidFormatCode); }
        }

        public static string DeliveryLimitExceeded
        {
            get { return SRAmqp.GetString(StringResources.DeliveryLimitExceeded); }
        }

        public static string EncodingTypeNotSupported
        {
            get { return SRAmqp.GetString(StringResources.EncodingTypeNotSupported); }
        }

        public static string AmqpChannelNotFound
        {
            get { return SRAmqp.GetString(StringResources.AmqpChannelNotFound); }
        }

        public static string AmqpIllegalOperationState
        {
            get { return SRAmqp.GetString(StringResources.AmqpIllegalOperationState); }
        }

        public static string InvalidMapCount
        {
            get { return SRAmqp.GetString(StringResources.InvalidMapCount); }
        }

        public static string WindowViolation
        {
            get { return SRAmqp.GetString(StringResources.WindowViolation); }
        }
    }
}