//  
//  (c) Microsoft Corporation. See LICENSE.TXT file for licensing details 
//   
namespace IoTHubAmqp
{
    using System;
    using System.Text;

    class HttpUtility
    {
        public static string UrlEncode(string str)
        {
            if (str == null)
            {
                return null;
            }
            return new string(Encoding.UTF8.GetChars(UrlEncodeToBytes(str, Encoding.UTF8)));
        }

        static byte[] UrlEncodeToBytes(string str, Encoding e)
        {
            if (str == null)
            {
                return null;
            }
            var bytes = e.GetBytes(str);
            return UrlEncodeBytesToBytesInternal(bytes, 0, bytes.Length, false);
        }

        static byte[] UrlEncodeBytesToBytesInternal(byte[] bytes, int offset, int count, bool alwaysCreateReturnValue)
        {
            var num = 0;
            var num2 = 0;
            for (var i = 0; i < count; i++)
            {
                var ch = (char)bytes[offset + i];
                if (ch == ' ')
                {
                    num++;
                }
                else if (!IsSafe(ch))
                {
                    num2++;
                }
            }
            if ((!alwaysCreateReturnValue && (num == 0)) &&
                (num2 == 0))
            {
                return bytes;
            }
            var buffer = new byte[count + (num2 * 2)];
            var num4 = 0;
            for (var j = 0; j < count; j++)
            {
                var num6 = bytes[offset + j];
                var ch2 = (char)num6;
                if (IsSafe(ch2))
                {
                    buffer[num4++] = num6;
                }
                else if (ch2 == ' ')
                {
                    buffer[num4++] = 0x2b;
                }
                else
                {
                    buffer[num4++] = 0x25;
                    buffer[num4++] = (byte)IntToHex((num6 >> 4) & 15);
                    buffer[num4++] = (byte)IntToHex(num6 & 15);
                }
            }
            return buffer;
        }

        static char IntToHex(int n)
        {
            if (n <= 9)
            {
                return (char)(n + 0x30);
            }
            return (char)((n - 10) + 0x61);
        }

        static bool IsSafe(char ch)
        {
            if ((((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) ||
                ((ch >= '0') && (ch <= '9')))
            {
                return true;
            }
            switch (ch)
            {
                case '\'':
                case '(':
                case ')':
                case '*':
                case '-':
                case '.':
                case '_':
                case '!':
                    return true;
            }
            return false;
        }


        public static string UrlDecode(string url)
        {
            var data = Encoding.UTF8.GetBytes(url);
            return new string(Encoding.UTF8.GetChars(UrlDecodeBytesToBytes(data, 0, data.Length)));
        }

        static byte[] UrlDecodeBytesToBytes(byte[] bytes, int offset, int count)
        {
            var length = 0;
            var sourceArray = new byte[count];
            for (var i = 0; i < count; i++)
            {
                var index = offset + i;
                var num4 = bytes[index];
                if (num4 == 0x2b)
                {
                    num4 = 0x20;
                }
                else if ((num4 == 0x25) &&
                         (i < (count - 2)))
                {
                    var num5 = HexToInt((char)bytes[index + 1]);
                    var num6 = HexToInt((char)bytes[index + 2]);
                    if ((num5 >= 0) &&
                        (num6 >= 0))
                    {
                        num4 = (byte)((num5 << 4) | num6);
                        i += 2;
                    }
                }
                sourceArray[length++] = num4;
            }
            if (length < sourceArray.Length)
            {
                var destinationArray = new byte[length];
                Array.Copy(sourceArray, destinationArray, length);
                sourceArray = destinationArray;
            }
            return sourceArray;
        }

        public static int HexToInt(char h)
        {
            if ((h >= '0') &&
                (h <= '9'))
            {
                return (h - '0');
            }
            if ((h >= 'a') &&
                (h <= 'f'))
            {
                return ((h - 'a') + 10);
            }
            if ((h >= 'A') &&
                (h <= 'F'))
            {
                return ((h - 'A') + 10);
            }
            return -1;
        }
    }
}
