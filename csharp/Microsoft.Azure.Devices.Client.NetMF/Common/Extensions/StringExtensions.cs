using System;
namespace Microsoft.Azure.Devices.Client.Extensions
{
    static class StringExtensions
    {
        #region extensions to handle string null, empty, etc.

        public static bool IsNullOrWhiteSpace(this string value)
        {
            if(value == null || value == "")
            {
                return true;
            }
            else
            {
                return false;
            }
        }

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

        #endregion

        #region DateTime string parser

        static string[] months = new string[] { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

        public static bool TryParseDateTime(this string s, out DateTime result)
        {
            // implement this using string manipulation

            result = new DateTime();

            // Wed, 07 Oct 2015 23:17:26 GMT
            // 0         1         2
            // 0123456789012345678901234567890
            try
            {
                int day = int.Parse(s.Substring(5, 2));
                int month = Array.IndexOf(months, s.Substring(8, 3)) + 1;
                int year = int.Parse(s.Substring(12, 4));

                int hour = int.Parse(s.Substring(17, 2));
                int minute = int.Parse(s.Substring(20, 2));
                int second = int.Parse(s.Substring(23, 2));

                result = new DateTime(year, month, day, hour, minute, second, 0);

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

                result = new DateTime(int.Parse(date[2]), int.Parse(date[0]), int.Parse(date[1]), int.Parse(time[0]), int.Parse(time[1]), int.Parse(time[2]));

                // we should have a valid DateTime 
                return true;
            }
            catch { };

            // no joy parsing this one
            return false;
        }

        #endregion
    }
}
