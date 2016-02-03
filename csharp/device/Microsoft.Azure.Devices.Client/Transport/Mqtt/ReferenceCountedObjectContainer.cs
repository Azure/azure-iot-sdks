// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using DotNetty.Common;

    abstract class ReferenceCountedObjectContainer<T> : IReferenceCounted where T : IReferenceCounted
    {
        public abstract T Value { get; set; }

        public IReferenceCounted Retain()
        {
            return this.Value.Retain();
        }

        public IReferenceCounted Retain(int increment)
        {
            return this.Value.Retain(increment);
        }

        public IReferenceCounted Touch()
        {
            return this.Value.Touch();
        }

        public IReferenceCounted Touch(object hint)
        {
            return this.Value.Touch(hint);
        }

        public bool Release()
        {
            return this.Value.Release();
        }

        public bool Release(int decrement)
        {
            return this.Value.Release(decrement);
        }

        public int ReferenceCount => this.Value.ReferenceCount;
    }
}