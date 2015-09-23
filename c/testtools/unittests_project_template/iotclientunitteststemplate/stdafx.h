// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently


#pragma once

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Windows Platform headers and control defines
#define STRICT
#define _WIN32_WINNT 0x0500 // Visual Studio requires Windows 2000 or better
#define NOMINMAX // Windows Platform min and max macros cause problems for the Standard C++ Library
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from the Windows Platform headers

// ATL headers and control defines
#define _ATL_APARTMENT_THREADED
#define _ATL_REGISTER_PER_USER

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atlfile.h>
#include <atlsafe.h>

// Windows Platform headers
#include <windowsx.h> // REVIEW - what is this for?


// Visual Studio Platform headers
#include <dte.h> // for extensibility
#include <objext.h> // for ILocalRegistry
#include <vshelp.h> // for Help
#include <uilocale.h> // for IUIHostLocale2
#include <IVsQueryEditQuerySave2.h> // for IVsQueryEditQuerySave2
#include <vbapkg.h> // for IVsMacroRecorder
#include <fpstfmt.h> // for IPersistFileFormat
#include <VSRegKeyNames.h>
#include <stdidcmd.h>
#include <stdiduie.h> // For status bar consts.
#include <textfind.h>
#include <textmgr.h>

// VSL headers
#define VSLASSERT _ASSERTE
#define VSLASSERTEX(exp, szMsg) _ASSERT_BASE(exp, szMsg)
#define VSLTRACE ATLTRACE

#include <VSLPackage.h>
#include <VSLCommandTarget.h>
#include <VSLWindows.h>

#include <VSLFile.h>
#include <VSLContainers.h>
#include <VSLComparison.h>
#include <VSLAutomation.h>
#include <VSLFindAndReplace.h>
#include <VSLShortNameDefines.h>

using namespace VSL;

