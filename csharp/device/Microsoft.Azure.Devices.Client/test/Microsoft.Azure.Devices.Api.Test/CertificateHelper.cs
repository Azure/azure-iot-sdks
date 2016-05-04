// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.ApiTest
{
    using System.Collections.Concurrent;
    using System.IO;
    using System.Security.Cryptography.X509Certificates;

    public static class CertificateHelper
    {
        static readonly ConcurrentDictionary<string, X509Certificate2> certificatesFromFiles = new ConcurrentDictionary<string, X509Certificate2>();

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
    }
}
