// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test.JobClient
{
    using System;
    using System.Collections.Generic;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class UpdateDeviceConfigurationJobParametersTests
    {
        [TestMethod]
        public void ConstructorTest()
        {
            new UpdateDeviceConfigurationJobParameters("value", "deviceId");
        }

        [TestMethod]
        public void ConstructorWithEmptyValueTest()
        {
            new UpdateDeviceConfigurationJobParameters("", "deviceId");
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void ConstructorWithNullValueTest()
        {
            new UpdateDeviceConfigurationJobParameters(null, "deviceId");
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentException))]
        public void ConstructorWithNullDeviceIdTest()
        {
            string value = null;
            new UpdateDeviceConfigurationJobParameters("value", value);
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentException))]
        public void ConstructorWithNullDeviceIdsTest()
        {
            List<string> values = null;
            new UpdateDeviceConfigurationJobParameters("value", values);
        }

        [TestMethod]
        [ExpectedException(typeof(ArgumentException))]
        public void ConstructorWithEmptyDeviceIdTest()
        {
            new UpdateDeviceConfigurationJobParameters("value", "");
        }
    }
}