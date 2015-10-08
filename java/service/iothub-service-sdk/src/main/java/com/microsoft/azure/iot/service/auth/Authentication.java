/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.auth;

public class Authentication
{
    private SymmetricKey symmetricKey;

    public Authentication()
    {
        this.symmetricKey = new SymmetricKey();
    }

    public SymmetricKey getSymmetricKey()
    {
        return symmetricKey;
    }

    public void setSymmetricKey(SymmetricKey symmetricKey)
    {
        this.symmetricKey = symmetricKey;
    }
}
