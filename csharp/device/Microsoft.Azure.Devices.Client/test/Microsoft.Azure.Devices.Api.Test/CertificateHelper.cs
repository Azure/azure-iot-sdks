// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.ApiTest
{
    using System;
    using System.Collections.Concurrent;
    using System.Globalization;
    using System.IO;
    using System.Reflection;
    using System.Security.Cryptography.X509Certificates;

    public static class CertificateHelper
    {
        static readonly ConcurrentDictionary<string, X509Certificate2> certificatesFromFiles = new ConcurrentDictionary<string, X509Certificate2>();

        public static X509Certificate2 GetCertificateFromResource(string resourceName, string password)
        {
            var assembly = Assembly.GetCallingAssembly();
            var fullResourceName = string.Format(CultureInfo.InvariantCulture, "{0}.{1}", assembly.GetName().Name, resourceName);
            var stream = assembly.GetManifestResourceStream(fullResourceName);

            if (stream == null)
            {
                throw new Exception("Could not find a specified certificate resource.");
            }

            var memoryStream = new MemoryStream();

            stream.CopyTo(memoryStream);
            byte[] certificateContent = memoryStream.ToArray();

            var certificate = new X509Certificate2(certificateContent, password,
                X509KeyStorageFlags.Exportable | X509KeyStorageFlags.PersistKeySet | X509KeyStorageFlags.MachineKeySet);

            return certificate;
        }

        public static X509Certificate2 GetCertificateFromFile(string fileName, string password)
        {
            X509Certificate2 certificate;
            if (certificatesFromFiles.TryGetValue(fileName, out certificate))
            {
                return certificate;
            }

            certificate = new X509Certificate2(fileName, password, X509KeyStorageFlags.Exportable);
            certificatesFromFiles[fileName] = certificate;
            return certificate;
        }

        public static X509Certificate2 GetCertificateFromPFX(byte[] pfxBytes, string password)
        {
            return new X509Certificate2(pfxBytes, password, X509KeyStorageFlags.PersistKeySet | X509KeyStorageFlags.Exportable);
        }

        public static X509Certificate2 GetCertificateFromThumbprint(string thumbprint)
        {
            return GetCertificateFromThumbprint(thumbprint, StoreName.My, StoreLocation.LocalMachine);
        }

        public static X509Certificate2 GetCertificateFromThumbprint(string thumbprint, bool validOnly)
        {
            return GetCertificateFromThumbprint(thumbprint, StoreName.My, StoreLocation.LocalMachine, validOnly);
        }

        public static X509Certificate2 GetCertificateFromThumbprint(string thumbprint, StoreName storeName, StoreLocation storeLocation)
        {
            return GetCertificateFromThumbprint(thumbprint, storeName, storeLocation, true);
        }

        public static X509Certificate2 GetCertificateFromThumbprint(string thumbprint, StoreName storeName, StoreLocation storeLocation, bool validOnly)
        {
            X509Store store = null;
            X509Certificate2 cert = null;

            try
            {
                store = new X509Store(storeName, storeLocation);
                store.Open(OpenFlags.ReadOnly);
                X509Certificate2Collection certCollection = store.Certificates.Find(X509FindType.FindByThumbprint, thumbprint, validOnly);
                X509Certificate2Enumerator enumerator = certCollection.GetEnumerator();
                while (enumerator.MoveNext())
                {
                    cert = enumerator.Current;
                }
            }
            finally
            {
                if (store != null)
                {
                    store.Close();
                }
            }

            return cert;
        }

        public static byte[] GetPFXFromCertificate(X509Certificate2 certificate, string password)
        {
            if (!certificate.HasPrivateKey)
            {
                throw new InvalidOperationException("Supplied certificate does not have an exportable private key");
            }

            return certificate.Export(X509ContentType.Pfx, password);
        }

        public static void InstallCertificate(X509Certificate2 certificate)
        {
            InstallCertificate(certificate, StoreLocation.LocalMachine);
        }

        public static void InstallCertificate(X509Certificate2 certificate, StoreLocation storeLocation)
        {
            var store = new X509Store(StoreName.My, storeLocation);

            try
            {
                store.Open(OpenFlags.ReadWrite);
                store.Add(certificate);
            }
            finally
            {
                store.Close();
            }
        }

        public static X509Certificate2 InstallCertificateFromFile(string pfxFile, string passwordFile)
        {
            return InstallCertificateFromFile(pfxFile, passwordFile, StoreLocation.LocalMachine);
        }

        public static X509Certificate2 InstallCertificateFromFile(string pfxFile, string passwordFile, StoreLocation storeLocation)
        {
            var certPassword = File.ReadAllText(passwordFile);
            X509Certificate2 certificate = CertificateHelper.GetCertificateFromFile(pfxFile, certPassword);
            CertificateHelper.InstallCertificate(certificate, storeLocation);

            return certificate;
        } 

        public static void InstallSslCertificateRootChain(string sslCertThumbprint)
        {
            X509Store rootStore = new X509Store(StoreName.Root, StoreLocation.LocalMachine);
            X509Store certificateAuthorityStore = new X509Store(StoreName.CertificateAuthority, StoreLocation.LocalMachine);

            try
            {
                X509Certificate2 sslCertificate = CertificateHelper.GetCertificateFromThumbprint(sslCertThumbprint);
                if (sslCertificate != null)
                {
                    rootStore.Open(OpenFlags.ReadOnly);
                    certificateAuthorityStore.Open(OpenFlags.ReadWrite);

                    // We need the issuers in the root so that cert validation is fast, especially for mobile clients.
                    X509Chain chain = new X509Chain();
                    chain.Build(sslCertificate);
                    foreach (X509ChainElement chainElement in chain.ChainElements)
                    {
                        X509Certificate2 chainedCert = chainElement.Certificate;
                        if (chainedCert.Thumbprint != sslCertificate.Thumbprint)
                        {
                            bool found = rootStore.Certificates.Find(X509FindType.FindByThumbprint, chainedCert.Thumbprint, false).Count > 0 ||
                                certificateAuthorityStore.Certificates.Find(X509FindType.FindByThumbprint, chainedCert.Thumbprint, false).Count > 0;
                            if (!found)
                            {
                                certificateAuthorityStore.Add(chainedCert);
                            }
                        }
                    }
                    rootStore.Close();
                    rootStore = null;
                    certificateAuthorityStore.Close();
                    certificateAuthorityStore = null;
                }
                else
                {
                    throw new Exception(string.Format(CultureInfo.InvariantCulture, "Could not find SSL Cert with thumbprint {0} in LocalMachine My store", sslCertThumbprint));
                }
            }
            catch (Exception ex)
            {
                throw new Exception(string.Format(CultureInfo.InvariantCulture, "Failed to install root certs for SSL Cert with thumbprint {0} in LocalMachine store.  Exception: {1}", sslCertThumbprint, ex));
            }
            finally
            {
                if (rootStore != null)
                {
                    rootStore.Close();                    
                }

                if (certificateAuthorityStore != null)
                {
                    certificateAuthorityStore.Close();
                }
            }
        }
    }
}
