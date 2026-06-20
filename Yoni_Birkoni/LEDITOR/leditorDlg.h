// leditorDlg.h : header file
//

#if !defined(AFX_LEDITORDLG_H__8139B881_CA8E_48E3_AFDF_4DF8D4B58C77__INCLUDED_)
#define AFX_LEDITORDLG_H__8139B881_CA8E_48E3_AFDF_4DF8D4B58C77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLeditorDlg dialog

class CLeditorDlg : public CDialog
{
// Construction
public:
	void state (int but);
	CLeditorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLeditorDlg)
	enum { IDD = IDD_LEDITOR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLeditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton10();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButton11();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnCancel();
	afx_msg void OnButton1();
	virtual void OnOK();
	afx_msg void On1Monster1();
	afx_msg void On1Monster2();
	afx_msg void On1Monster3();
	afx_msg void On1Monster4();
	afx_msg void On1Monster5();
	afx_msg void On1Monster6();
	afx_msg void OnButton9();
	afx_msg void On1Monster7();
	afx_msg void On1Boss1();
	afx_msg void On1Master1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEDITORDLG_H__8139B881_CA8E_48E3_AFDF_4DF8D4B58C77__INCLUDED_)
