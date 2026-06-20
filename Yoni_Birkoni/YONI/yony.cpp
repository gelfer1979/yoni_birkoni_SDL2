// packman.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "yony.h"
#include "yonyDlg.h"
//#include <wtypes.h> 


BEGIN_MESSAGE_MAP(CPackApp, CWinApp)
	// ваши сообщения здесь, если есть
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackApp


/////////////////////////////////////////////////////////////////////////////
// CPackApp construction

CPackApp::CPackApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPackApp object

CPackApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPackApp initialization

BOOL CPackApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.



	CPackmanDlg dlg;
	m_pMainWnd = &dlg;
	
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
