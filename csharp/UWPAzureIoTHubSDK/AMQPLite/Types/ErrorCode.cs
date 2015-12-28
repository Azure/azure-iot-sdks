//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace Amqp.Types
{
    /// <summary>
    /// Defines the conditions of AMQP errors.
    /// </summary>
    public static class ErrorCode
    {
        // amqp errors

        /// <summary>
        /// An internal error occurred. Operator intervention may be required to resume
        /// normal operation.
        /// </summary>
        public const string InternalError = "amqp:internal-error";

        /// <summary>
        /// A peer attempted to work with a remote entity that does not exist.
        /// </summary>
        public const string NotFound = "amqp:not-found";

        /// <summary>
        /// A peer attempted to work with a remote entity to which it has no access
        /// due to security settings.
        /// </summary>
        public const string UnauthorizedAccess = "amqp:unauthorized-access";

        /// <summary>
        /// Data could not be decoded.
        /// </summary>
        public const string DecodeError = "amqp:decode-error";

        /// <summary>
        /// Data could not be decoded.
        /// </summary>
        public const string ResourceLimitExceeded = "amqp:resource-limit-exceeded";

        /// <summary>
        /// The peer tried to use a frame in a manner that is inconsistent with
        /// the semantics defined in the specification.
        /// </summary>
        public const string NotAllowed = "amqp:not-allowed";

        /// <summary>
        /// An invalid field was passed in a frame body, and the operation could not proceed.
        /// </summary>
        public const string InvalidField = "amqp:invalid-field";

        /// <summary>
        /// The peer tried to use functionality that is not implemented in its partner.
        /// </summary>
        public const string NotImplemented = "amqp:not-implemented";

        /// <summary>
        /// The client attempted to work with a server entity to which it has no access
        /// because another client is working with it.
        /// </summary>
        public const string ResourceLocked = "amqp:resource-locked";

        /// <summary>
        /// The client made a request that was not allowed because some precondition failed.
        /// </summary>
        public const string PreconditionFailed = "amqp:precondition-failed";

        /// <summary>
        /// A server entity the client is working with has been deleted.
        /// </summary>
        public const string ResourceDeleted = "amqp:resource-deleted";

        /// <summary>
        /// The peer sent a frame that is not permitted in the current state of the Session.
        /// </summary>
        public const string IllegalState = "amqp:illegal-state";

        /// <summary>
        /// The peer cannot send a frame because the smallest encoding of the performative
        /// with the currently valid values would be too large to fit within a frame of
        /// the agreed maximum frame size. 
        /// </summary>
        public const string FrameSizeTooSmall = "amqp:frame-size-too-small";

        // connection errors

        /// <summary>
        /// An operator intervened to close the Connection for some reason.
        /// The client may retry at some later date.
        /// </summary>
        public const string ConnectionForced = "amqp:connection:forced";

        /// <summary>
        /// A valid frame header cannot be formed from the incoming byte stream.
        /// </summary>
        public const string FramingError = "amqp:connection:framing-error";

        /// <summary>
        /// The container is no longer available on the current connection. 
        /// </summary>
        public const string ConnectionRedirect = "amqp:connection:redirect";
        
        // session errors

        /// <summary>
        /// The peer violated incoming window for the session.
        /// </summary>
        public const string WindowViolation = "amqp:session:window-violation";

        /// <summary>
        /// Input was received for a link that was detached with an error.
        /// </summary>
        public const string ErrantLink = "amqp:session-errant-link";

        /// <summary>
        /// An attach was received using a handle that is already in use for an attached Link.
        /// </summary>
        public const string HandleInUse = "amqp:session:handle-in-use";

        /// <summary>
        /// A frame (other than attach) was received referencing a handle which is not currently
        /// in use of an attached Link.
        /// </summary>
        public const string UnattachedHandle = "amqp:session:unattached-handle";
        
        // link errors

        /// <summary>
        /// An operator intervened to detach for some reason.
        /// </summary>
        public const string DetachForced = "amqp:link:detach-forced";

        /// <summary>
        /// The peer sent more Message transfers than currently allowed on the link.
        /// </summary>
        public const string TransferLimitExceeded = "amqp:link:transfer-limit-exceeded";

        /// <summary>
        /// The peer sent a larger message than is supported on the link.
        /// </summary>
        public const string MessageSizeExceeded = "amqp:link:message-size-exceeded";

        /// <summary>
        /// The address provided cannot be resolved to a terminus at the current container.
        /// </summary>
        public const string LinkRedirect = "amqp:link:redirect";

        /// <summary>
        /// The link has been attached elsewhere, causing the existing attachment
        /// to be forcibly closed.
        /// </summary>
        public const string Stolen = "amqp:link:stolen";

        // tx error conditions

        /// <summary>
        /// The specified txn-id does not exist.
        /// </summary>
        public const string TransactionUnknownId = "amqp:transaction:unknown-id";

        /// <summary>
        /// The transaction was rolled back for an unspecified reason.
        /// </summary>
        public const string TransactionRollback = "amqp:transaction:rollback";

        /// <summary>
        /// The work represented by this transaction took too long.
        /// </summary>
        public const string TransactionTimeout = "amqp:transaction:timeout";

        // messaging

        /// <summary>
        /// The message has been released by the peer.
        /// </summary>
        public const string MessageReleased = "amqp:message:released";
    }
}
