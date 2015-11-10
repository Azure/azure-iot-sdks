// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    interface IAmqpTrace
    {
        void AmqpOpenConnection(object source, object connection);

        void AmqpCloseConnection(object source, object connection, bool abort);

        void AmqpAddSession(object source, object session, ushort localChannel, ushort remoteChannel);

        void AmqpAttachLink(object source, object link, uint localHandle, uint remoteHandle, string linkName, string role);
        
        void AmqpDeliveryNotFound(object source, string deliveryTag);
        
        void AmqpDispose(object source, uint deliveryId, bool settled, object state);
        
        void AmqpDynamicBufferSizeChange(object source, string type, int oldSize, int newSize);
        
        void AmqpInsecureTransport(object source, object transport, bool isSecure, bool isAuthenticated);
        
        void AmqpLinkDetach(object source, string name, uint handle, string action, string error);
        
        void AmqpListenSocketAcceptError(object source, bool willRetry, string error);
        
        void AmqpLogError(object source, string operation, string message);
        
        void AmqpLogOperationInformational(object source, TraceOperation operation, object detail);
        
        void AmqpLogOperationVerbose(object source, TraceOperation operation, object detail);
        
        void AmqpManageLink(string action, object link, string info);
        
        void AmqpMissingHandle(object source, string type, uint handle);
        
        void AmqpOpenEntityFailed(object source, object obj, string name, string entityName, string error);
        
        void AmqpOpenEntitySucceeded(object source, object obj, string name, string entityName);
        
        void AmqpReceiveMessage(object source, uint deliveryId, int transferCount);
        
        void AmqpRemoveLink(object source, object link, uint localHandle, uint remoteHandle, string linkName);
        
        void AmqpRemoveSession(object source, object session, ushort localChannel, ushort remoteChannel);
        
        void AmqpSessionWindowClosed(object source, int nextId);
        
        void AmqpSettle(object source, int settleCount, uint lwm, uint next);
        
        void AmqpStateTransition(object source, string operation, object fromState, object toState);
        
        void AmqpUpgradeTransport(object source, object from, object to);
        
        void AmqpThrowingExceptionError(string exception);

        void AmqpThrowingExceptionWarning(string exception);
    }
}
