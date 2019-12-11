// vSMR.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "vSMR.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CvSMRApp

BEGIN_MESSAGE_MAP(CvSMRApp, CWinApp)
END_MESSAGE_MAP()


// CvSMRApp construction

CvSMRApp::CvSMRApp()
{

}


// The one and only CvSMRApp object

CvSMRApp theApp;


// CvSMRApp initialization

BOOL CvSMRApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//---EuroScopePlugInInit-----------------------------------------------

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		// create the instance
		* ppPlugInInstance = theApp.gpMyPlugin = new CSMRPlugin();
}
