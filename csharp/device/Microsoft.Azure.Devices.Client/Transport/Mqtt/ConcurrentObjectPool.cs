// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class ConcurrentObjectPool<TKey, TValue> where TValue : class
    {
        class Slot
        {
            public Dictionary<TKey, int> References { get; set; }

            public DateTime LastAccessedTime { get; set; }

            public TValue Value { get; set; }
        }
        
        readonly TimeSpan keepAliveTimeout;
        readonly Func<TValue, Task> disposeCallback;
        readonly int poolSize;
        readonly Func<TValue> objectFactory;
        readonly Slot[] slots;

        int position;
        readonly object syncRoot = new object();
        Timer cleanupTimer;

        public ConcurrentObjectPool(int poolSize, Func<TValue> objectFactory, TimeSpan keepAliveTimeout, Func<TValue, Task> disposeCallback)
            : this(poolSize, objectFactory, keepAliveTimeout, disposeCallback, null)
        {
        }

        public ConcurrentObjectPool(int poolSize, Func<TValue> objectFactory, TimeSpan keepAliveTimeout, Func<TValue, Task> disposeCallback, IEqualityComparer<TKey> keyComparer)
        {
            this.poolSize = poolSize;
            this.objectFactory = objectFactory;
            this.keepAliveTimeout = keepAliveTimeout;
            this.disposeCallback = disposeCallback;
            this.slots = new Slot[poolSize];
            for (int i = 0; i < poolSize; i++)
            {
                this.slots[i] = new Slot
                {
                    References = new Dictionary<TKey, int>(keyComparer)
                };
            }
            this.cleanupTimer = new Timer(this.Cleanup, null, this.keepAliveTimeout, this.keepAliveTimeout);
        }

        public TValue TakeOrAdd(TKey key)
        {
            lock (this.syncRoot)
            {
                Slot slot = this.slots.FirstOrDefault(s => s.References.ContainsKey(key));

                if (slot == null)
                {
                    slot = this.slots[this.position++ % this.poolSize];
                    if (slot.Value == null)
                    {
                        slot.Value = this.objectFactory();
                    }
                    slot.References.Add(key, 1);
                }
                else
                {
                    slot.References[key]++;
                }
                slot.LastAccessedTime = DateTime.UtcNow;
                return slot.Value;
            }
        }

        public bool Release(TKey key)
        {
            lock (this.syncRoot)
            {
                foreach (Slot slot in this.slots)
                {
                    int refCount; 
                    if (!slot.References.TryGetValue(key, out refCount))
                    {
                        continue;
                    }
                    refCount--;
                    if (refCount == 0)
                    {
                        slot.References.Remove(key);
                    }
                    slot.References[key] = refCount;
                    break;
                }
            }
            return true;
        }

        void Cleanup(object state)
        {
            var objectsToCleanup = new List<TValue>();
            lock (this.syncRoot)
            {
                for (int i = 0; i < poolSize; i++)
                {
                    Slot slot = this.slots[i];
                    if (slot.Value != null && slot.References.Count == 0 && slot.LastAccessedTime < DateTime.UtcNow - this.keepAliveTimeout)
                    {
                        objectsToCleanup.Add(slot.Value);
                        slot.Value = null;
                    }
                }
            }
            foreach (TValue obj in objectsToCleanup)
            {
                this.ScheduleDispose(obj);
            }
        }

        async void ScheduleDispose(TValue obj)
        {
            try
            {
                await this.disposeCallback(obj);
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
            }
        }
    }
}