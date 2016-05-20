namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;

    interface IDelegatingHandler: IDisposable
    {
        IDelegatingHandler InnerHandler { get; }

        Task AbandonAsync(string lockToken);
        Task CloseAsync();
        Task CompleteAsync(string lockToken);
        Task OpenAsync(bool explicitOpen);
        Task<Message> ReceiveAsync();
        Task<Message> ReceiveAsync(TimeSpan timeout);
        Task RejectAsync(string lockToken);
#if WINDOWS_UWP
        [Windows.Foundation.Metadata.DefaultOverload]
#endif
        Task SendEventAsync(Message message);
        Task SendEventAsync(IEnumerable<Message> messages);
    }
}