# serializer_int tests

serializer_int are integration tests for serializer. These tests will usually: 
1. receive a JSON / test produced data structures from JSON
2. set data structures to values and produce a JSON out of them

The below table summarizes the extend of the tests. Columns are the major component tested, rows are the place of test. 
Cell contain the test name. All these are positive test cases (that is, no bad data offered as input).

| where\what               | WITH_DATA                             | WITH_REPORTED_PROPERTY                             | WITH_DESIRED_PROPERTY                             | WITH_ACTION                     |
|--------------------------|---------------------------------------|----------------------------------------------------|---------------------------------------------------|---------------------------------|
| ROOT MODEL               | WITH_DATA_IN_ROOT_MODEL               | WITH_REPORTED_PROPERTY_IN_ROOT_MODEL               | WITH_DESIRED_PROPERTY_IN_ROOT_MODEL               | WITH_ACTION_IN_ROOT_MODEL       |
| STRUCT_IN_ROOT_MODEL     | WITH_DATA_IN_STRUCT_IN_ROOT_MODEL     | WITH_REPORTED_PROPERTY_IN_STRUCT_IN_ROOT_MODEL     | WITH_DESIRED_PROPERTY_IN_STRUCT_IN_ROOT_MODEL     | na(structs cannot have actions) |
| MODEL_IN_MODEL           | WITH_DATA_IN_MODEL_IN_MODEL           | WITH_REPORTED_PROPERTY_IN_MODEL_IN_MODEL           | WITH_DESIRED_PROPERTY_IN_MODEL_IN_MODEL           | WITH_ACTION_IN_MODEL_IN_MODEL   |
| STRUCT_IN_MODEL_IN_MODEL | WITH_DATA_IN_STRUCT_IN_MODEL_IN_MODEL | WITH_REPORTED_PROPERTY_IN_STRUCT_IN_MODEL_IN_MODEL | WITH_DESIRED_PROPERTY_IN_STRUCT_IN_MODEL_IN_MODEL | na(structs cannot have actions) |

Other tests seek to prove that properties with the same name found in different models can compile (no other checking).