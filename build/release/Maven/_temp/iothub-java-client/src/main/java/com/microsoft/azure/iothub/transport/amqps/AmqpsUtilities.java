/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.AzureHubType;
import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.Data;
import org.apache.qpid.proton.amqp.messaging.Properties;
import org.apache.qpid.proton.message.impl.MessageImpl;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;

public class AmqpsUtilities {

    /**
     * Generates a valid SAS token from the client configuration.
     *
     * @param config the DeviceClientConfig
     *
     * @return a SAS token that authenticates the device to the IoT Hub.
     */
    protected static IotHubSasToken buildToken(DeviceClientConfig config)
    {
        String iotHubHostname = config.getIotHubHostname();
        String deviceId = config.getDeviceId();
        String deviceKey = config.getDeviceKey();
        long tokenValidSecs = config.getTokenValidSecs();

        long msgExpiryTime = System.currentTimeMillis() / 1000l + tokenValidSecs + 1l;

        String resourceUri = IotHubUri.getResourceUri(iotHubHostname, deviceId);

        IotHubSasToken sasToken = new IotHubSasToken(resourceUri, deviceId, deviceKey, msgExpiryTime);

        if (config.targetHubType == AzureHubType.IoTHub) {
            sasToken.appendSknValue = false;
        } else if (config.targetHubType == AzureHubType.EventHub) {
            sasToken.appendSknValue = true;
        }

        return sasToken;
    }

    /**
     * Converts an AMQPS message to a corresponding IoT Hub message.
     *
     * @param protonMsg the AMQPS message.
     *
     * @return the corresponding IoT Hub message.
     */
    protected static Message protonMessageToMessage(MessageImpl protonMsg){
        Message msg = null;
        if(protonMsg != null) {
            Data d = (Data) protonMsg.getBody();
            Binary b = d.getValue();
            byte[] msgBody = new byte[b.getLength()];
            ByteBuffer buffer = b.asByteBuffer();
            buffer.get(msgBody);
            msg = new Message(msgBody);

            Properties properties = protonMsg.getProperties();

            //Call all of the getters for the Proton message Properties and set those properties
            //in the IoT Hub message properties if they exist.
            for (Method m : properties.getClass().getMethods()) {
                if (m.getName().startsWith("get")) {
                    try {
                        String propertyName = Character.toLowerCase(m.getName().charAt(3)) + m.getName().substring(4);
                        Object value = m.invoke(properties);
                        if (value != null && !propertyName.equals("class")) {
                            String val = value.toString();

                            if (MessageProperty.isValidAppProperty(propertyName, val)) {
                                msg.setProperty(propertyName, val);
                            }
                        }
                    } catch (IllegalAccessException e) {
                        System.err.println("Attempted to access private or protected member of class during message conversion.");
                    } catch (InvocationTargetException e) {
                        System.err.println("Exception thrown while attempting to get member variable. See: " + e.getMessage());
                    }
                }
            }
        }
        return msg;
    }

}
