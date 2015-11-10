namespace Microsoft.Azure.Devices.Client.Extensions
{
    static class StringExtensions
    {
        public static bool IsNullOrWhiteSpace(this string value)
        {
#if NETMF
            if (value == null || value == "")
            {
                return true;
            }
            else
            {
                return false;
            }
#else
            return string.IsNullOrWhiteSpace(value);
#endif
        }

#if NETMF
        public static bool IsNullOrEmpty(this string value)
        {
            if (value == null || value == "")
            {
                return true;
            }
            else
            {
                return false;
            }
        }
#endif
    }
}
