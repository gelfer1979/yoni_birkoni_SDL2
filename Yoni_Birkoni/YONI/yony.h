// packman.h : main header file for the PACKMAN application
//

#if !defined(AFX_PACKMAN_H__C91C51E1_B47D_41DF_A920_DDFC7EF78340__INCLUDED_)
#define AFX_PACKMAN_H__C91C51E1_B47D_41DF_A920_DDFC7EF78340__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPackApp:
// See packman.cpp for the implementation of this class
//

class CPackApp : public CWinApp
{
public:
	CPackApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPackApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKMAN_H__C91C51E1_B47D_41DF_A920_DDFC7EF78340__INCLUDED_)
