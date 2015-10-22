// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.-

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Threading;

    [Fx.Tag.SynchronizationPrimitive(Fx.Tag.BlocksUsing.NonBlocking)]
    //TODO: 171524 - Across remoting boundary Serializable is not sufficient, and requires AsyncResult that derives from MarshalByRefObject.  
    [Serializable]
    class SignalGate
    {
        [Fx.Tag.SynchronizationObject(Blocking = false, Kind = Fx.Tag.SynchronizationKind.InterlockedNoSpin)]
        int state;

        public SignalGate()
        {
        }

        internal bool IsLocked
        {
            get
            {
                return this.state == GateState.Locked;
            }
        }

        internal bool IsSignalled
        {
            get
            {
                return this.state == GateState.Signalled;
            }
        }

        // Returns true if this brings the gate to the Signalled state.
        // Transitions - Locked -> SignalPending | Completed before it was unlocked
        //               Unlocked -> Signaled
        public bool Signal()
        {
            int lastState = this.state;
            if (lastState == GateState.Locked)
            {
                lastState = Interlocked.CompareExchange(ref this.state, GateState.SignalPending, GateState.Locked);
            }
            if (lastState == GateState.Unlocked)
            {
                this.state = GateState.Signalled;
                return true;
            }

            if (lastState != GateState.Locked)
            {
                ThrowInvalidSignalGateState();
            }
            return false;
        }

        // Returns true if this brings the gate to the Signalled state.
        // Transitions - SignalPending -> Signaled | return the AsyncResult since the callback already 
        //                                         | completed and provided the result on its thread
        //               Locked -> Unlocked
        public bool Unlock()
        {
            int lastState = this.state;
            if (lastState == GateState.Locked)
            {
                lastState = Interlocked.CompareExchange(ref this.state, GateState.Unlocked, GateState.Locked);
            }
            if (lastState == GateState.SignalPending)
            {
                this.state = GateState.Signalled;
                return true;
            }

            if (lastState != GateState.Locked)
            {
                ThrowInvalidSignalGateState();
            }
            return false;
        }

        // This is factored out to allow Signal and Unlock to be inlined.
        static void ThrowInvalidSignalGateState()
        {
            throw Fx.Exception.AsError(new InvalidOperationException(CommonResources.InvalidSemaphoreExit));
        }

        static class GateState
        {
            public const int Locked = 0;
            public const int SignalPending = 1;
            public const int Unlocked = 2;
            public const int Signalled = 3;
        }
    }

    [Fx.Tag.SynchronizationPrimitive(Fx.Tag.BlocksUsing.NonBlocking)]
    [Serializable]
    class SignalGate<T> : SignalGate
    {
        T Result { get; set; }

        public SignalGate()
            : base()
        {
        }

        public bool Signal(T result)
        {
            this.Result = result;
            return Signal();
        }

        public bool Unlock(out T result)
        {
            if (Unlock())
            {
                result = this.Result;
                return true;
            }

            result = default(T);
            return false;
        }
    }
}
