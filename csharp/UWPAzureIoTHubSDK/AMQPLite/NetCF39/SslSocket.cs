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

using System;
using System.Globalization;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Threading;

namespace Amqp
{
    /// <summary>
    /// Delagate for user certificate validatin callback
    /// </summary>
    /// <param name="sender">Object sender (SslSocket instance)</param>
    /// <param name="certificate">X509 certificate</param>
    /// <param name="sslPolicyErrors">Internal validation error</param>
    /// <returns>Certificate status (valid or not)</returns>
    public delegate bool RemoteCertificateValidationCallback(object sender, X509Certificate certificate, SslPolicyErrors sslPolicyErrors);

    class SslSocket : ITransport, IDisposable
    {
        #region P/Invoke constants define ...

        // reference : winsock2.h in the SDK

        // in/out parameter for ioctlsocket() -> WSAIoctl() in winsock
        private const uint IOC_VOID = 0x20000000;           // no parameters
        private const uint IOC_OUT = 0x40000000;            // copy out parameters
        private const uint IOC_IN = 0x80000000;             // copy in parameters
        private const uint IOC_INOUT = (IOC_IN | IOC_OUT);

        // reference : sslsock.h in the SDK

        // options and option values for creating a secure socket
        private const ushort SO_SECURE = 0x2001;        // add security to socket
        private const ushort SO_SEC_NONE = 0x2002;      // security not used on socket
        private const ushort SO_SEC_SSL = 0x2004;       // use unified SSL/PCT for security

        // SSL WSAIoctl control code tags
        // NOTE : in managed code we have related Socket.IOControl() method

        private const int SO_SSL_FAMILY = 0x00730000;
        private const int SO_SSL_FAMILY_MASK = 0x07ff0000;

        private const long _SO_SSL = ((2L << 27) | SO_SSL_FAMILY);

        // base SSL WSAIoctl tags
        private const int _SO_SSL_CAPABILITIES = 0x01;
        private const int _SO_SSL_FLAGS = 0x02;
        private const int _SO_SSL_PROTOCOLS = 0x03;
        private const int _SO_SSL_CIPHERS = 0x04;
        private const int _SO_SSL_CLIENT_OPTS = 0x05;
        private const int _SO_SSL_SERVER_OPTS = 0x06;
        private const int _SO_SSL_CLIENT_AUTH_OPTS = 0x07;
        private const int _SO_SSL_VALIDATE_CERT_HOOK = 0x08;
        private const int _SO_SSL_AUTH_REQUEST_HOOK = 0x09;
        private const int _SO_SSL_KEY_EXCH_HOOK = 0x0a;
        private const int _SO_SSL_SIGNATURE_HOOK = 0x0b;
        private const int _SO_SSL_PERFORM_HANDSHAKE = 0x0d;
        private const int _SO_SSL_CONNECTION_INFO = 0x0e;
        private const int _SO_SSL_PEERNAME = 0x0f;

        // actual SSL WSAIoctl commands
        private const long SO_SSL_GET_CAPABILITIES = (IOC_OUT | _SO_SSL | _SO_SSL_CAPABILITIES);
        private const long SO_SSL_SET_FLAGS = (IOC_IN  | _SO_SSL | _SO_SSL_FLAGS);
        private const long SO_SSL_GET_FLAGS = (IOC_OUT | _SO_SSL | _SO_SSL_FLAGS);
        private const long SO_SSL_SET_PROTOCOLS = (IOC_IN  | _SO_SSL | _SO_SSL_PROTOCOLS);
        private const long SO_SSL_GET_PROTOCOLS = (IOC_OUT | _SO_SSL | _SO_SSL_PROTOCOLS);
        private const long SO_SSL_SET_CIPHERS = (IOC_IN  | _SO_SSL | _SO_SSL_CIPHERS);
        private const long SO_SSL_GET_CIPHERS = (IOC_OUT | IOC_IN | _SO_SSL | _SO_SSL_CIPHERS);
        private const long SO_SSL_SET_CLIENT_OPTS = (IOC_IN  | _SO_SSL | _SO_SSL_CLIENT_OPTS);
        private const long SO_SSL_GET_CLIENT_OPTS = (IOC_OUT | _SO_SSL | _SO_SSL_CLIENT_OPTS);
        private const long SO_SSL_SET_SERVER_OPTS = (IOC_IN  | _SO_SSL | _SO_SSL_SERVER_OPTS);
        private const long SO_SSL_GET_SERVER_OPTS = (IOC_OUT | _SO_SSL | _SO_SSL_SERVER_OPTS);
        private const long SO_SSL_SET_CLIENT_AUTH_OPTS = (IOC_IN  | _SO_SSL | _SO_SSL_CLIENT_AUTH_OPTS);
        private const long SO_SSL_GET_CLIENT_AUTH_OPTS = (IOC_OUT | _SO_SSL | _SO_SSL_CLIENT_AUTH_OPTS);
        private const long SO_SSL_SET_VALIDATE_CERT_HOOK = (IOC_IN  | _SO_SSL | _SO_SSL_VALIDATE_CERT_HOOK);
        private const long SO_SSL_GET_VALIDATE_CERT_HOOK = (IOC_OUT | _SO_SSL | _SO_SSL_VALIDATE_CERT_HOOK);
        private const long SO_SSL_SET_AUTH_REQUEST_HOOK = (IOC_IN  | _SO_SSL | _SO_SSL_AUTH_REQUEST_HOOK);
        private const long SO_SSL_GET_AUTH_REQUEST_HOOK = (IOC_OUT | _SO_SSL | _SO_SSL_AUTH_REQUEST_HOOK);
        private const long SO_SSL_SET_KEY_EXCH_HOOK = (IOC_IN  | _SO_SSL | _SO_SSL_KEY_EXCH_HOOK);
        private const long SO_SSL_GET_KEY_EXCH_HOOK = (IOC_OUT | _SO_SSL | _SO_SSL_KEY_EXCH_HOOK);
        private const long SO_SSL_SET_SIGNATURE_HOOK = (IOC_IN  | _SO_SSL | _SO_SSL_SIGNATURE_HOOK);
        private const long SO_SSL_GET_SIGNATURE_HOOK = (IOC_OUT | _SO_SSL | _SO_SSL_SIGNATURE_HOOK);
        private const long SO_SSL_PERFORM_HANDSHAKE = (          _SO_SSL | _SO_SSL_PERFORM_HANDSHAKE);
        private const long SO_SSL_GET_CONNECTION_INFO = (IOC_OUT | _SO_SSL | _SO_SSL_CONNECTION_INFO);
        private const long SO_SSL_SET_PEERNAME = (IOC_OUT | _SO_SSL | _SO_SSL_PEERNAME);
        
        // error codes to be returned by the hook functions.
        private const int SSL_ERR_OKAY = 0;
        private const int SSL_ERR_FAILED = 2;
        private const int SSL_ERR_BAD_LEN = 3;
        private const int SSL_ERR_BAD_TYPE = 4;
        private const int SSL_ERR_BAD_DATA = 5;
        private const int SSL_ERR_NO_CERT = 6;
        private const int SSL_ERR_BAD_SIG = 7;
        private const int SSL_ERR_CERT_EXPIRED = 8;
        private const int SSL_ERR_CERT_REVOKED = 9;
        private const int SSL_ERR_CERT_UNKNOWN = 10;
        private const int SSL_ERR_SIGNATURE = 11;
        
        // declarations for _SO_SSL_VALIDATE_CERT_HOOK
        private const int SSL_CERT_X509 = 0x0001;
        private const int SSL_CERT_FLAG_ISSUER_UNKNOWN = 0x0001;
        
        #endregion

        #region P/Invoke declarations ...

        // sslsock.h : callback (function pointer) for validate certificate
        private delegate int SSLVALIDATECERTFUNC(uint dwType, IntPtr pvArg, uint dwChainLen, IntPtr pCertChain, uint dwFlags);

        // pointer to host name for certificate validation
        private IntPtr ptrHost;
        // pointer to hook function for certificate validation
        private IntPtr ptrValidateCertHook;

        #endregion

        // reference to underlying socket; 
        private Socket socket;
        // callback to user certificate validation
        private RemoteCertificateValidationCallback userCertificateValidationCallback;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="socket">Underlying socket</param>
        /// <param name="host">Remote host name</param>
        /// <param name="userCertificateValidationCallback">Callback to user certificate validation</param>
        public SslSocket(Socket socket, string host, RemoteCertificateValidationCallback userCertificateValidationCallback)
        {
            this.socket = socket;

            // allocate buffer for the host name a copy string
            this.ptrHost = Marshal.StringToBSTR(host);

            this.userCertificateValidationCallback = userCertificateValidationCallback;

            // set SSL (security) as option at socket level
            // NOTE : the SocketOptionName enum doesn't contain an option for security but we can cast SO_SECURE
            this.socket.SetSocketOption(SocketOptionLevel.Socket, (SocketOptionName)SO_SECURE, SO_SEC_SSL);

            // need to execute IOControl on socket (WSAIoctl in native) to seet the hook to validate function
            // IOCTL command = SO_SSL_SET_VALIDATE_CERT_HOOK;
            // IOCTL in = pointer to hook function and parameters
            // IOCTL out = nothing (null);

            // get a pointer to delegate for validate certificate function
            this.ptrValidateCertHook = Marshal.GetFunctionPointerForDelegate(new SSLVALIDATECERTFUNC(ValidateCertificate));

            // prepare buffer IOCTL in
            // 4 bytes : pointer to hook function
            // 4 bytes : pointer to paramaters
            byte[] optionInValue = new byte[8];
            // copy pointer to hook function into buffer
            byte[] validateCertHookBytes = BitConverter.GetBytes(ptrValidateCertHook.ToInt32());
            byte[] hostPtrBytes = BitConverter.GetBytes(ptrHost.ToInt32());
            Array.Copy(validateCertHookBytes, optionInValue, validateCertHookBytes.Length);
            Array.Copy(hostPtrBytes, 0, optionInValue, validateCertHookBytes.Length, hostPtrBytes.Length);
            
            unchecked
            {
                // set hook to validate certificate function
                this.socket.IOControl((int)SO_SSL_SET_VALIDATE_CERT_HOOK, optionInValue, null);
            }
        }

        void ITransport.Send(ByteBuffer buffer)
        {
            // SSL socket on WEC aren't full duplex : send/receive operations must be exclusive
            lock (this)
            {
                this.socket.Send(buffer.Buffer, buffer.Offset, buffer.Length, SocketFlags.None);
            }
        }

        int ITransport.Receive(byte[] buffer, int offset, int count)
        {
            int read = 0;
            do
            {
                // wait for incoming data
                if (this.socket.Poll(Timeout.Infinite, SelectMode.SelectRead))
                {
                    // SSL socket on WEC aren't full duplex : send/receive operations must be exclusive
                    lock (this)
                    {
                        read = this.socket.Receive(buffer, offset, count, SocketFlags.None);
                    }
                }
            } while (read == 0);

            return read;
        }

        public void Close()
        {
            this.Dispose();
        }

        /// <summary>
        /// Validate certificate method as callback to the socket
        /// </summary>
        /// <param name="dwType">Data type pointed to by pCertChain (SSL_CERT_X.509 if X509 certs chain)</param>
        /// <param name="pvArg">Pointer to application-defined context (passed by the SSLVALIDATECERTHOOK structure)</param>
        /// <param name="dwChainLen">Number of certificates pointed to by pCertChain (It will always be equal to one)</param>
        /// <param name="pCertChain">Pointer to the root certificate</param>
        /// <param name="dwFlags">Will contain SSL_CERT_FLAG_ISSUER_UNKNOWN if the root issuer of the certificate could not be found in the CA database</param>
        /// <returns>Result</returns>
        private int ValidateCertificate(uint dwType, IntPtr pvArg, uint dwChainLen, IntPtr pCertChain, uint dwFlags)
        {
            bool userResult = false;
            X509Certificate2 certificate = null;
            
            // execute internal certificate validation
            int result = ValidateCertificateInternal(dwType, pvArg, dwChainLen, pCertChain, dwFlags, out certificate);

            // execute user validation callback if available
            if (this.userCertificateValidationCallback != null)
                userResult = this.userCertificateValidationCallback(this, certificate, (SslPolicyErrors)result);

            result = userResult ? SSL_ERR_OKAY : SSL_ERR_FAILED;

            return result;
        }

        /// <summary>
        /// Validate certificate method as callback to the socket
        /// </summary>
        /// <param name="dwType">Data type pointed to by pCertChain (SSL_CERT_X.509 if X509 certs chain)</param>
        /// <param name="pvArg">Pointer to application-defined context (passed by the SSLVALIDATECERTHOOK structure)</param>
        /// <param name="dwChainLen">Number of certificates pointed to by pCertChain (It will always be equal to one)</param>
        /// <param name="pCertChain">Pointer to the root certificate</param>
        /// <param name="dwFlags">Will contain SSL_CERT_FLAG_ISSUER_UNKNOWN if the root issuer of the certificate could not be found in the CA database</param>
        /// <param name="certificate">X509 certificate</param>
        /// <returns>Result</returns>
        private int ValidateCertificateInternal(uint dwType, IntPtr pvArg, uint dwChainLen, IntPtr pCertChain, uint dwFlags, out X509Certificate2 certificate)
        {
            certificate = null;

            // check if it is a valid X509 certificate
            if (dwType != SSL_CERT_X509)
                return SSL_ERR_BAD_TYPE;

            // in debug mode accept self-signed certificates
#if !DEBUG
            // check if issuer is unknown
            if ((dwFlags & SSL_CERT_FLAG_ISSUER_UNKNOWN) != 0)
                return SSL_ERR_CERT_UNKNOWN;
#endif

            // sslsock.h : pCertChain is a pointer to BLOB structure
            //             - first 4 bytes are the certificate size
            //             - following bytes are the certificate itself
            // read certificate size
            int certSize = Marshal.ReadInt32(pCertChain);
            // pointer to start of certificate data
            IntPtr pCertData = Marshal.ReadIntPtr(new IntPtr(pCertChain.ToInt32() + sizeof(int)));

            byte[] certData = new byte[certSize];
            // read certificate data bytes
            for (int i = 0; i < certSize; i++)
                certData[i] = Marshal.ReadByte(pCertData, (int)i);

            // create X509 certificate from raw bytes
            try
            {
                certificate = new X509Certificate2(certData);
            }
            catch (ArgumentException) { return SSL_ERR_BAD_DATA; }
            catch (CryptographicException) { return SSL_ERR_BAD_DATA; }

            // check expiration date
            if (DateTime.Now > DateTime.Parse(certificate.GetExpirationDateString(), CultureInfo.CurrentCulture))
                return SSL_ERR_CERT_EXPIRED;

            // check the effective date
            if (DateTime.Now < DateTime.Parse(certificate.GetEffectiveDateString(), CultureInfo.CurrentCulture))
                return SSL_ERR_FAILED;

            // validate the certificate CN with provided host name
            string host = Marshal.PtrToStringBSTR(pvArg);
            if (!certificate.GetName().Contains("CN=" + host))
                return SSL_ERR_FAILED;
                
            return SSL_ERR_OKAY;
        }

        #region IDisposable ...

        /// <summary>
        /// Destructor
        /// </summary>
        ~SslSocket()
        {
            // the object went out of scope and finalized is called
            // call dispose ("false" param) to release unmanaged resources 
            // NOTE : the managed resources will anyways be released when GC 
            //        runs the next time.
            Dispose(false);
        }

        public void Dispose()
        {
            // method called from user that wants to release all resources (managed and not)
            Dispose(true);

            // we have already cleaned up so the finalizer is useless
            // (tell the GC not to call it later)
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Dispose object resources
        /// </summary>
        /// <param name="disposing">Dispose managed resources or not</param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // we want explicitily clean up resources (managed)
                this.ReleaseManagedResources();
            }
            else
            {
                // disposing "false" means that object went out of scope
                // finalizer is called and GC will release resources on next run
            }

            // release unmanaged resource because they will not be released by GC
            this.ReleaseUnmangedResources();
        }

        /// <summary>
        /// Release unmanaged resources
        /// </summary>
        private void ReleaseUnmangedResources()
        {
            // free memory of remote host name
            if (this.ptrHost != IntPtr.Zero)
            {
                Marshal.FreeBSTR(this.ptrHost);
                this.ptrHost = IntPtr.Zero;
            }
        }

        /// <summary>
        /// Release managed resources
        /// </summary>
        private void ReleaseManagedResources()
        {
            // close underlying socket
            if (this.socket != null)
                this.socket.Close();
        }

        #endregion
    }

    /// <summary>
    /// Errors related to internal validation
    /// </summary>
    public enum SslPolicyErrors
    {
        SslErrOkay = 0,
        SslErrFailed = 2,
        SslErrBadLen = 3,
        SslErrBadType = 4,
        SslErrBadData = 5,
        SslErrNoCert = 6,
        SslErrBadSig = 7,
        SslErrCertExpired = 8,
        SslErrCertRevoked = 9,
        SslErrCertUnknown = 10,
        SslErrSignature = 11
    }
}
