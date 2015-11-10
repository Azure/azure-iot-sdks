// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Security.Cryptography;
    using System.Text;
    using System.Web.Security;

    /// <summary>
    /// Utility methods for generating cryptographically secure keys and passwords
    /// </summary>
    static public class CryptoKeyGenerator
    {
        const int DefaultPasswordLength = 16;
        const int GuidLength = 16;
        public const int Sha512KeySize = 64;

        public static byte[] GenerateKeyBytes(int keySize)
        {
            var keyBytes = new byte[keySize];
            using (var cyptoProvider = new RNGCryptoServiceProvider())
            {
                cyptoProvider.GetNonZeroBytes(keyBytes);
            }

            return keyBytes;
        }

        public static string GenerateKey(int keySize)
        {
            return Convert.ToBase64String(GenerateKeyBytes(keySize));
        }

        public static string GenerateKeyInHex(int keySize)
        {
            var keyBytes = new byte[keySize];
            using (var cyptoProvider = new RNGCryptoServiceProvider())
            {
                cyptoProvider.GetNonZeroBytes(keyBytes);
            }

            return BitConverter.ToString(keyBytes).Replace("-", "");
        }

        public static Guid GenerateGuid()
        {
            byte[] bytes = new byte[GuidLength];
            using (var rng = new RNGCryptoServiceProvider())
            {
                rng.GetBytes(bytes);

            }

            var time = BitConverter.ToUInt32(bytes, 0);
            var time_mid = BitConverter.ToUInt16(bytes, 4);
            var time_hi_and_ver = BitConverter.ToUInt16(bytes, 6);
            time_hi_and_ver = (ushort)((time_hi_and_ver | 0x4000) & 0x4FFF);

            bytes[8] = (byte)((bytes[8] | 0x80) & 0xBF);

            return new Guid(time, time_mid, time_hi_and_ver, bytes[8], bytes[9],
                bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
        }


        public static string GeneratePassword()
        {
            return GeneratePassword(DefaultPasswordLength, false);
        }

        public static string GeneratePassword(int length, bool base64Encoding)
        {
            var password = Membership.GeneratePassword(length, length / 2);
            if (base64Encoding)
            {
                password = Convert.ToBase64String(Encoding.UTF8.GetBytes(password));
            }
            return password;
        }
    }
}
