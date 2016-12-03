// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Common;
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Diagnostics.Contracts;
    
    /// <summary>
    /// Read-only wrapper for another generic dictionary.
    /// </summary>
    /// <typeparam name="TKey">Type to be used for keys.</typeparam>
    /// <typeparam name="TValue">Type to be used for values</typeparam>
#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
    [DebuggerDisplay("Count = {Count}")]
#if !WINDOWS_UWP && !PCL // WinRT types cannot be generic
    public
#endif
    class ReadOnlyDictionary45<TKey, TValue> : IDictionary<TKey, TValue>, IDictionary //, IReadOnlyDictionary<TKey, TValue>
    {
        readonly IDictionary<TKey, TValue> m_dictionary;
#if !WINDOWS_UWP && !PCL
        [NonSerialized]
#endif
        Object m_syncRoot;
#if !WINDOWS_UWP && !PCL
        [NonSerialized]
#endif
        KeyCollection m_keys;
#if !WINDOWS_UWP && !PCL
        [NonSerialized]
#endif
        ValueCollection m_values;
#if !WINDOWS_UWP && !PCL
        [NonSerialized]
#endif
        IReadOnlyIndicator m_readOnlyIndicator;

        public ReadOnlyDictionary45(IDictionary<TKey, TValue> dictionary)
            : this(dictionary, new AlwaysReadOnlyIndicator())
        {
        }

        internal ReadOnlyDictionary45(IDictionary<TKey, TValue> dictionary, IReadOnlyIndicator readOnlyIndicator)
        {
            if (dictionary == null)
            {
                throw new ArgumentNullException("dictionary");
            }
            Contract.EndContractBlock();
            m_dictionary = dictionary;
            m_readOnlyIndicator = readOnlyIndicator;
        }

        protected IDictionary<TKey, TValue> Dictionary
        {
            get { return m_dictionary; }
        }

        public KeyCollection Keys
        {
            get
            {
                Contract.Ensures(Contract.Result<KeyCollection>() != null);
                if (m_keys == null)
                {
                    m_keys = new KeyCollection(m_dictionary.Keys, this.m_readOnlyIndicator);
                }
                return m_keys;
            }
        }

        public ValueCollection Values
        {
            get
            {
                Contract.Ensures(Contract.Result<ValueCollection>() != null);
                if (m_values == null)
                {
                    m_values = new ValueCollection(m_dictionary.Values, this.m_readOnlyIndicator);
                }
                return m_values;
            }
        }

#region IDictionary<TKey, TValue> Members

        public bool ContainsKey(TKey key)
        {
            return m_dictionary.ContainsKey(key);
        }

        ICollection<TKey> IDictionary<TKey, TValue>.Keys
        {
            get
            {
                return Keys;
            }
        }

        public bool TryGetValue(TKey key, out TValue value)
        {
            return m_dictionary.TryGetValue(key, out value);
        }

        ICollection<TValue> IDictionary<TKey, TValue>.Values
        {
            get
            {
                return Values;
            }
        }

        public TValue this[TKey key]
        {
            get
            {
                return m_dictionary[key];
            }
        }

        void IDictionary<TKey, TValue>.Add(TKey key, TValue value)
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            m_dictionary.Add(key, value);
        }

        bool IDictionary<TKey, TValue>.Remove(TKey key)
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            return m_dictionary.Remove(key);
        }

        TValue IDictionary<TKey, TValue>.this[TKey key]
        {
            get
            {
                return m_dictionary[key];
            }
            set
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                m_dictionary[key] = value;
            }
        }

#endregion

#region ICollection<KeyValuePair<TKey, TValue>> Members

        public int Count
        {
            get { return m_dictionary.Count; }
        }

        bool ICollection<KeyValuePair<TKey, TValue>>.Contains(KeyValuePair<TKey, TValue> item)
        {
            return m_dictionary.Contains(item);
        }

        void ICollection<KeyValuePair<TKey, TValue>>.CopyTo(KeyValuePair<TKey, TValue>[] array, int arrayIndex)
        {
            m_dictionary.CopyTo(array, arrayIndex);
        }

        bool ICollection<KeyValuePair<TKey, TValue>>.IsReadOnly
        {
            get { return true; }
        }

        void ICollection<KeyValuePair<TKey, TValue>>.Add(KeyValuePair<TKey, TValue> item)
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            m_dictionary.Add(item);
        }

        void ICollection<KeyValuePair<TKey, TValue>>.Clear()
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            m_dictionary.Clear();
        }

        bool ICollection<KeyValuePair<TKey, TValue>>.Remove(KeyValuePair<TKey, TValue> item)
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            return m_dictionary.Remove(item);
        }

#endregion

#region IEnumerable<KeyValuePair<TKey, TValue>> Members

        public IEnumerator<KeyValuePair<TKey, TValue>> GetEnumerator()
        {
            return m_dictionary.GetEnumerator();
        }

#endregion

#region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return ((IEnumerable)m_dictionary).GetEnumerator();
        }

#endregion

#region IDictionary Members

        private static bool IsCompatibleKey(object key)
        {
            if (key == null)
            {
                throw Fx.Exception.ArgumentNull("key");
            }
            return key is TKey;
        }

        void IDictionary.Add(object key, object value)
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            m_dictionary.Add((TKey)key, (TValue)value);
        }

        void IDictionary.Clear()
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            m_dictionary.Clear();
        }

        bool IDictionary.Contains(object key)
        {
            return IsCompatibleKey(key) && ContainsKey((TKey)key);
        }

        IDictionaryEnumerator IDictionary.GetEnumerator()
        {
            IDictionary d = m_dictionary as IDictionary;
            if (d != null)
            {
                return d.GetEnumerator();
            }
            return new DictionaryEnumerator(m_dictionary);
        }

        bool IDictionary.IsFixedSize
        {
            get { return true; }
        }

        bool IDictionary.IsReadOnly
        {
            get { return true; }
        }

        ICollection IDictionary.Keys
        {
            get
            {
                return Keys;
            }
        }

        void IDictionary.Remove(object key)
        {
            if (this.m_readOnlyIndicator.IsReadOnly)
            {
                throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
            }

            m_dictionary.Remove((TKey)key);
        }

        ICollection IDictionary.Values
        {
            get
            {
                return Values;
            }
        }

        object IDictionary.this[object key]
        {
            get
            {
                if (IsCompatibleKey(key))
                {
                    return this[(TKey)key];
                }
                return null;
            }
            set
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                m_dictionary[(TKey)key] = (TValue)value;
            }
        }

        void ICollection.CopyTo(Array array, int index)
        {
            if (array == null)
            {
                Fx.Exception.ArgumentNull("array");
            }

            if (array.Rank != 1 || array.GetLowerBound(0) != 0)
            {
                throw Fx.Exception.Argument("array", Resources.InvalidBufferSize);
            }

            if (index < 0 || index > array.Length)
            {
                throw Fx.Exception.ArgumentOutOfRange("index", index, Resources.ValueMustBeNonNegative);
            }

            if (array.Length - index < Count)
            {
                throw Fx.Exception.Argument("array", Resources.InvalidBufferSize);
            }

            KeyValuePair<TKey, TValue>[] pairs = array as KeyValuePair<TKey, TValue>[];
            if (pairs != null)
            {
                m_dictionary.CopyTo(pairs, index);
            }
            else
            {
                DictionaryEntry[] dictEntryArray = array as DictionaryEntry[];
                if (dictEntryArray != null)
                {
                    foreach (var item in m_dictionary)
                    {
                        dictEntryArray[index++] = new DictionaryEntry(item.Key, item.Value);
                    }
                }
                else
                {
                    object[] objects = array as object[];
                    if (objects == null)
                    {
                        throw Fx.Exception.Argument("array", Resources.InvalidBufferSize);
                    }

                    try
                    {
                        foreach (var item in m_dictionary)
                        {
                            objects[index++] = new KeyValuePair<TKey, TValue>(item.Key, item.Value);
                        }
                    }
                    catch (ArrayTypeMismatchException)
                    {
                        throw Fx.Exception.Argument("array", Resources.InvalidBufferSize);
                    }
                }
            }
        }

        bool ICollection.IsSynchronized
        {
            get { return false; }
        }

        object ICollection.SyncRoot
        {
            get
            {
                if (m_syncRoot == null)
                {
                    ICollection c = m_dictionary as ICollection;
                    if (c != null)
                    {
                        m_syncRoot = c.SyncRoot;
                    }
                    else
                    {
                        System.Threading.Interlocked.CompareExchange<Object>(ref m_syncRoot, new Object(), null);
                    }
                }
                return m_syncRoot;
            }
        }
#if !WINDOWS_UWP && !PCL
        [Serializable]
#endif
        private struct DictionaryEnumerator : IDictionaryEnumerator
        {
            private readonly IDictionary<TKey, TValue> m_dictionary;
            private IEnumerator<KeyValuePair<TKey, TValue>> m_enumerator;

            public DictionaryEnumerator(IDictionary<TKey, TValue> dictionary)
            {
                m_dictionary = dictionary;
                m_enumerator = m_dictionary.GetEnumerator();
            }

            public DictionaryEntry Entry
            {
                get { return new DictionaryEntry(m_enumerator.Current.Key, m_enumerator.Current.Value); }
            }

            public object Key
            {
                get { return m_enumerator.Current.Key; }
            }

            public object Value
            {
                get { return m_enumerator.Current.Value; }
            }

            public object Current
            {
                get { return Entry; }
            }

            public bool MoveNext()
            {
                return m_enumerator.MoveNext();
            }

            public void Reset()
            {
                m_enumerator.Reset();
            }
        }

#endregion

        [DebuggerDisplay("Count = {Count}")]
#if !WINDOWS_UWP && !PCL
        [Serializable]
#endif
        public sealed class KeyCollection : ICollection<TKey>, ICollection
        {
            private readonly ICollection<TKey> m_collection;
#if !WINDOWS_UWP && !PCL
            [NonSerialized]
#endif
            private Object m_syncRoot;
#if !WINDOWS_UWP && !PCL
            [NonSerialized]
#endif
            private readonly IReadOnlyIndicator m_readOnlyIndicator;

            internal KeyCollection(ICollection<TKey> collection, IReadOnlyIndicator readOnlyIndicator)
            {
                if (collection == null)
                {
                    throw Fx.Exception.ArgumentNull("collection");
                }
                m_collection = collection;
                m_readOnlyIndicator = readOnlyIndicator;
            }

#region ICollection<T> Members

            void ICollection<TKey>.Add(TKey item)
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                m_collection.Add(item);
            }

            void ICollection<TKey>.Clear()
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                m_collection.Clear();
            }

            bool ICollection<TKey>.Contains(TKey item)
            {
                return m_collection.Contains(item);
            }

            public void CopyTo(TKey[] array, int arrayIndex)
            {
                m_collection.CopyTo(array, arrayIndex);
            }

            public int Count
            {
                get { return m_collection.Count; }
            }

            bool ICollection<TKey>.IsReadOnly
            {
                get { return true; }
            }

            bool ICollection<TKey>.Remove(TKey item)
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                return m_collection.Remove(item);
            }

#endregion

#region IEnumerable<T> Members

            public IEnumerator<TKey> GetEnumerator()
            {
                return m_collection.GetEnumerator();
            }

#endregion

#region IEnumerable Members

            System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
            {
                return ((IEnumerable)m_collection).GetEnumerator();
            }

#endregion

#region ICollection Members

            void ICollection.CopyTo(Array array, int index)
            {
                throw Fx.Exception.AsError(new NotImplementedException());
            }

            bool ICollection.IsSynchronized
            {
                get { return false; }
            }

            object ICollection.SyncRoot
            {
                get
                {
                    if (m_syncRoot == null)
                    {
                        ICollection c = m_collection as ICollection;
                        if (c != null)
                        {
                            m_syncRoot = c.SyncRoot;
                        }
                        else
                        {
                            System.Threading.Interlocked.CompareExchange<Object>(ref m_syncRoot, new Object(), null);
                        }
                    }
                    return m_syncRoot;
                }
            }

#endregion
        }

        [DebuggerDisplay("Count = {Count}")]
#if !WINDOWS_UWP && !PCL
        [Serializable]
#endif
        public sealed class ValueCollection : ICollection<TValue>, ICollection
        {
            private readonly ICollection<TValue> m_collection;
#if !WINDOWS_UWP && !PCL
            [NonSerialized]
#endif
            private Object m_syncRoot;
#if !WINDOWS_UWP && !PCL
            [NonSerialized]
#endif
            private readonly IReadOnlyIndicator m_readOnlyIndicator;

            internal ValueCollection(ICollection<TValue> collection, IReadOnlyIndicator readOnlyIndicator)
            {
                if (collection == null)
                {
                    throw Fx.Exception.ArgumentNull("collection");
                }

                m_collection = collection;
                m_readOnlyIndicator = readOnlyIndicator;
            }

#region ICollection<T> Members

            void ICollection<TValue>.Add(TValue item)
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                m_collection.Add(item);
            }

            void ICollection<TValue>.Clear()
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                m_collection.Clear();
            }

            bool ICollection<TValue>.Contains(TValue item)
            {
                return m_collection.Contains(item);
            }

            public void CopyTo(TValue[] array, int arrayIndex)
            {
                m_collection.CopyTo(array, arrayIndex);
            }

            public int Count
            {
                get { return m_collection.Count; }
            }

            bool ICollection<TValue>.IsReadOnly
            {
                get { return true; }
            }

            bool ICollection<TValue>.Remove(TValue item)
            {
                if (this.m_readOnlyIndicator.IsReadOnly)
                {
                    throw Fx.Exception.AsError(new NotSupportedException(Resources.ObjectIsReadOnly));
                }

                return m_collection.Remove(item);
            }

#endregion

#region IEnumerable<T> Members

            public IEnumerator<TValue> GetEnumerator()
            {
                return m_collection.GetEnumerator();
            }

#endregion

#region IEnumerable Members

            System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
            {
                return ((IEnumerable)m_collection).GetEnumerator();
            }

#endregion

#region ICollection Members

            void ICollection.CopyTo(Array array, int index)
            {
                throw Fx.Exception.AsError(new NotImplementedException());
            }

            bool ICollection.IsSynchronized
            {
                get { return false; }
            }

            object ICollection.SyncRoot
            {
                get
                {
                    if (m_syncRoot == null)
                    {
                        ICollection c = m_collection as ICollection;
                        if (c != null)
                        {
                            m_syncRoot = c.SyncRoot;
                        }
                        else
                        {
                            System.Threading.Interlocked.CompareExchange<Object>(ref m_syncRoot, new Object(), null);
                        }
                    }
                    return m_syncRoot;
                }
            }

#endregion ICollection Members
        }

        class AlwaysReadOnlyIndicator : IReadOnlyIndicator
        {
            public bool IsReadOnly
            {
                get { return true; }
            }
        }
    }
    
    public interface IReadOnlyIndicator
    {
        bool IsReadOnly { get; }
    }
}
