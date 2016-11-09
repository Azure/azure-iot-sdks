// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define DList_InitializeListHead real_DList_InitializeListHead
#define DList_IsListEmpty real_DList_IsListEmpty
#define DList_InsertTailList real_DList_InsertTailList
#define DList_InsertHeadList real_DList_InsertHeadList
#define DList_AppendTailList real_DList_AppendTailList
#define DList_RemoveEntryList real_DList_RemoveEntryList
#define DList_RemoveHeadList real_DList_RemoveHeadList

#define GBALLOC_H

#include "doublylinkedlist.c"
