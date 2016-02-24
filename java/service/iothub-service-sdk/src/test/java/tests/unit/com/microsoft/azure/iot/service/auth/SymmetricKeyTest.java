/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.auth;

import mockit.Deencapsulation;
import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

@RunWith(JMockit.class)
public class SymmetricKeyTest
{
    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_001: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void setPrimaryKey_length_less_than_16()
    {
        // Arrange
        String key = "012345678901234";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setPrimaryKey(key);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_001: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void setPrimaryKey_length_greater_than_64()
    {
        // Arrange
        String key = "01234567890123456789012345678901234567890123456789012345678901234";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setPrimaryKey(key);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_002: [The function shall set the private primaryKey member to the given value if the length validation passed]
    @Test
    public void setPrimaryKey_length_good_case_min()
    {
        // Arrange
        String key = "0123456789012345";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setPrimaryKey(key);
        // Assert
        assertEquals(key, Deencapsulation.getField(symmetricKey, "primaryKey"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_002: [The function shall set the private primaryKey member to the given value if the length validation passed]
    @Test
    public void setPrimaryKey_length_good_case_max()
    {
        // Arrange
        String key = "0123456789012345678901234567890123456789012345678901234567890123";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setPrimaryKey(key);
        // Assert
        assertEquals(key, Deencapsulation.getField(symmetricKey, "primaryKey"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void setSecondaryKey_length_less_than_16()
    {
        // Arrange
        String key = "012345678901234";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setSecondaryKey(key);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void setSecondaryKey_length_greater_than_64()
    {
        // Arrange
        String key = "01234567890123456789012345678901234567890123456789012345678901234";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setSecondaryKey(key);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
    @Test
    public void setSecondaryKey_length_good_case_min()
    {
        // Arrange
        String key = "0123456789012345";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setSecondaryKey(key);
        // Assert
        assertEquals(key, Deencapsulation.getField(symmetricKey, "secondaryKey"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
    @Test
    public void setSecondaryKey_length_good_case_max()
    {
        // Arrange
        String key = "0123456789012345678901234567890123456789012345678901234567890123";
        SymmetricKey symmetricKey = new SymmetricKey();
        // Act
        symmetricKey.setSecondaryKey(key);
        // Assert
        assertEquals(key, Deencapsulation.getField(symmetricKey, "secondaryKey"));
    }
}
