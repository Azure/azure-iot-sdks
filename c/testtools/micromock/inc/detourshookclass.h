// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DETOURSHOOKCLASS_H
#define DETOURSHOOKCLASS_H

#pragma once

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifndef DISABLE_DETOURS

#include "stdafx.h"

typedef struct HOOK_FUNCTION_DATA_TAG
{
    const void* originalFunction;
    const void* whoWillBeCalled;
} HOOK_FUNCTION_DATA;

typedef std::map<const void*, HOOK_FUNCTION_DATA> HOOKED_FUNCTIONS_MAP;

class CDetoursHookClass
{
public:
    virtual ~CDetoursHookClass();

    void InstallHook(_In_ const void* functionToHook, _In_ const void* whoWillBeCalled);
    void UninstallHook(_In_ const void* functionToHook, _In_ const void* whoWillBeCalled);
    void UninstallAllHooks();

protected:
    HOOKED_FUNCTIONS_MAP m_HookedFunctionsToOriginals;
};

#endif // DISABLE_DETOURS

#endif // DETOURSHOOKCLASS_H
