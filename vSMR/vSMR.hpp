// vSMR.h : main header file for the vSMR DLL
//

#pragma once
#include "EuroScopePlugIn.h"
#include "SMRPlugin.hpp"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CvSMRApp
// See vSMR.cpp for the implementation of this class
//

class CvSMRApp : public CWinApp
{
public:
	CvSMRApp();

// Overrides
public:
	virtual BOOL InitInstance();
	CSMRPlugin * gpMyPlugin = NULL;
	DECLARE_MESSAGE_MAP()
};
