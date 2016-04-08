// WARNING
//
// This file has been generated automatically by Xamarin Studio from the outlets and
// actions declared in your storyboard file.
// Manual changes to this file will not be maintained.
//
using Foundation;
using System;
using System.CodeDom.Compiler;
using UIKit;

namespace DeviceClientSampleiOS
{
	[Register ("ViewController")]
	partial class ViewController
	{
		[Outlet]
		[GeneratedCode ("iOS Designer", "1.0")]
		UITextView receivedMessagesTextBox { get; set; }

		[Outlet]
		[GeneratedCode ("iOS Designer", "1.0")]
		UIButton sendMessagesButton { get; set; }

		[Action ("SendMessagesButton_TouchUpInside:")]
		[GeneratedCode ("iOS Designer", "1.0")]
		partial void SendMessagesButton_TouchUpInside (UIButton sender);

		[Action ("UIButton15_TouchUpInside:")]
		[GeneratedCode ("iOS Designer", "1.0")]
		partial void UIButton15_TouchUpInside (UIButton sender);

		void ReleaseDesignerOutlets ()
		{
			if (receivedMessagesTextBox != null) {
				receivedMessagesTextBox.Dispose ();
				receivedMessagesTextBox = null;
			}
			if (sendMessagesButton != null) {
				sendMessagesButton.Dispose ();
				sendMessagesButton = null;
			}
		}
	}
}
