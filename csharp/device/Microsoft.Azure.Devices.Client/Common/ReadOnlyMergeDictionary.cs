// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Common
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;

    sealed class ReadOnlyMergeDictionary<TKey, TValue> : IDictionary<TKey, TValue>, IList<KeyValuePair<TKey, TValue>>
    {
        readonly IDictionary<TKey, TValue> mainDictionary;
        readonly IDictionary<TKey, TValue> secondaryDictionary;

        public ReadOnlyMergeDictionary(IDictionary<TKey, TValue> mainDictionary, IDictionary<TKey, TValue> secondaryDictionary)
        {
            Contract.Requires(mainDictionary != null);
            Contract.Requires(secondaryDictionary != null);

            this.mainDictionary = mainDictionary;
            this.secondaryDictionary = secondaryDictionary;
        }

        IEnumerator<KeyValuePair<TKey, TValue>> IEnumerable<KeyValuePair<TKey, TValue>>.GetEnumerator()
        {
            foreach (KeyValuePair<TKey, TValue> pair in this.mainDictionary)
            {
                yield return pair;
            }

            foreach (KeyValuePair<TKey, TValue> pair in this.secondaryDictionary)
            {
                if (!this.mainDictionary.ContainsKey(pair.Key))
                {
                    yield return pair;
                }
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable<KeyValuePair<TKey, TValue>>)this).GetEnumerator();
        }

        void ICollection<KeyValuePair<TKey, TValue>>.Add(KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException();
        }

        void ICollection<KeyValuePair<TKey, TValue>>.Clear()
        {
            throw new NotSupportedException();
        }

        bool ICollection<KeyValuePair<TKey, TValue>>.Contains(KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException();
        }

        void ICollection<KeyValuePair<TKey, TValue>>.CopyTo(KeyValuePair<TKey, TValue>[] array, int arrayIndex)
        {
            throw new NotSupportedException();
        }

        bool ICollection<KeyValuePair<TKey, TValue>>.Remove(KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException();
        }

        int ICollection<KeyValuePair<TKey, TValue>>.Count
        {
            get { throw new NotSupportedException(); }
        }

        bool ICollection<KeyValuePair<TKey, TValue>>.IsReadOnly
        {
            get { return true; }
        }

        bool IDictionary<TKey, TValue>.ContainsKey(TKey key)
        {
            if (this.mainDictionary.ContainsKey(key))
            {
                return true;
            }
            return this.secondaryDictionary.ContainsKey(key);
        }

        void IDictionary<TKey, TValue>.Add(TKey key, TValue value)
        {
            throw new NotSupportedException();
        }

        bool IDictionary<TKey, TValue>.Remove(TKey key)
        {
            throw new NotSupportedException();
        }

        public bool TryGetValue(TKey key, out TValue value)
        {
            if (this.mainDictionary.TryGetValue(key, out value))
            {
                return true;
            }
            return this.secondaryDictionary.TryGetValue(key, out value);
        }

        TValue IDictionary<TKey, TValue>.this[TKey key]
        {
            get
            {
                TValue value;
                if (!this.TryGetValue(key, out value))
                {
                    throw new ArgumentException("Specified key was not found in the dictionary.", "key");
                }
                return value;
            }
            set { throw new NotSupportedException(); }
        }

        ICollection<TKey> IDictionary<TKey, TValue>.Keys
        {
            get { throw new NotSupportedException(); }
        }

        ICollection<TValue> IDictionary<TKey, TValue>.Values
        {
            get { throw new NotSupportedException(); }
        }

        public int IndexOf(KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException();
        }

        public void Insert(int index, KeyValuePair<TKey, TValue> item)
        {
            throw new NotSupportedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotSupportedException();
        }

        public KeyValuePair<TKey, TValue> this[int index]
        {
            get { throw new NotSupportedException(); }
            set { throw new NotSupportedException(); }
        }
    }
}