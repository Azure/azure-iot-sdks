// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Runtime.ExceptionServices;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Practices.EnterpriseLibrary.TransientFaultHandling;

    class RetryDelegatingHandler : DefaultDelegatingHandler
    {
        const int DefaultRetryCount = 15;
        const int UndeterminedPosition = -1;
        const string StopRetrying = "StopRetrying";

        internal static int RetryCount = DefaultRetryCount;

        class SendMessageState
        {
            public int Iteration { get; set; }

            public long InitialStreamPosition { get; set; }

            public ExceptionDispatchInfo OriginalError { get; set; }
        }

        class IotHubTransientErrorIgnoreStrategy : ITransientErrorDetectionStrategy
        {
            public bool IsTransient(Exception ex)
            {
                if (!(ex is IotHubClientTransientException))
                    return false;
                if (ex.Data[StopRetrying] == null)
                    return true;
                if ((bool)ex.Data[StopRetrying])
                {
                    ex.Data.Remove(StopRetrying);
                    return false;
                }
                return true;
            }
        }

        readonly RetryPolicy retryPolicy;
        public RetryDelegatingHandler(IDelegatingHandler innerHandler)
            :base(innerHandler)
        {
            this.retryPolicy = new RetryPolicy(new IotHubTransientErrorIgnoreStrategy(), RetryCount, TimeSpan.FromMilliseconds(100), TimeSpan.FromSeconds(10), TimeSpan.FromMilliseconds(100));
        }

        public override async Task SendEventAsync(Message message)
        {
            try
            {
                var sendState = new SendMessageState();
                await this.retryPolicy.ExecuteAsync(() => this.SendMessageWithRetryAsync(sendState, message, () => base.SendEventAsync(message)));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
            }
        }

        public override async Task SendEventAsync(IEnumerable<Message> messages)
        {
            try
            {
                var sendState = new SendMessageState();
                IEnumerable<Message> messageList = messages as IList<Message> ?? messages.ToList();
                await this.retryPolicy.ExecuteAsync(() => this.SendMessageWithRetryAsync(sendState, messageList, () => base.SendEventAsync(messageList)));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
            }
        }

        public override async Task<Message> ReceiveAsync()
        {
            try
            {
                return await this.retryPolicy.ExecuteAsync(() => base.ReceiveAsync());
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
                throw;
            }
        }

        public override async Task<Message> ReceiveAsync(TimeSpan timeout)
        {
            try
            {
                return await this.retryPolicy.ExecuteAsync(() => base.ReceiveAsync(timeout));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
                throw;
            }
        }

        public override async Task CompleteAsync(string lockToken)
        {
            try
            {
                await this.retryPolicy.ExecuteAsync(() => base.CompleteAsync(lockToken));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
            }
        }

        public override async Task AbandonAsync(string lockToken)
        {
            try
            {
                await this.retryPolicy.ExecuteAsync(() => base.AbandonAsync(lockToken));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
            }
        }

        public override async Task RejectAsync(string lockToken)
        {
            try
            {
                await this.retryPolicy.ExecuteAsync(() => base.RejectAsync(lockToken));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
            }
        }

        public override async Task OpenAsync(bool explicitOpen)
        {
            try
            {
                await this.retryPolicy.ExecuteAsync(() => base.OpenAsync(explicitOpen));
            }
            catch (IotHubClientTransientException ex)
            {
                GetNormalizedIotHubException(ex).Throw();
            }
        }

        async Task SendMessageWithRetryAsync(SendMessageState sendState, IEnumerable<Message> messages, Func<Task> action)
        {
            if (sendState.Iteration == 0)
            {
                foreach (Message message in messages)
                {
                    long messageStreamPosition = message.BodyStream.CanSeek ? message.BodyStream.Position : UndeterminedPosition;
                    if (messageStreamPosition == UndeterminedPosition || sendState.InitialStreamPosition == UndeterminedPosition)
                    {
                        sendState.InitialStreamPosition = UndeterminedPosition;
                    }
                    else
                    {
                        sendState.InitialStreamPosition = messageStreamPosition > sendState.InitialStreamPosition ? messageStreamPosition : sendState.InitialStreamPosition;
                    }
                    message.TryResetBody(messageStreamPosition);
                }

                await TryExecuteActionAsync(sendState, action);
                return;
            }

            EnsureStreamsAreInOriginalState(sendState, messages);

            await TryExecuteActionAsync(sendState, action);
        }

        async Task SendMessageWithRetryAsync(SendMessageState sendState, Message message, Func<Task> action)
        {
            if (sendState.Iteration == 0)
            {
                sendState.InitialStreamPosition = message.BodyStream.CanSeek ? message.BodyStream.Position : UndeterminedPosition;
                message.TryResetBody(sendState.InitialStreamPosition);

                await TryExecuteActionAsync(sendState, action);
                return;
            }

            EnsureStreamIsInOriginalState(sendState, message);

            await TryExecuteActionAsync(sendState, action);
        }

        static void EnsureStreamsAreInOriginalState(SendMessageState sendState, IEnumerable<Message> messages)
        {
            IEnumerable<Message> messageList = messages as IList<Message> ?? messages.ToList();

            //We do not retry if:
            //1. any message was attempted to read the body stream and the stream is not seekable; 
            //2. any message has initial stream position different from zero.

            if (sendState.InitialStreamPosition != 0)
            {
                sendState.OriginalError.SourceException.Data[StopRetrying] = true;
                sendState.OriginalError.Throw();
            }

            foreach (Message message in messageList)
            {               
                if (!message.IsBodyCalled || message.TryResetBody(0))
                {
                    continue;
                }

                sendState.OriginalError.SourceException.Data[StopRetrying] = true;
                sendState.OriginalError.Throw();
            }
        }

        static void EnsureStreamIsInOriginalState(SendMessageState sendState, Message message)
        {
            //We do not retry if a message was attempted to read the body stream and the stream is not seekable;             
            if (!message.IsBodyCalled || message.TryResetBody(sendState.InitialStreamPosition))
            {
                return;
            }

            sendState.OriginalError.SourceException.Data[StopRetrying] = true;
            sendState.OriginalError.Throw();
        }

        static async Task TryExecuteActionAsync(SendMessageState sendState, Func<Task> action)
        {
            sendState.Iteration++;
            try
            {
                await action();
            }
            catch (IotHubClientTransientException ex)
            {
                sendState.OriginalError = ExceptionDispatchInfo.Capture(ex);
                throw;
            }
        }

        static ExceptionDispatchInfo GetNormalizedIotHubException(IotHubClientTransientException ex)
        {
            if (ex.InnerException != null)
            {
                return ExceptionDispatchInfo.Capture(ex.InnerException);
            }
            return ExceptionDispatchInfo.Capture(ex);
        }
    }
}