namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Transport;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using NSubstitute;

    [TestClass]
    public class DeviceClientTests
    {
        /* Tests_SRS_DEVICECLIENT_28_002: [This property shall be defaulted to 240000 (4 minutes).] */
        [TestMethod]
        [TestCategory("DevClient")]
        public void DeviceClient_OperationTimeoutInMilliseconds_Property_DefaultValue()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;DeviceId=dumpy;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString);

            Assert.AreEqual((uint)(4 * 60 * 1000), deviceClient.OperationTimeoutInMilliseconds);
        }

        [TestMethod]
        [TestCategory("DeviceClient")]
        public void DeviceClient_OperationTimeoutInMilliseconds_Property_GetSet()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;DeviceId=dumpy;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
            deviceClient.OperationTimeoutInMilliseconds = 9999;

            Assert.AreEqual((uint)9999, deviceClient.OperationTimeoutInMilliseconds);
        }

        [TestMethod]
        [TestCategory("DeviceClient")]
        public async Task DeviceClient_OperationTimeoutInMilliseconds_Equals_0_Open()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;DeviceId=dumpy;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
            deviceClient.OperationTimeoutInMilliseconds = 0;

            var innerHandler = Substitute.For<IDelegatingHandler>();
            innerHandler.OpenAsync(Arg.Any<bool>(), Arg.Is<CancellationToken>(ct => ct.CanBeCanceled == false)).Returns(TaskConstants.Completed);
            deviceClient.InnerHandler = innerHandler;

            Task t = deviceClient.OpenAsync();

            await innerHandler.Received().OpenAsync(Arg.Any<bool>(), Arg.Is<CancellationToken>(ct => ct.CanBeCanceled == false));
        }

        [TestMethod]
        [TestCategory("DeviceClient")]
        public async Task DeviceClient_OperationTimeoutInMilliseconds_Equals_0_Receive()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;DeviceId=dumpy;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
            deviceClient.OperationTimeoutInMilliseconds = 0;

            var innerHandler = Substitute.For<IDelegatingHandler>();
            innerHandler.ReceiveAsync(Arg.Is<CancellationToken>(ct => ct.CanBeCanceled == false)).Returns(new Task<Message>(() => new Message()));
            deviceClient.InnerHandler = innerHandler;

            Task<Message> t = deviceClient.ReceiveAsync();
            
            await innerHandler.Received().ReceiveAsync(Arg.Is<CancellationToken>(ct => ct.CanBeCanceled == false));
        }

    }
}
