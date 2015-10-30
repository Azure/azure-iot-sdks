/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import javax.jms.ExceptionListener;
import javax.jms.JMSException;

public class AmqpExceptionListener implements ExceptionListener
{
    @Override
    public void onException(JMSException exception)
    {
        exception.printStackTrace(System.out);
    }

}
