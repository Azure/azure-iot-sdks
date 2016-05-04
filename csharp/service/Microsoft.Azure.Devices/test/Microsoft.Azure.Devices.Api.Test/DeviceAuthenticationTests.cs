// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;

    [TestClass]
    public class DeviceAuthenticationTests
    {
        const string IotHubName = "acme";

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        public async Task DeviceAuthenticationGoodAuthConfigTest1()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = new X509Thumbprint()
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        public async Task DeviceAuthenticationGoodAuthConfigTest2()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        public async Task DeviceAuthenticationGoodAuthConfigTest3()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = null
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        public async Task DeviceAuthenticationGoodAuthConfigTest4()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = null,
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        public async Task DeviceAuthenticationGoodAuthConfigTest5()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = null
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        public async Task DeviceAuthenticationGoodAuthConfigTest6()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = "781BC9694ADEB8929D4F7FE4B9A3A6DE58B07952"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof (ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest1()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest2()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = null,
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest3()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = null
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest4()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = null
                    },
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest5()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = null,
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest6()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = null,
                        SecondaryKey = CryptoKeyGenerator.GenerateKey(32)
                    },
                    X509Thumbprint = null
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadAuthConfigTest7()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = new SymmetricKey()
                    {
                        PrimaryKey = CryptoKeyGenerator.GenerateKey(32),
                        SecondaryKey = null
                    },
                    X509Thumbprint = null
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest1()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = null,
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B079"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest2()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F74B9A3A6DE58B0790B",
                        SecondaryThumbprint = null
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest3()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F74B9A3A",
                        SecondaryThumbprint = "8929D4F74B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest4()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F74B9A3A",
                        SecondaryThumbprint = null
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest5()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = null,
                        SecondaryThumbprint = "8929D4F74B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest6()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B",
                        SecondaryThumbprint = "8929D4F74B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("Auth")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeviceAuthenticationBadThumbprintTest7()
        {
            var deviceBadAuthConfig = new Device("123")
            {
                ConnectionState = DeviceConnectionState.Connected,
                Authentication = new AuthenticationMechanism()
                {
                    SymmetricKey = null,
                    X509Thumbprint = new X509Thumbprint()
                    {
                        PrimaryThumbprint = "921BC9694ADEB8929D4F74B9A3A",
                        SecondaryThumbprint = "921BC9694ADEB8929D4F7FE4B9A3A6DE58B0790B"
                    }
                }
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(
                restOp =>
                    restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(),
                        It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                        It.IsAny<CancellationToken>())).ReturnsAsync(deviceBadAuthConfig);
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceBadAuthConfig);
        }
    }
}
