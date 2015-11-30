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

        #region DateTime string parser 

        static string[] months = new string[] { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        public static bool TryParseDateTime(this string s, out System.DateTime result)
        {
            // implement this using string manipulation

            result = new System.DateTime();

            // Wed, 07 Oct 2015 23:17:26 GMT
            // 0         1         2
            // 0123456789012345678901234567890
            try
            {
                int day = int.Parse(s.Substring(5, 2));
                int month = System.Array.IndexOf(months, s.Substring(8, 3)) + 1;
                int year = int.Parse(s.Substring(12, 4));

                int hour = int.Parse(s.Substring(17, 2));
                int minute = int.Parse(s.Substring(20, 2));
                int second = int.Parse(s.Substring(23, 2));

                // create DateTime with these values
                // make sure it's UTC
                result = new System.DateTime(year, month, day, hour, minute, second, 0).ToUniversalTime();

                // we should have a valid DateTime 
                return true;
            }
            catch { };

            // try with next format
            // 10/7/2015 23:17:26 PM
            // this one has a nice pattern, split with spaces to get date and time and then split each one to get individual date bits
            try
            {
                string[] splitDateTime = s.Split(' ');
                string[] date = splitDateTime[0].Split('/');
                string[] time = splitDateTime[1].Split(':');

                // create DateTime with these values
                // make sure it's UTC
                result = new System.DateTime(int.Parse(date[2]), int.Parse(date[0]), int.Parse(date[1]), int.Parse(time[0]), int.Parse(time[1]), int.Parse(time[2])).ToUniversalTime();

                // check for PM time and add 12H if required
                if (splitDateTime[2] == "PM")
                {
                    result = result.AddHours(12);
                }

                // we should have a valid DateTime now
                return true;
            }
            catch { };

            // no joy parsing this one
            return false;
        }

        #endregion

#endif
    }
}
