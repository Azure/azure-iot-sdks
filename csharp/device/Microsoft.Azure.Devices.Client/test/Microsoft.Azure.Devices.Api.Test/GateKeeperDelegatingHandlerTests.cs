// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;
    using DotNetty.Common.Concurrency;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Transport;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using NSubstitute;

    [TestClass]
    public class GateKeeperDelegatingHandlerTests
    {
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task OpenAsync_InnerCompleted_SutIsOpen()
        {
            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(true, Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);

            var cancellationToken = new CancellationToken();
            var sut = new GateKeeperDelegatingHandler(innerHandlerMock);
            await sut.OpenAsync(cancellationToken);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task ImplicitOpen_SubjWasNotOpen_SubjIsOpen()
        {
            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(false, Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);
            innerHandlerMock.SendEventAsync(Arg.Any<Message>(), Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);
            innerHandlerMock.SendEventAsync(Arg.Any<IEnumerable<Message>>(), Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);
            innerHandlerMock.ReceiveAsync(Arg.Any<CancellationToken>()).Returns(t => Task.FromResult(new Message()));
            innerHandlerMock.ReceiveAsync(Arg.Any<TimeSpan>(), Arg.Any<CancellationToken>()).Returns(t => Task.FromResult(new Message()));
            innerHandlerMock.AbandonAsync(Arg.Any<string>(), Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);
            innerHandlerMock.CompleteAsync(Arg.Any<string>(), Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);
            innerHandlerMock.RejectAsync(Arg.Any<string>(), Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);

            var cancellationToken = new CancellationToken();
            var actions = new Func<IDelegatingHandler, Task>[]
            {
                sut => sut.SendEventAsync(new Message(), cancellationToken),
                sut => sut.SendEventAsync(new[] { new Message() }, cancellationToken),
                sut => sut.ReceiveAsync(cancellationToken),
                sut => sut.ReceiveAsync(TimeSpan.FromSeconds(1), cancellationToken),
                sut => sut.AbandonAsync(string.Empty, cancellationToken),
                sut => sut.CompleteAsync(string.Empty, cancellationToken),
                sut => sut.RejectAsync(string.Empty, cancellationToken),
            };

            foreach (Func<IDelegatingHandler, Task> action in actions)
            {
                var sut = new GateKeeperDelegatingHandler(innerHandlerMock);
                await action(sut);
            }

            await innerHandlerMock.Received(actions.Length).OpenAsync(false, Arg.Any<CancellationToken>());
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task OpenAsync_ClosedCannotBeReopened_Throws()
        {
            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.CloseAsync().Returns(t => TaskConstants.Completed);

            var sut = new GateKeeperDelegatingHandler(innerHandlerMock);
            await sut.CloseAsync();

            var cancellationToken = new CancellationToken();
            await ((Func<Task>)(() => sut.OpenAsync(cancellationToken))).ExpectedAsync<ObjectDisposedException>();
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task OpenAsync_TwoCallers_OnlyOneOpenCalled()
        {
            var tcs = new TaskCompletionSource();
            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            int callCounter = 0;
            innerHandlerMock.OpenAsync(true, Arg.Any<CancellationToken>()).Returns(t =>
            {
                callCounter++;
                return tcs.Task;
            });

            var sut = new GateKeeperDelegatingHandler(innerHandlerMock);
            var cancellationToken = new CancellationToken();
            Task firstOpen = sut.OpenAsync(cancellationToken);
            Task secondOpen = sut.OpenAsync(cancellationToken);
            tcs.Complete();
            await Task.WhenAll(firstOpen, secondOpen);

            Assert.AreEqual(1, callCounter);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task OpenAsync_InnerFailed_SutIsOpenAndCanBeReopen()
        {
            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(Arg.Is(true), Arg.Any<CancellationToken>()).Returns(ci =>
            {
                throw new IOException();
            });
            var sut = new GateKeeperDelegatingHandler(innerHandlerMock);
            var cancellationToken = new CancellationToken();
            await ((Func<Task>)(() => sut.OpenAsync(cancellationToken))).ExpectedAsync<IOException>();

            innerHandlerMock.OpenAsync(Arg.Is(true), Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);

            await sut.OpenAsync(cancellationToken);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task OpenAsync_InnerCancelled_SutIsOpenAndCanBeReopen()
        {
            var tcs = new TaskCompletionSource();
            tcs.SetCanceled();
            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(true, Arg.Any<CancellationToken>()).Returns(tcs.Task);
            var sut = new GateKeeperDelegatingHandler(innerHandlerMock);
            var cancellationToken = new CancellationToken();
            await sut.OpenAsync(cancellationToken).ExpectedAsync<TaskCanceledException>();

            innerHandlerMock.OpenAsync(true, Arg.Any<CancellationToken>()).Returns(t => TaskConstants.Completed);

            await sut.OpenAsync(cancellationToken);
        }
    }
}