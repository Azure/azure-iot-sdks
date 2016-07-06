// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Tests.UWPTests
{
    using System;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.Azure.Devices.Client.Test;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class UtilsTests
    {
        [TestMethod]
        public void ConvertDeliveryAckTypeFromString_ValidString_Pass()
        {
            Assert.AreEqual(DeliveryAcknowledgement.PositiveOnly, Utils.ConvertDeliveryAckTypeFromString("positive"));
        }

        [TestMethod]
        public void ConvertDeliveryAckTypeFromString_InvalidString_Fail()
        {
            Action action = () => Utils.ConvertDeliveryAckTypeFromString("unknown");
            TestAssert.Throws<NotSupportedException>(action);
        }

        [TestMethod]
        public void ConvertDeliveryAckTypeToString_ValidValue_Pass()
        {
            Assert.AreEqual("negative", Utils.ConvertDeliveryAckTypeToString(DeliveryAcknowledgement.NegativeOnly));
        }

        [TestMethod]
        public void ConvertDeliveryAckTypeToString_InvalidValue_Fail()
        {
            Action action = () => Utils.ConvertDeliveryAckTypeToString((DeliveryAcknowledgement)100500);
            TestAssert.Throws<NotSupportedException>(action);
        }
    }
}
