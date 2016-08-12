# MultiTree

## Overview

Description: a multitree is a tree that has for every non-leaf has multiple children.

```code
                                 |---------|
                                 |   ROOT  |
                                 |---------|
                                      |
                   |------------------|--------------------|
                   |                  |                    |
            |------------|      |------------|      |------------|
            | child1(a)  |      | child2(b)  |      | child3     |
            |------------|      |------------|      |------------|
                                                           |
                                              |------------|----------|
                                              |                       |  
                                        |------------|          |------------|
                                        | child4(c)  |          | child5(d)  |
                                        |------------|          |------------|



```
 
Every node of the tree has name of type char*.
Nodes are further classified as "inner nodes" and "leafs". Inner nodes are the nodes that have children. Leafs are the nodes that do not have children.
Leafs have a value attached to them of type "void *".
**SRS_MULTITREE_99_004: [**  MultiTree shall have the following interface: **]**

```c
typedef void* MULTITREE_HANDLE;
 
typedef enum MULTITREE_RESULT_TAG
{
    MULTITREE_OK,
    MULTITREE_INVALID_ARG,
    MULTITREE_ALREADY_HAS_A_VALUE,
    MULTITREE_EMPTY_CHILD_NAME,
    MULTITREE_EMPTY_VALUE,
    MULTITREE_OUT_OF_RANGE_INDEX,
    MULTITREE_ERROR,
    MULTITREE_CHILD_NOT_FOUND
}MULTITREE_RESULT;

typedef void (*MULTITREE_FREE_FUNCTION)(void* value);
typedef int (*MULTITREE_CLONE_FUNCTION)(void** destination, const void* source);
 
extern MULTITREE_HANDLE MultiTree_Create(MULTITREE_CLONE_FUNCTION cloneFunction, MULTITREE_FREE_FUNCTION freeFunction);
extern MULTITREE_RESULT MultiTree_AddLeaf(MULTITREE_HANDLE treeHandle, const char* destinationPath, const void* value);
extern MULTITREE_RESULT MultiTree_AddChild(MULTITREE_HANDLE treeHandle, const char* childName, MULTITREE_HANDLE* childHandle);
extern MULTITREE_RESULT MultiTree_GetChildCount(MULTITREE_HANDLE treeHandle, size_t* count);
extern MULTITREE_RESULT MultiTree_GetChild(MULTITREE_HANDLE treeHandle, size_t index, MULTITREE_HANDLE* childHandle);
extern MULTITREE_RESULT MultiTree_GetChildByName(MULTITREE_HANDLE treeHandle, const char* childName, MULTITREE_HANDLE* childHandle);
extern MULTITREE_RESULT MultiTree_GetName(MULTITREE_HANDLE treeHandle, char* destination, size_t destinationSize);
extern MULTITREE_RESULT MultiTree_GetValue(MULTITREE_HANDLE treeHandle, const void** destination);
extern MULTITREE_RESULT MultiTree_GetLeafValue(MULTITREE_HANDLE treeHandle, const char* leafPath, const void** destination);
extern MULTITREE_RESULT MultiTree_SetValue(MULTITREE_HANDLE treeHandle, void* value);
extern void MultiTree_Destroy(MULTITREE_HANDLE treeHandle);
```

### MultiTree_Create

**SRS_MULTITREE_99_005: [**  MultiTree_Create creates a new tree. **]**

**SRS_MULTITREE_99_052: [**  If any of the arguments passed to MultiTree_Create is NULL, the call shall return NULL. **]**

**SRS_MULTITREE_99_006: [**  MultiTree_Create shall return a non-NULL pointer if the tree has been successfully created. **]**

**SRS_MULTITREE_99_007: [**  MultiTree_Create returns NULL if the tree has not been successfully created. **]**

### MultiTree_AddLeaf

MultiTree_AddLeaf is used to populate the tree with data. 
Parameter treeHandle is a previous tree handle.
DestinationPath is the leaf name to whom the value shall be assigned to.

**SRS_MULTITREE_99_014: [** DestinationPath is a string in the following format: /child1/child12 or child1/child12 **]**

The first "/" designates the root of the tree.
**SRS_MULTITREE_99_017: [**  Subsequent names designate hierarchical children in the tree. The last child designates the child that will receive the value. **]**

**SRS_MULTITREE_99_018: [**  If the treeHandle parameter is NULL, MULTITREE_INVALID_ARG shall be returned. **]**

**SRS_MULTITREE_99_019: [**  If parameter destinationPath is NULL, MULTITREE_INVALID_ARG shall be returned. **]**

**SRS_MULTITREE_99_050: [**  If destinationPath a string with zero characters, MULTITREE_EMPTY_CHILD_NAME shall be returned. **]**

**SRS_MULTITREE_99_020: [**  If parameter value is NULL, MULTITREE_INVALID_ARG shall be returned. **]**

**SRS_MULTITREE_99_021: [**  If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed. **]**

**SRS_MULTITREE_99_022: [**  If a child along the path does not exist, it shall be created. **]**

**SRS_MULTITREE_99_023: [**  The newly created children along the path shall have a NULL value by default. **]**

**SRS_MULTITREE_99_024: [**  if a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned. **]**

**SRS_MULTITREE_99_025: [**  The function shall return MULTITREE_ERROR to indicate any other error not specified here. **]**

**SRS_MULTITREE_99_026: [**  If the function fails to execute, it shall not affect the already stored tree data. **]**

**SRS_MULTITREE_99_034: [**  The function returns MULTITREE_OK when data has been stored in the tree. **]**

### MultiTree_AddChild

**SRS_MULTITREE_99_053: [**  MultiTree_AddChild shall add a new node with the name childName to the multi tree node identified by treeHandle **]**

**SRS_MULTITREE_99_060: [**  The value associated with the new node shall be NULL. **]**

**SRS_MULTITREE_99_062: [**  The new node handle shall be returned in the childHandle argument. **]**

**SRS_MULTITREE_99_054: [**  On success, MultiTree_AddChild shall return MULTITREE_OK. **]**

**SRS_MULTITREE_99_055: [**  If any argument is NULL, MultiTree_AddChild shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_066: [**  If the childName argument is an empty string, MultiTree_AddChild shall return MULTITREE_EMPTY_CHILD_NAME. **]**

**SRS_MULTITREE_99_061: [**  If a child node with the same name already exists, MultiTree_AddChild shall return MULTITREE_ALREADY_HAS_A_VALUE. **]**

### MultiTree_GetChildCount

**SRS_MULTITREE_99_029: [**  This function writes in *count the number of direct children for a tree node specified by the parameter treeHandle **]**

**SRS_MULTITREE_99_027: [**  If treeHandle is NULL, the function returns MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_028: [**  If parameter count is NULL, the function returns MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_035: [**  The function shall return MULTITREE_OK when *count contains the number of children of the node pointed to be parameter treeHandle. **]**

### MultiTree_GetChild

**SRS_MULTITREE_99_030: [**  This function writes in *childHandle parameter the "index"th child of the node pointed to by parameter treeHandle **]**

**SRS_MULTITREE_99_031: [**  If parameter treeHandle is NULL, the function returns MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_032: [**  If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX **]**

**SRS_MULTITREE_99_033: [**  If parameter childHandle is NULL, the function shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_035: [**  The function returns MULTITREE_OK when *childHandle contains a handle to the "index"th child of the tree designated by parameter treeHandle. **]**

### MultiTree_GetChildByName

**SRS_MULTITREE_99_063: [**  MultiTree_GetChildByName shall retrieve the handle of the child node childName from the treeNode node. **]**

**SRS_MULTITREE_99_067: [**  The child node handle shall be returned in the childHandle argument. **]**

**SRS_MULTITREE_99_064: [**  On success, MultiTree_GetChildByName shall return MULTITREE_OK. **]**

**SRS_MULTITREE_99_065: [**  If any argument is NULL, MultiTree_GetChildByName shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_068: [**  If the specified child is not found, MultiTree_GetChildByName shall return MULTITREE_CHILD_NOT_FOUND. **]**

### MultiTree_GetName

**SRS_MULTITREE_99_036: [**  This function fills the buffer pointed to by parameter destination with the name of the root node of the tree designated by parameter treeHandle. **]**

**SRS_MULTITREE_99_037: [**  If treeHandle is NULL, the function shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_038: [**  If destination is NULL, the function shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_039: [**  The function returns MULTITREE_OK when destination contains the name of the root node of the tree designated by treeHandle parameter. **]**

**SRS_MULTITREE_99_040: [**  The function returns MULTITREE_ERROR to indicate any other error. **]**

**SRS_MULTITREE_99_051: [**  The function returns MULTITREE_EMPTY_CHILD_NAME when used with the root of the tree. **]**

### MultiTree_GetValue

**SRS_MULTITREE_99_041: [**  This function updates the *destination parameter to the internally stored value. **]**

**SRS_MULTITREE_99_042: [**  If treeHandle is NULL, the function shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_043: [**  If destination is NULL, the function shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_044: [**  If there is no value in the node then MULTITREE_EMPTY_VALUE shall be returned. **]**

**SRS_MULTITREE_99_045: [**  The function shall return MULTITREE_OK when destination contains the void* value of the root node of the tree designated by treeHandle parameter. **]**

**SRS_MULTITREE_99_046: [**  The function returns MULTITREE_ERROR to indicate any other error. **]**

### MultiTree_SetValue

**SRS_MULTITREE_99_072: [**  MultiTree_SetValue shall set the value of the node indicated by the treeHandle argument to the value of the argument value. **]**

**SRS_MULTITREE_99_073: [**  On success, MultiTree_SetValue shall return MULTITREE_OK. **]**

**SRS_MULTITREE_99_074: [**  If any argument is NULL, MultiTree_SetValue shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_076: [**  If the node already has a value then MultiTree_SetValue shall return MULTITREE_ALREADY_HAS_A_VALUE. **]**

**SRS_MULTITREE_99_075: [**  MultiTree_SetValue shall return MULTITREE_ERROR to indicate any other error. **]**

### MultiTree_GetLeafValue

**SRS_MULTITREE_99_053: [**  MultiTree_GetLeafValue shall copy into the *destination argument the value of the node identified by the leafPath argument. **]**

**SRS_MULTITREE_99_054: [**  On success, MultiTree_GetLeafValue shall return MULTITREE_OK. **]**

**SRS_MULTITREE_99_055: [**  If any argument is NULL, MultiTree_GetLeafValue shall return MULTITREE_INVALID_ARG. **]**

**SRS_MULTITREE_99_056: [**  The leafPath argument is a string in the following format: /child1/child12 or child1/child12. **]**

**SRS_MULTITREE_99_057: [**  Subsequent names designate hierarchical children in the tree. **]**

**SRS_MULTITREE_99_058: [**  The last child designates the child that will receive the value. **]**

**SRS_MULTITREE_99_069: [**  If a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned. **]**

**SRS_MULTITREE_99_071: [**  When the child node is not found, MultiTree_GetLeafValue shall return MULTITREE_CHILD_NOT_FOUND. **]**

**SRS_MULTITREE_99_070: [**  If an attempt is made to get the value for a node that does not have a value set, then MultiTree_GetLeafValue shall return MULTITREE_EMPTY_VALUE. **]**

**SRS_MULTITREE_99_059: [**  MultiTree_GetLeafValue shall return MULTITREE_ERROR to indicate any other error. **]**

### MultiTree_Destroy
**SRS_MULTITREE_99_047: [**  This function frees any system resource used by the tree designated by parameter treeHandle **]**
