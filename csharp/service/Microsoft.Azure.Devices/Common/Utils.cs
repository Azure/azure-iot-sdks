// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Diagnostics;
    using System.IO;
    using System.Threading;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    public static class Utils
    {
        public static bool IsValidBase64(string input, out int lengthInBytes)
        {
            lengthInBytes = 0;
            try
            {
                lengthInBytes = Convert.FromBase64String(input).Length;
                return true;
            }
            catch (FormatException)
            {
                return false;
            }
        }

        public static string JsonSerialize(object obj)
        {
            return JsonConvert.SerializeObject(
                obj,
                Formatting.Indented,
                new JsonSerializerSettings());
        }

        public static T JsonDeserialize<T>(string serializedString)
        {
            return JsonConvert.DeserializeObject<T>(
                serializedString,
                new JsonSerializerSettings());
        }

        public static JsonTextWriter CreateJsonWriter(Stream stream)
        {
            return new JsonTextWriter(new StreamWriter(stream));
        }

        public static void JsonBufferedSerializeBegin(JsonTextWriter writer)
        {
            writer.WriteStartArray();
        }

        public static void JsonBufferedSerializeAppend(JsonTextWriter writer, object obj)
        {
            var serializer = new JsonSerializer();
            var jObj = JObject.FromObject(obj, serializer);
            jObj.WriteTo(writer);
        }

        public static void JsonBufferedSerializeEnd(JsonTextWriter writer)
        { 
            writer.WriteEndArray();
            writer.Flush();
        }

        public static JsonTextReader CreateJsonReader(Stream stream)
        {
            return new JsonTextReader(new StreamReader(stream));
        }

        public static T JsonBufferedDeserializeNext<T>(JsonTextReader reader)
        {
            T toReturn = default(T);
            while (reader.Read())
            {
                if (reader.TokenType == JsonToken.StartObject)
                {
                    var obj = JObject.Load(reader);
                    toReturn = obj.ToObject<T>();
                    break;
                }
            }

            return toReturn;
        }

        public static void StartProcessMonitor(Action<string> traceEventAction)
        {
            while (true)
            {
                using (Process currentProcess = Process.GetCurrentProcess())
                {
                    int currentThreadCount = currentProcess.Threads.Count;
                    if (currentThreadCount > CommonConstants.MaxThreadsCountThreshold)
                    {
                        string errorMessage = "Thread count limit exceeded. Current count {0}, Threshold {1}.".FormatInvariant(currentThreadCount, CommonConstants.MaxThreadsCountThreshold);
                        traceEventAction(errorMessage);
                        Fx.AssertAndFailFastService(errorMessage);
                    }
                }

                Thread.Sleep(CommonConstants.ProcessThreadCheckInterval);
            }
        }

        public static void ValidateBufferBounds(byte[] buffer, int offset, int size)
        {
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }

            ValidateBufferBounds(buffer.Length, offset, size);
        }

        static void ValidateBufferBounds(int bufferSize, int offset, int size)
        {
            if (offset < 0)
            {
                throw new ArgumentOutOfRangeException("offset", offset, ApiResources.ArgumentMustBeNonNegative);
            }

            if (offset > bufferSize)
            {
                throw new ArgumentOutOfRangeException("offset", offset, ApiResources.OffsetExceedsBufferSize.FormatInvariant(bufferSize));
            }

            if (size <= 0)
            {
                throw new ArgumentOutOfRangeException("size", size, ApiResources.ArgumentMustBePositive);
            }

            int remainingBufferSpace = bufferSize - offset;
            if (size > remainingBufferSpace)
            {
                throw new ArgumentOutOfRangeException("size", size, ApiResources.SizeExceedsRemainingBufferSpace.FormatInvariant(remainingBufferSpace));
            }
        }
    }
}
