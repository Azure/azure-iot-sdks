#Azure-iothub.QueryExpression Requirements

##Overview
A `QueryExpression` object is used as a parameter to the `Registry.queryDevices` method to query the registry for devices with specific properties.

## Example
```json
{
  "filter": {
    "logicalOperator": "and",
    "filters": [
      {
        "property": {
          "name": "Manufacturer",
          "type": "device"
        },
        "value": "Contoso",
        "comparisonOperator": "gt",
        "type": "comparison"
      },
      {
        "logicalOperator": "or",
        "filters": [
          {
            "property": {
              "name": "FirmwareVersion",
              "type": "device"
            },
            "value": 5,
            "comparisonOperator": "lt",
            "type": "comparison"
          },
          {
            "property": {
              "name": "FirmwareVersion",
              "type": "device"
            },
            "value": 1,
            "comparisonOperator": "gt",
            "type": "comparison"
          }
        ],
        "type": "logical"
      }
    ],
    "type": "logical"
  },
  "project": null,
  "aggregate": null,
  "sort": null
}
```

##QueryExpression properties
The `QueryExpression` object must have these 4 properties:
- `project` is akin to a simple `SELECT` clause in SQL or type `ProjectionExpression`.
- `aggregate` is akin to a complex `SELECT` clause with aggregation operations such as `AVG` and must be of type `AggregationExpression`.
- `filter` is akin to a `WHERE` clause in SQL and must be of type `FilterExpression`.
- `sort` is akin to a `SORTBY` clause in SQL and must be of type `SortExpression` or an array of `SortExpression` objects.

If `project` is set, aggregate must be `null`.
If `aggregate` is set, project must be `null`.

If `sort` is an array of `SortExpression` it cannot contain `null` values.

## ProjectionExpression properties
A `ProjectionExpression` object must have 2 properties:
- `all` is a boolean indicating whether the query should return all properties of the device, and can be set to true or false.
- `properties` is an array of `QueryProperty` objects.

- if `all` is true the `properties` array is ignored.
- if `all` is false the `properties` array must have at least one element.
- if `all` is false the `properties` array must not contain any `null` values.

## AggregationExpression properties
An `AggregationExpression` object must have 2 properties:
- `keys` is an array of `QueryProperty` objects.
- `properties` is an array of `AggregationProperty` objects.

There must be at least one `QueryProperty` object in the `keys` array.
There must be at least one `AggregationProperty` object in the `properties` array.
the `keys` array cannot contain null values.
the `properties` array cannot contain null values.

### AggregationProperty properties
An `AggregationProperty` object must have 3 properties:
- `operator` must be one of the following strings: 
  - `sum` will produce the summation of all `QueryProperty` values
  - `avg` will produce the average of all `QueryProperty` values
  - `count` will produce the number of all `QueryProperty` values
  - `min` will produce the minimum value in the `QueryProperty` values
  - `max` will produce the maximum value in the `QueryProperty` values
- `property` must be of a `QueryProperty` object.
- `columnName` must be a string that will be used as the name of the resulting column in the results.

If `operator` is not `count` the `property` property cannot be `null`.
`columnName` cannot be `undefined`, `null` or an empty string.
`columnName` must not contain the `$` reserved character.

## FilterExpression properties
A `FilterExpression` object can be either of type `LogicalExpression` or `ComparisonExpression`.

### LogicalExpression properties
A `LogicalExpression` object must have 3 properties:
- `type` must be the following string: `logical`.
- `logicalOperator` is the logical operation that must be applied to the operands and is one of the following strings: `and`, `or`, `not`.
- `filters` is an array of `FilterExpression`.

The `filters` array must contain at least 2 elements if the operatior is `and` or `or`.
The `filters` array must contain only one element if the operator is `not`.
The `filters` array must not have any `null` value.

### ComparisonExpression properties
A `ComparisonExpression` object must have 3 properties
- `type` must be the following string: `comparison`.
- `property` is the property that should be evaluated and must be of type `QueryProperty`.
- `value` is the value or array of values against which the property value will be evaluated using the specified operator.
- `comparisonOperator` is the comparison operator that must be applied to the operands and is one of the following strings: 
  - `eq`: the property value must be equal to the reference value.
  - `ne`: the property value must not be equal to the reference value.
  - `gt`: the property value must be greater than the reference value.
  - `gte`: the property value must be greater or equal to the reference value.
  - `lt`: the property value must be less than the reference value.
  - `lte`: the property value must be less than or equal to the reference value.
  - `in`: the property value must be contained in the reference values.
  - `nin`: the property value must not be contained in the reference values.
  - `all`: the property value must match all the reference values.

- if `comparisonOperator` is one of `in`, `nin`, or `all`, the `value` property must be an array of values.
- the `property` property cannot be null.
- if the `type` property of `property` is `default` and the `name` of the property is `tags` (case-insensitive) then the `comparisonOperator` must be `all`.

## SortExpression properties
A `SortExpression` object must have 2 properties:
- `order` must be one of the following strings: `asc` or `desc`.
- `property` is the `QueryProperty` that should be used to sort results.

`property` cannot be `undefined` or `null`.

## QueryProperty properties
A `QueryProperty` object must have 2 properties:
- `type` is the type of the property and must be one of the following strings: `device`, `service`, `default`, `aggregated`.
- `name` is the property name and must be a `string`.