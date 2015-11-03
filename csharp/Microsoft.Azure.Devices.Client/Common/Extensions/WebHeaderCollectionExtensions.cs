using System.Net;

namespace Microsoft.Azure.Devices.Client
{
    static class WebHeaderCollectionExtensions
    {
        public static void TryGetValues(this WebHeaderCollection s, string name, out string[] values)
        {
            values = s.GetValues(name);
        }
    }
}
