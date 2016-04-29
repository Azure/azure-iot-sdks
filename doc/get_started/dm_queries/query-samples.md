---
platform: windows, Android, iOS
device: desktop, phone, tablet
language: csharp, json
---

Device Management Query Sample Snippets
===
---

# Table of Contents

* [Introduction](#Introduction)
* [Device Queries](#DeviceQueries)
 * [Filter over a Device Property](#device-query-filter-deviceprop)
 * [Filter over a Device property (not-equals)](#device-query-filter-deviceprop-ne)
 * [Filter over a Service property](#device-query-filter-serviceprop)
 * [Filter over a Service property with aggregates](#device-query-filter-serviceprop-aggregates)
 * [Order-by](#device-query-order-by)
* [Device Job Queries](#DeviceJobQueries)
 * [Filter jobs by status](#jobs-query-filter-status)
 * [Order jobs by start time](#jobs-query-orderby)
* [Paging](#Paging)

<a name="Introduction"></a>
# Introduction

**About this document**

This document contains sample JSON snippets for various device and job queries that can be performed via the device management public preview APIs.

Note that for csharp the snippets have to be modified to escape all the double quote characters. This is as simple as pasting in the cs code file and replacing every double quote character with two double quotes instead of one. I.e, " -> ""

<a name="DeviceQueries"></a>
# Device Queries

<a name="device-query-filter-deviceprop"></a>
**Filter over a Device Property**

Get all devices where the FirmwareVersion Device property is set to 1.0

``` js
{
    "filter": {
        "property": {
            "name": "FirmwareVersion",
            "type": "device"
        },
        "value": "1.0",
        "comparisonOperator": "eq",
        "type": "comparison"
    }
}
```

<a name="device-query-filter-deviceprop-ne"></a>
**Filter over a Device property (not-equals)**

Get all devices where the FirmwareVersion Device property is **not** set to 1.0

```js
{
    "filter": {
        "property": {
            "name": "FirmwareVersion",
            "type": "device"
        },
        "value": "1.0",
        "comparisonOperator": "ne",
        "type": "comparison"
    }
}
```

<a name="device-query-filter-serviceprop"></a>
**Filter over a Service property**

Get all devices where CustomerId Service property is set to 123456

```js
{
    "filter": {
        "property": {
            "name": "CustomerId",
            "type": "service"
        },
        "value": "123456",
        "comparisonOperator": "eq",
        "type": "comparison"
    }
}
```

<a name="device-query-filter-serviceprop-aggregates"></a>
**Filter over a Service property with aggregates**

Group by the CustomerId Service property and get sum of Weight Service property for all devices where CustomerId Service property is present.

```js
{
    "filter": {
        "property": {
            "name": "CustomerId",
            "type": "service"
        },
        "value": null,
        "comparisonOperator": "ne",
        "type": "comparison"
    },
    "aggregate": {
        "keys": [
            {
                "name": "CustomerId",
                "type": "service"
            }
        ],
        "properties": [
            {
                "operator": "sum",
                "property": {
                    "name": "Weight",
                    "type": "service"
                },
                "columnName": "TotalWeight"
            }
        ]
    },
    "sort": []
}
```

<a name="device-query-order-by"></a>
**Order-by**

Return all devices ordered by the QoS Service property.

```js
{
    "sort": [
        {
            "property": {
                "name": "QoS",
                "type": "service"
            },
            "order": "asc"
        }
    ]
}
```

<a name="DeviceJobQueries"></a>
# Device Job Queries

<a name="jobs-query-filter-status"></a>
**Filter jobs by status**

Get all device jobs where status of the job is set to Running.

``` js
{
    "filter": {
        "property": {
            "name": "Status",
            "type": "default"
        },
        "value": "Running",
        "comparisonOperator": "eq",
        "type": "comparison"
    }
}
```

<a name="jobs-query-orderby"></a>
**Order jobs by start time**

Get all device jobs ordered by StartTimeUtc

``` js
{
    "sort": [
        {
            "property": {
                "name": "StartTimeUtc",
                "type": "default"
            },
            "order": "desc"
        }
    ]
}
```

<a name="Paging"></a>
# Paging

All query results are limited to 100 objects. Paging needs to be done on the client side.

Get Page 1 of all device jobs where parent jobid is '123456'.

``` js
{
    "filter": {
        "property": {
            "name": "ParentJobId",
            "type": "default"
        },
        "value": "123456"
        "comparisonOperator": "eq",
        "type": "comparison"
    }
}
```

For non-aggregate queries, all results are automatically sorted by either DeviceId or JobId.
To get Page 2, reissue the same query with an additional filter where JobId > JobId of the last object in the previous result.

``` js
{
    "filter": {
        "filters": [
          {
              "property": {
                  "name": "ParentJobId",
                  "type": "default"
              },
              "value": "123456"
              "comparisonOperator": "eq",
              "type": "comparison"
          },
          {
              "property": {
                  "name": "JobId",
                  "type": "default"
              },
              "value": "{JobId of last object in previous result}"
              "comparisonOperator": "gt",
              "type": "comparison"
          }
        ],
        "logicalOperator": "and",
        "type": "logical"
    }
}
```
