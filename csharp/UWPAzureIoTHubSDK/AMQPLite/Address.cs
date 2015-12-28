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

namespace Amqp
{
    using System;
    using Amqp.Types;

    /// <summary>
    /// Defines the address of an AMQP endpoint. An address has the following form:
    ///   amqp[s] :// [user:[password]@] domain[:port] [/[path]
    /// Where domain can be:
    ///   host | ip | name
    /// If "amqps" is specified, the connection uses TLS in the underlying transport.
    /// When port is not specified, it is set to the standard based on scheme (amqp: 5672, amqps: 5671)
    /// path is not used by the library.
    /// </summary>
    public sealed class Address
    {
        internal const string Amqp = "AMQP";
        internal const string Amqps = "AMQPS";
        const int AmqpPort = 5672;
        const int AmqpsPort = 5671;

        /// <summary>
        /// Initializes a new instance of the Address class from a string.
        /// </summary>
        /// <param name="address">The string representation of the address.</param>
        public Address(string address)
        {
            this.Port = -1;
            this.Parse(address);
            this.SetDefault();
        }

        /// <summary>
        /// Initializes a new instance of the Address class from individual components.
        /// </summary>
        /// <param name="host">The domain of the address.</param>
        /// <param name="port">The port number of the address.</param>
        /// <param name="user">User name for SASL PLAIN profile.</param>
        /// <param name="password">Password for SASL PLAIN profile.</param>
        /// <param name="path">The path of the address.</param>
        /// <param name="scheme">Protocol scheme, which can be either "amqp" or "amqps".</param>
        public Address(string host, int port, string user = null, string password = null, string path = "/", string scheme = Amqps)
        {
            this.Host = host;
            this.Port = port;
            this.Path = path;
            this.Scheme = scheme;
            this.User = user;
            this.Password = password;
            this.SetDefault();
        }

        /// <summary>
        /// Gets the protocol scheme.
        /// </summary>
        public string Scheme
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets a value indicating where TLS is enabled.
        /// </summary>
        public bool UseSsl
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the host of the address.
        /// </summary>
        public string Host
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the port number of the address.
        /// </summary>
        public int Port
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the user name that is used for SASL PLAIN profile.
        /// </summary>
        public string User
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the password that is used for SASL PLAIN profile.
        /// </summary>
        public string Password
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the path of the address.
        /// </summary>
        public string Path
        {
            get;
            private set;
        }

        enum ParseState
        {
            Scheme,
            Slash1,
            Slash2,
            User,
            Password,
            Host,
            Port,
            Path
        }

        void Parse(string address)
        {
            //  amqp[s]://user:password@a.contoso.com:port/foo/bar
            ParseState state = ParseState.Scheme;
            int startIndex = 0;
            for (int i = 0; i < address.Length; ++i)
            {
                switch (address[i])
                {
                    case ':':
                        if (state == ParseState.Scheme)
                        {
                            this.Scheme = address.Substring(startIndex, i - startIndex);
                            state = ParseState.Slash1;
                        }
                        else if (state == ParseState.User)
                        {
                            this.User = address.Substring(startIndex, i - startIndex);
                            state = ParseState.Password;
                            startIndex = i + 1;
                        }
                        else if (state == ParseState.Host)
                        {
                            this.Host = address.Substring(startIndex, i - startIndex);
                            state = ParseState.Port;
                            startIndex = i + 1;
                        }
                        else
                        {
                            throw new AmqpException(ErrorCode.InvalidField,
                                Fx.Format(SRAmqp.InvalidAddressFormat));
                        }
                        break;
                    case '/':
                        if (state == ParseState.Slash1)
                        {
                            state = ParseState.Slash2;
                        }
                        else if (state == ParseState.Slash2)
                        {
                            state = ParseState.User;
                            startIndex = i + 1;
                        }
                        else if (state == ParseState.User || state == ParseState.Host)
                        {
                            this.Host = address.Substring(startIndex, i - startIndex);
                            state = ParseState.Path;
                            startIndex = i;
                        }
                        else if (state == ParseState.Port)
                        {
                            this.Port = int.Parse(address.Substring(startIndex, i - startIndex));
                            state = ParseState.Path;
                            startIndex = i;
                        }
                        else if (state == ParseState.Password)
                        {
                            this.Host = this.User;
                            this.User = null;
                            this.Port = int.Parse(address.Substring(startIndex, i - startIndex));
                            state = ParseState.Path;
                            startIndex = i;
                        }
                        break;
                    case '@':
                        if (state == ParseState.Password)
                        {
                            this.Password = address.Substring(startIndex, i - startIndex);
                            state = ParseState.Host;
                            startIndex = i + 1;
                        }
                        else
                        {
                            throw new AmqpException(ErrorCode.InvalidField,
                                Fx.Format(SRAmqp.InvalidAddressFormat));
                        }
                        break;
                    default:
                        break;
                }

                if (state == ParseState.Path)
                {
                    this.Path = address.Substring(startIndex);
                    break;
                }
            }

            // check state in case of no trailing slash
            if (state == ParseState.User || state == ParseState.Host)
            {
                this.Host = address.Substring(startIndex);
            }
            else if (state == ParseState.Password)
            {
                this.Host = this.User;
                this.User = null;
                if (startIndex < address.Length - 1)
                {
                    this.Port = int.Parse(address.Substring(startIndex));
                }
            }
            else if (state == ParseState.Port)
            {
                this.Port = int.Parse(address.Substring(startIndex));
            }

            if (this.Password != null && this.Password.Length > 0)
            {
                this.Password = Uri.UnescapeDataString(this.Password);
            }

            if (this.User != null && this.User.Length > 0)
            {
                this.User = Uri.UnescapeDataString(this.User);
            }

            if (this.Host != null)
            {
                this.Host = Uri.UnescapeDataString(this.Host);
            }
        }

        void SetDefault()
        {
            string schemeUpper = this.Scheme.ToUpper();
            if (schemeUpper == Amqps)
            {
                this.UseSsl = true;
            }

            if (this.Port == -1)
            {
                if (this.UseSsl)
                {
                    this.Port = AmqpsPort;
                }
                else if (schemeUpper == Amqp)
                {
                    this.Port = AmqpPort;
                }
            }

            if (this.Path == null)
            {
                this.Path = "/";
            }
        }
    }
}