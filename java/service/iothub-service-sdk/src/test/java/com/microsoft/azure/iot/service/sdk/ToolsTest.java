/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.util.HashMap;
import java.util.Map;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 * Utility function collection
 */
public class ToolsTest
{
    /**
     * Test of isNullOrEmpty method, of class Tools.
     * Input: null
     * Expected: true
     */
    @Test
    public void testIsNullOrEmptyInputNull()
    {
        String value = null;
        Boolean expResult = true;
        Boolean result = Tools.isNullOrEmpty(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of isNullOrEmpty method, of class Tools.
     * Input: empty string
     * Expected: true
     */
    @Test
    public void testIsNullOrEmptyInputEmpty()
    {
        String value = "";
        Boolean expResult = true;
        Boolean result = Tools.isNullOrEmpty(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of isNullOrEmpty method, of class Tools.
     * Input: valid string
     * Expected: true
     */
    @Test
    public void testIsNullOrEmptyInputNotEmpty()
    {
        String value = "XXX";
        Boolean expResult = false;
        Boolean result = Tools.isNullOrEmpty(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of isNullOrWhiteSpace method, of class Tools.
     * Input: null
     * Expected: true
     */
    @Test
    public void testIsNullOrWhiteSpaceInputNull()
    {
        String value = null;
        Boolean expResult = true;
        Boolean result = Tools.isNullOrWhiteSpace(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of isNullOrWhiteSpace method, of class Tools.
     * Input: white space
     * Expected: true
     */
    @Test
    public void testIsNullOrWhiteSpaceInputWhiteSpace()
    {
        String value = " ";
        Boolean expResult = true;
        Boolean result = Tools.isNullOrWhiteSpace(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of isNullOrWhiteSpace method, of class Tools.
     * Input: white space
     * Expected: true
     */
    @Test
    public void testIsNullOrWhiteSpaceInputWhiteSpaces()
    {
        String value = "   ";
        Boolean expResult = true;
        Boolean result = Tools.isNullOrWhiteSpace(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of isNullOrWhiteSpace method, of class Tools.
     * Input: valid string
     * Expected: false
     */
    @Test
    public void testIsNullOrWhiteSpaceNotEmpty()
    {
        String value = "XXX";
        Boolean expResult = false;
        Boolean result = Tools.isNullOrWhiteSpace(value);
        assertEquals(expResult, result);
    }

    /**
     * Test of getValueStringByKey method, of class Tools.
     * Input: map=null
     * Expect: empty string
     */
    @Test
    public void testGetValueStringByKeyMapNull()
    {
        Map<String, String> map = null;
        String keyName = "";
        String expResult = "";
        String result = Tools.getValueStringByKey(map, keyName);
        assertEquals(expResult, result);
    }

    /**
     * Test of getValueStringByKey method, of class Tools.
     * Input: keyName=null
     * Expect: empty string
     */
    @Test
    public void testGetValueStringByKeyKeyNameNull()
    {
        Map<String, String> map = new HashMap<String, String>();
        String keyName = null;
        String expResult = "";
        String result = Tools.getValueStringByKey(map, keyName);
        assertEquals(expResult, result);
    }

    /**
     * Test of getValueStringByKey method, of class Tools.
     * Input: map=valid map, keyName=valid string
     * Expect: value of the key
     */
    @Test
    public void testGetValueStringByKeyGoodCase()
    {
        HashMap<String, String> map = new HashMap<String, String>();
        map.put("key1", "value1");
        map.put("key2", "value2");
        map.put("key3", "value3");
        String keyName = "key2";
        String expResult = "value2";
        String result = Tools.getValueStringByKey(map, keyName);
        assertEquals(expResult, result);
    }
}
