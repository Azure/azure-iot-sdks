// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Diagnostics.CodeAnalysis;
    using System.Runtime.Serialization;

    [SuppressMessage(FxCop.Category.Design, FxCop.Rule.ExceptionsShouldBePublic, Justification = "Asserts should not be seen by users.", Scope = "Type", Target = "Microsoft.Azure.Devices.Common.AssertionFailedException")]
#if !WINDOWS_UWP
    [Serializable]
#endif
    class AssertionFailedException : Exception
    {
        public AssertionFailedException(string description)
            : base(CommonResources.GetString(CommonResources.ShipAssertExceptionMessage, description))
        {
        }

#if !WINDOWS_UWP
        protected AssertionFailedException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
