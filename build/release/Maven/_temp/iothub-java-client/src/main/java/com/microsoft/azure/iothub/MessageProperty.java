// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

/** An IoT Hub message property. */
public final class MessageProperty {
    /**
     * A set of reserved property names. The reserved property names are
     * interpreted in a meaningful way by the device and the IoT Hub.
     */
    public static final Set<String> RESERVED_PROPERTY_NAMES;

    static {
        HashSet<String> reservedPropertyNames = new HashSet<>();
        reservedPropertyNames.add("message-id");
        reservedPropertyNames.add("iothub-enqueuedtime");
        reservedPropertyNames.add("iothub-messagelocktoken");
        reservedPropertyNames.add("iothub-sequencenumber");
        reservedPropertyNames.add("to");
        reservedPropertyNames.add("absolute-expiry-time");
        reservedPropertyNames.add("correlation-id");
        reservedPropertyNames.add("user-id");
        reservedPropertyNames.add("iothub-operation");
        reservedPropertyNames.add("iothub-partition-key");
        reservedPropertyNames.add("iothub-ack");
        reservedPropertyNames.add("iothub-connection-device-id");
        reservedPropertyNames.add("iothub-connection-auth-method");
        reservedPropertyNames.add("iothub-connection-auth-generation-id");
        reservedPropertyNames.add("content-type");
        reservedPropertyNames.add("content-encoding");

        RESERVED_PROPERTY_NAMES = Collections.unmodifiableSet(reservedPropertyNames);
    }

    /** The property name. */
    protected final String name;
    /** The property value. */
    protected final String value;

    /**
     * Constructor.
     *
     * @param name The IoT Hub message property name.
     * @param value The IoT Hub message property value.
     *
     * @throws IllegalArgumentException if the name and value constitute an
     * invalid IoT Hub message property. A message property can only contain
     * US-ASCII printable chars, with some exceptions as specified in RFC 2047.
     * A message property name cannot be one of the reserved property names.
     */
    public MessageProperty(String name, String value) {
        if (name == null) {
            throw new IllegalArgumentException("Property argument 'name' cannot be null.");
        }

        if (value == null) {
            throw new IllegalArgumentException("Property argument 'value' cannot be null.");
        }

        // Codes_SRS_MESSAGEPROPERTY_11_002: [If the name contains a character that is not in US-ASCII printable characters or is one of: ()<>@,;:\"/[]?={} (space) (horizontal tab), the function shall throw an IllegalArgumentException.]
        if (!usesValidChars(name)) {
            String errMsg = String.format("%s is not a valid IoT Hub message property name.\n", name);
            throw new IllegalArgumentException(errMsg);
        }

        // Codes_SRS_MESSAGEPROPERTY_11_008: [If the name is a reserved property name, the function shall throw an IllegalArgumentException.]
        if (RESERVED_PROPERTY_NAMES.contains(name)) {
            String errMsg = String.format("%s is a reserved IoT Hub message property name.\n", name);
            throw new IllegalArgumentException(errMsg);
        }

        // Codes_SRS_MESSAGEPROPERTY_11_003: [If the value contains a character that is not in US-ASCII printable characters or is one of: ()<>@,;:\"/[]?={} (space) (horizontal tab), the function shall throw an IllegalArgumentException.]
        if (!usesValidChars(value))
        {
            String errMsg = String.format("%s is not a valid IoT Hub message property value.\n", value);
            throw new IllegalArgumentException(errMsg);
        }

        // Codes_SRS_MESSAGEPROPERTY_11_001: [The constructor shall save the property name and value.]
        this.name = name;
        this.value = value;
    }

    /**
     * Returns the property name.
     *
     * @return the property name.
     */
    public String getName() {
        // Codes_SRS_MESSAGEPROPERTY_11_004: [The function shall return the property name.]
        return this.name;
    }

    /**
     * Returns the property value.
     *
     * @return the property value.
     */
    public String getValue() {
        // Codes_SRS_MESSAGEPROPERTY_11_005: [The function shall return the property value.]
        return this.value;
    }

    /**
     * Equivalent to property.getName().equalsIgnoreCase(name).
     *
     * @param name the property name.
     *
     * @return true if the given name is the property name.
     */
    public boolean hasSameName(String name) {
        boolean nameMatches = false;

        // Codes_SRS_MESSAGEPROPERTY_11_006: [The function shall return true if and only if the property has the given name, where the names are compared in a case-insensitive manner.]
        if (this.getName().equalsIgnoreCase(name)) {
            nameMatches = true;
        }

        return nameMatches;
    }

    /**
     * Returns whether the property is a valid application property. The
     * property is valid if it is not one of the reserved properties, only uses
     * US-ASCII printable chars, and does not contain: <code>()&lt;&gt;@,;:\"/[]?={}</code> (space)
     * (horizontal tab).
     *
     * @param name the property name.
     * @param value the property value.
     *
     * @return whether the property is a valid application property.
     */
    public static boolean isValidAppProperty(String name, String value) {
        boolean propertyIsValid = false;

        // Codes_SRS_MESSAGEPROPERTY_11_007: [The function shall return true if and only if the name and value only use characters in: US-ASCII printable characters, excluding ()<>@,;:\"/[]?={} (space) (horizontal tab), and the name is not a reserved property name.]
        if (!RESERVED_PROPERTY_NAMES.contains(name)
                && usesValidChars(name)
                && usesValidChars(value)) {
            propertyIsValid = true;
        }

        return propertyIsValid;
    }

    /**
     * Returns true if the string only uses US-ASCII printable chars and does
     * not contain: <code>()&lt;&gt;@,;:\"/[]?={}</code> (space) (horizontal tab)
     *
     * @param s the string.
     *
     * @return whether the string only uses US-ASCII printable chars and does
     * not contain: <code>()&lt;&gt;@,;:\"/[]?={}</code> (space) (horizontal tab)
     */
    protected static boolean usesValidChars(String s) {
        boolean isValid = false;

        if (s.matches("[\\p{Print}]+")
                && s.matches(
                "[^()<>@,;:\\\\\"/\\[\\]\\?=\\{\\}\u0040\u0011]+"))
        {
            isValid = true;
        }

        return isValid;
    }

    protected MessageProperty() {
        this.name = null;
        this.value = null;
    }
}
