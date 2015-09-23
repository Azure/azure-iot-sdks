using System;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;

using Microsoft.Azure.Devices.Client;

namespace UWPTests
{
    [TestClass]
    public class MessageTest1
    {
        [TestMethod]
        public void SelfTest()
        {
            Assert.IsTrue(true);
        }

        [TestMethod]
        public void TestMessageCreation01()
        {
            new Message();
        }
    }
}
