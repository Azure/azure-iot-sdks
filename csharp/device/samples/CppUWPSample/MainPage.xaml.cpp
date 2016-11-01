//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace CppUWPSample;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Microsoft::Azure::Devices::Client;
using namespace concurrency;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    InitializeComponent();

    // It is NOT a good practice to put device credentials in the code as shown below.
    // This is done in this sample for simplicity purposes.
    // Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.
    auto deviceClient = DeviceClient::CreateFromConnectionString(L"<replace>", TransportType::Http1);
    byte dataBuffer[] = { 'H', 'e', 'l', 'l', 'o' };
    auto pbuffer = ref new Platform::Array<byte>(&dataBuffer[0], _countof(dataBuffer));
    auto eventMessage = ref new Message(pbuffer);
    create_task(deviceClient->SendEventAsync(eventMessage)).then([] {
        OutputDebugString(L"message sent successfully\n");
    });
}
