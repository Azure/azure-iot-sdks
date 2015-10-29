// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    static class AmqpTrace
    {
        public static IAmqpTrace Provider = new EmptyAmqpTrace();

        class EmptyAmqpTrace : IAmqpTrace
        {
            public void AmqpOpenConnection(object source, object connection)
            {
                
            }

            public void AmqpCloseConnection(object source, object connection, bool abort)
            {
                
            }

            public void AmqpAddSession(object source, object session, ushort localChannel, ushort remoteChannel)
            {
            }

            public void AmqpAttachLink(object source, object link, uint localHandle, uint remoteHandle, string linkName, string role)
            {
            }

            public void AmqpDeliveryNotFound(object source, string deliveryTag)
            {
            }

            public void AmqpDispose(object source, uint deliveryId, bool settled, object state)
            {
            }

            public void AmqpDynamicBufferSizeChange(object source, string type, int oldSize, int newSize)
            {
            }

            public void AmqpInsecureTransport(object source, object transport, bool isSecure, bool isAuthenticated)
            {
            }

            public void AmqpLinkDetach(object source, string name, uint handle, string action, string error)
            {
            }

            public void AmqpListenSocketAcceptError(object source, bool willRetry, string error)
            {
            }

            public void AmqpLogError(object source, string operation, string message)
            {
            }

            public void AmqpLogOperationInformational(object source, TraceOperation operation, object detail)
            {
            }

            public void AmqpLogOperationVerbose(object source, TraceOperation operation, object detail)
            {
            }

            public void AmqpManageLink(string action, object link, string info)
            {
            }

            public void AmqpMissingHandle(object source, string type, uint handle)
            {
            }

            public void AmqpOpenEntityFailed(object source, object obj, string name, string entityName, string error)
            {
            }

            public void AmqpOpenEntitySucceeded(object source, object obj, string name, string entityName)
            {
            }

            public void AmqpReceiveMessage(object source, uint deliveryId, int transferCount)
            {
            }

            public void AmqpRemoveLink(object source, object link, uint localHandle, uint remoteHandle, string linkName)
            {
            }

            public void AmqpRemoveSession(object source, object session, ushort localChannel, ushort remoteChannel)
            {
            }

            public void AmqpSessionWindowClosed(object source, int nextId)
            {
            }

            public void AmqpSettle(object source, int settleCount, uint lwm, uint next)
            {
            }

            public void AmqpStateTransition(object source, string operation, object fromState, object toState)
            {
            }

            public void AmqpUpgradeTransport(object source, object from, object to)
            {
            }

            public void AmqpThrowingExceptionError(string exception)
            {
            }

            public void AmqpThrowingExceptionWarning(string exception)
            {
            }
        }
    }
}
