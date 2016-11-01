// For an introduction to the Blank template, see the following documentation:
// http://go.microsoft.com/fwlink/?LinkId=232509
(function () {
    "use strict";

    var app = WinJS.Application;
    var activation = Windows.ApplicationModel.Activation;
    var azureClient = Microsoft.Azure.Devices.Client;

    app.onactivated = function (args) {
        if (args.detail.kind === activation.ActivationKind.launch) {
            if (args.detail.previousExecutionState !== activation.ApplicationExecutionState.terminated) {

                // It is NOT a good practice to put device credentials in the code as shown below.
                // This is done in this sample for simplicity purposes.
                // Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.
                var deviceConnectionString = "<replace with real connection string>";

                var str = "Hello from JS";
                var bytes = [];

                for (var i = 0; i < str.length; ++i) {
                    bytes.push(str.charCodeAt(i));
                }

                var eventMessage = new azureClient.Message(bytes);

                var deviceClient = azureClient.DeviceClient.createFromConnectionString(deviceConnectionString, azureClient.TransportType.http1);

                deviceClient.sendEventAsync(eventMessage);

            } else {
                // TODO: This application was suspended and then terminated.
                // To create a smooth user experience, restore application state here so that it looks like the app never stopped running.
            }
            args.setPromise(WinJS.UI.processAll());
        }
    };

    app.oncheckpoint = function (args) {
        // TODO: This application is about to be suspended. Save any state that needs to persist across suspensions here.
        // You might use the WinJS.Application.sessionState object, which is automatically saved and restored across suspension.
        // If you need to complete an asynchronous operation before your application is suspended, call args.setPromise().
    };

    app.start();
})();
