// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

public final class SizeLimitExceededException extends Exception {
	private static final long serialVersionUID = 3738564189144848045L;

	public SizeLimitExceededException() {
		super();
	}

	public SizeLimitExceededException(String explanation) {
		super(explanation);
	}
}
