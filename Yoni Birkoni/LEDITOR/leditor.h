// leditor.h : main header file for the LEDITOR application
//

#if !defined(AFX_LEDITOR_H__B17D1B99_9461_4356_988A_0165F69AE920__INCLUDED_)
#define AFX_LEDITOR_H__B17D1B99_9461_4356_988A_0165F69AE920__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLeditorApp:
// See leditor.cpp for the implementation of this class
//

class CLeditorApp : public CWinApp
{
public:
	CLeditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLeditorApp)
	afx_msg void On1Boss1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEDITOR_H__B17D1B99_9461_4356_988A_0165F69AE920__INCLUDED_)
