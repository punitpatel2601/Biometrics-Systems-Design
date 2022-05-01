// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FC844F48_1C95_11D2_87EB_00A0C9C9343F__INCLUDED_)
#define AFX_STDAFX_H__FC844F48_1C95_11D2_87EB_00A0C9C9343F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define UNICODE
#define _UNICODE

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <atlbase.h>
#include <comdef.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

#include <atlcom.h>
#include <limits.h>
#include <atlhost.h>

#include "resource.h"

inline void _com_test_error(HRESULT hr) {
	if (FAILED(hr))
		_com_issue_error(hr);
}

#include "dpRCodes.h"
#include "dpDefs.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FC844F48_1C95_11D2_87EB_00A0C9C9343F__INCLUDED_)
