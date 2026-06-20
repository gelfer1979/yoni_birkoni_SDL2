// leditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "leditor.h"
#include "leditorDlg.h"
#include "graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const np=99;//number of video buffers(pictures)
const mnum=6;//number of existing monsters
char board[19][26];//board for playing
int nenemys=0;//count of enemys
int z[np]; //array of pictures
int cel=0;//current element
int ix=1,iy=1;//packman coordinates
char fname[1024];
int mcount;//count of monsters in the table
bool chng=false;//if board was changed
CMenu mn;
int ax,ay;
/////////////////////////////////////////////////////////////////////////////
// CLeditorDlg dialog

CLeditorDlg::CLeditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLeditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLeditorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLeditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLeditorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLeditorDlg, CDialog)
	//{{AFX_MSG_MAP(CLeditorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON11, OnButton11)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_COMMAND(ID_1_MONSTER1, On1Monster1)
	ON_COMMAND(ID_1_MONSTER2, On1Monster2)
	ON_COMMAND(ID_1_MONSTER3, On1Monster3)
	ON_COMMAND(ID_1_MONSTER4, On1Monster4)
	ON_COMMAND(ID_1_MONSTER5, On1Monster5)
	ON_COMMAND(ID_1_MONSTER6, On1Monster6)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_COMMAND(ID_1_MONSTER7, On1Monster7)
	ON_COMMAND(ID_1_BOSS1, On1Boss1)
	ON_COMMAND(ID_1_MASTER1, On1Master1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeditorDlg message handlers

void loadfl()
{
		CFile fl;
	if (fl.Open(fname,CFile::modeRead))
	{
		fl.Read(board,sizeof(board));
		fl.Close();
	}
}

BOOL CLeditorDlg::OnInitDialog()
{
	//-----------
	strcpy(fname,"1.dat");
	//-----------

	//mn.CreatePopupMenu();
	mn.LoadMenu(IDR_MENU1);
	board[1][1]=5;
	loadfl();
	// graphics initialization
	if (!initgraph(this->m_hWnd,800,600,16,0,np))
		MessageBox("Initialization error!!!");
	// load elements pictures
	z[0]=loadbmptoptr("lback.bmp");
	z[1]=loadbmptoptr("wall.bmp");
	z[2]=loadbmptoptr("eat.bmp");
	z[3]=loadbmptoptr("pchang.bmp");
	z[4]=loadbmptoptr("backim.bmp");
	z[5]=loadbmptoptr("i.bmp");
	z[9]=loadbmptoptr("heart.bmp");
	z[20]=loadbmptoptr("monster1.bmp");
	z[21]=loadbmptoptr("monster2.bmp");
	z[22]=loadbmptoptr("monster3.bmp");
	z[23]=loadbmptoptr("monster4.bmp");
	z[24]=loadbmptoptr("monster5.bmp");
	z[25]=loadbmptoptr("monster6.bmp");
	z[26]=loadbmptoptr("monster7.bmp");
	z[40]=loadbmptoptr("boss1.bmp");
	z[90]=loadbmptoptr("master1.bmp");

	HBITMAP z = (HBITMAP)LoadImage(NULL,"lback.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON2))->SetBitmap(z);
	z = (HBITMAP)LoadImage(NULL,"wall.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON3))->SetBitmap(z);
	z = (HBITMAP)LoadImage(NULL,"eat.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON4))->SetBitmap(z);
	z = (HBITMAP)LoadImage(NULL,"i.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON10))->SetBitmap(z);
	z = (HBITMAP)LoadImage(NULL,"pchang.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON5))->SetBitmap(z);
	z = (HBITMAP)LoadImage(NULL,"monster1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON6))->SetBitmap(z);
	z = (HBITMAP)LoadImage(NULL,"heart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CButton*)GetDlgItem(IDC_BUTTON11))->SetBitmap(z);
	//set keys on her places
	GetDlgItem(IDC_BUTTON2)->MoveWindow(1,0,55,27);
	GetDlgItem(IDC_BUTTON3)->MoveWindow(60,0,55,27);
	GetDlgItem(IDC_BUTTON4)->MoveWindow(120,0,55,27);
	GetDlgItem(IDC_BUTTON10)->MoveWindow(180,0,58,27);
	GetDlgItem(IDC_BUTTON5)->MoveWindow(240,0,58,27);
	GetDlgItem(IDC_BUTTON6)->MoveWindow(300,0,65,27);
	GetDlgItem(IDC_BUTTON11)->MoveWindow(368,0,45,27);
	GetDlgItem(IDC_BUTTON1)->MoveWindow(600,0,45,27);
	GetDlgItem(IDC_BUTTON9)->MoveWindow(650,0,45,27);
	GetDlgItem(IDOK)->MoveWindow(700,0,95,27);

	((CButton*)GetDlgItem(IDC_BUTTON2))->SetState(true);
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLeditorDlg::OnPaint() 
{
	for (int i=0;i<19;i++)
			for (int i1=0;i1<26;i1++)
				putsprite(i1*30+10,i*30+30,z[0],0);
		mcount=0;
		for (i=0;i<19;i++)
			for (int i1=0;i1<26;i1++)
			{
				if (board[i][i1]==5)
				{
					ix=i1;iy=i;
				}
				if ((board[i-1][i1]<40)&&(board[i-1][i1-1]<40)&&(board[i][i1-1]<40)&&\
					(board[i][i1-2]<70)&&(board[i-1][i1-2]<70)&&(board[i-2][i1-2]<70)&&\
					(board[i-2][i1-1]<70)&&(board[i-2][i1]<70)&&(board[i][i1]>0))
				putsprite(i1*30+10,i*30+30,z[board[i][i1]],0);
				if (board[i][i1]>19) mcount++;
			}


	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLeditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLeditorDlg::OnButton2() 
{
	state(IDC_BUTTON2);
	cel=0;
}

void CLeditorDlg::OnButton3() 
{
	state(IDC_BUTTON3);
	cel=1;
}

void CLeditorDlg::OnButton4() 
{
	state(IDC_BUTTON4);
	cel=2;		
}

void CLeditorDlg::OnButton5() 
{
	state(IDC_BUTTON5);
	cel=3;			
}

void CLeditorDlg::OnButton6() 
{
	mn.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,302,20,this);
}

void CLeditorDlg::OnButton10() 
{
	state(IDC_BUTTON10);
	cel=5;			
}

void CLeditorDlg::OnButton11() 
{
	state(IDC_BUTTON11);
	cel=9;						
}

void CLeditorDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ((point.x>10)&&(point.x<790)&&(point.y>30)&&(point.y<600))
	{
		ax=(point.x-10)/30;ay=(point.y-30)/30;
		if (board[ay][ax]>=40)
			{
				board[ay+1][ax]=0;
				board[ay][ax+1]=0;
				board[ay+1][ax+1]=0;
				if (board[ay][ax]>=70)
				{
					board[ay][ax+2]=0;
					board[ay+1][ax+2]=0;
					board[ay+2][ax+2]=0;
					board[ay+2][ax+1]=0;
					board[ay+2][ax]=0;
				}
				board[ay][ax]=0;
			}	
		else if (board[ay-1][ax]>=40)
		{
				board[ay][ax]=0;
				board[ay][ax+1]=0;
				board[ay-1][ax+1]=0;
				if (board[ay-1][ax]>=70)
				{
					board[ay][ax+2]=0;
					board[ay+1][ax+2]=0;
					board[ay-1][ax+2]=0;
					board[ay+1][ax+1]=0;
					board[ay+1][ax]=0;
				}
				board[ay-1][ax]=0;
		}
		else if (board[ay-1][ax-1]>=40)
		{
				board[ay][ax]=0;
				board[ay-1][ax]=0;
				board[ay][ax-1]=0;
				if (board[ay-1][ax-1]>=70)
				{
					board[ay][ax+1]=0;
					board[ay+1][ax+1]=0;
					board[ay-1][ax+1]=0;
					board[ay+1][ax]=0;
					board[ay+1][ax-1]=0;
				}
				board[ay-1][ax-1]=0;
		}
		else if (board[ay][ax-1]>=40)
		{
				board[ay][ax]=0;
				board[ay+1][ax-1]=0;
				board[ay+1][ax]=0;
				if (board[ay][ax-1]>=70)
				{
					board[ay][ax+1]=0;
					board[ay+1][ax+1]=0;
					board[ay+2][ax+1]=0;
					board[ay+2][ax]=0;
					board[ay+2][ax-1]=0;
				}
				board[ay][ax-1]=0;
		}
		else if (board[ay][ax-2]>=70)
		{
				board[ay][ax]=0;
				board[ay+1][ax-1]=0;
				board[ay+1][ax]=0;
				board[ay][ax-1]=0;

				board[ay][ax-2]=0;
				board[ay+1][ax-2]=0;
				board[ay+2][ax-2]=0;
				board[ay+2][ax]=0;
				board[ay+2][ax-1]=0;

		}
		else if (board[ay-1][ax-2]>=70)
		{
				board[ay][ax]=0;
				board[ay+1][ax-1]=0;
				board[ay+1][ax]=0;
				board[ay][ax-1]=0;

				board[ay][ax-2]=0;
				board[ay+1][ax-2]=0;
				board[ay-1][ax-2]=0;
				board[ay-1][ax]=0;
				board[ay-1][ax-1]=0;

		}
		else if (board[ay-2][ax-2]>=70)
		{
				board[ay][ax]=0;
				board[ay-1][ax-1]=0;
				board[ay-1][ax]=0;
				board[ay][ax-1]=0;

				board[ay][ax-2]=0;
				board[ay-2][ax-2]=0;
				board[ay-1][ax-2]=0;
				board[ay-2][ax]=0;
				board[ay-2][ax-1]=0;
		}
		else if (board[ay-2][ax-1]>=70)
		{
				board[ay][ax]=0;
				board[ay-1][ax-1]=0;
				board[ay-1][ax]=0;
				board[ay][ax-1]=0;

				board[ay][ax+1]=0;
				board[ay-2][ax+1]=0;
				board[ay-1][ax+1]=0;
				board[ay-2][ax]=0;
				board[ay-2][ax-1]=0;
		}
		else if (board[ay-2][ax]>=70)
		{
				board[ay][ax]=0;
				board[ay-1][ax+1]=0;
				board[ay-1][ax]=0;
				board[ay][ax+1]=0;

				board[ay][ax+2]=0;
				board[ay-2][ax+1]=0;
				board[ay-1][ax+2]=0;
				board[ay-2][ax]=0;
				board[ay-2][ax+2]=0;
		}


		if (cel>=40)
		{//put big monster on the map
			//if have more than 20 monsters- dont put monster on the map
			if (mcount>=20) return;
			//if monster 2x2 out of screen - return
			if ((ax>24)||(ay>17)) return;
			//if monster 3x3 out of screen - return
			if (cel>=70) {if ((ax>23)||(ay>16)) return;}

			//if monster 2x2 set on packman - return
			if ((board[ay][ax]==5)||(board[ay+1][ax]==5)||\
				(board[ay][ax+1]==5)||(board[ay+1][ax+1]==5)) return;
			//if monster 3x3 set on packman - return
			if (cel>=70) if ((board[ay][ax+2]==5)||(board[ay+1][ax+2]==5)||\
				(board[ay+2][ax+2]==5)||(board[ay+2][ax+1]==5)||(board[ay+2][ax]==5)) return;

			//if monster already exist on this position++ -clear him
			if (board[ay+1][ax-1]>=40) board[ay+1][ax-1]=0;
			if (board[ay][ax-1]>=40) board[ay][ax-1]=0;
			if (board[ay-1][ax-1]>=40) board[ay-1][ax-1]=0;
			if (board[ay-1][ax]>=40) board[ay-1][ax]=0;
			if (board[ay-1][ax+1]>=40) board[ay-1][ax+1]=0;
			//--------------
			if (board[ay+1][ax-2]>=70) board[ay+1][ax-2]=0;
			if (board[ay][ax-2]>=70) board[ay][ax-2]=0;
			if (board[ay-1][ax-2]>=70) board[ay-1][ax-2]=0;
			if (board[ay-2][ax-2]>=70) board[ay-2][ax-2]=0;
			if (board[ay-2][ax-1]>=70) board[ay-2][ax-1]=0;
			if (board[ay-2][ax]>=70) board[ay-2][ax]=0;
			if (board[ay-2][ax+1]>=70) board[ay-2][ax+1]=0;
			//--------------
			if (cel>=70) 
			{
				if (board[ay+2][ax-1]>=70) board[ay+2][ax-1]=0;
				if (board[ay+2][ax-2]>=70) board[ay+2][ax-2]=0;
				if (board[ay-2][ax+2]>=70) board[ay-2][ax+2]=0;
				if (board[ay-1][ax+2]>=70) board[ay-1][ax+2]=0;
			}
				//--------------------------------
				if (board[ay][ax]>19) mcount--;
				if (board[ay+1][ax]>19) mcount--;
				if (board[ay][ax+1]>19) mcount--;
				if (board[ay+1][ax+1]>19) mcount--;
				board[ay][ax]=cel;
				board[ay][ax+1]=0;
				board[ay+1][ax]=0;
				board[ay+1][ax+1]=0;
				if (cel>=70) 
				{
					if (board[ay][ax+2]>19) mcount--;
					if (board[ay+1][ax+2]>19) mcount--;
					if (board[ay+2][ax+2]>19) mcount--;
					if (board[ay+2][ax+1]>19) mcount--;
					if (board[ay+2][ax]>19) mcount--;
					board[ay][ax+2]=0;
					board[ay+1][ax+2]=0;
					board[ay+2][ax+2]=0;
					board[ay+2][ax+1]=0;
					board[ay+2][ax]=0;
				}
				mcount++;
		}
		else if (cel==5)
		{
			board[iy][ix]=0;
			ix=ax;iy=ay;
			board[ay][ax]=cel;
		} else if(board[ay][ax]!=5)
		{
			if ((cel<19)||(mcount<20)) 
			{
			 if (cel>19) mcount++;
			 if (board[ay][ax]>19) mcount--;
			 board[ay][ax]=cel;
			}
		}
	}
	chng=true;
	OnPaint();
	CDialog::OnLButtonDown(nFlags, point);
}



void CLeditorDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if ((nFlags&MK_LBUTTON)!=0) 
	{
		int ox=(point.x-10)/30;int oy=(point.y-30)/30;	
		if ((ox!=ax)||(oy!=ay)) OnLButtonDown(nFlags, point);
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CLeditorDlg::OnCancel() 
{

}

void CLeditorDlg::OnButton1() 
{
	//save board
			CFile fl;
			try{
	if (fl.Open(fname,CFile::modeCreate|CFile::modeWrite))
	{
		fl.Write(board,sizeof(board));
		fl.Close();
		chng=false; 
	}
			}catch (int)
				{
				 MessageBox("Board coul not be saved!","Error",MB_ICONERROR|MB_OK);
				}
}

void CLeditorDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (chng)
	{
		if (MessageBox("Do you want to SAVE changes?",\
			"Cancel",MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON1)==IDYES)
			OnButton1();
	}
	closegraph();
	CDialog::OnOK();
}
void CLeditorDlg::state(int but)
{
	((CButton*)GetDlgItem(IDC_BUTTON2))->SetState(false);
	((CButton*)GetDlgItem(IDC_BUTTON3))->SetState(false);
	((CButton*)GetDlgItem(IDC_BUTTON4))->SetState(false);
	((CButton*)GetDlgItem(IDC_BUTTON5))->SetState(false);
	((CButton*)GetDlgItem(IDC_BUTTON10))->SetState(false);
	((CButton*)GetDlgItem(IDC_BUTTON11))->SetState(false);
	if (but>0) ((CButton*)GetDlgItem(but))->SetState(true);
}

void CLeditorDlg::On1Monster1() 
{
	state(0);
	cel=20;				
}

void CLeditorDlg::On1Monster2() 
{
	state(0);
	cel=21;				
}

void CLeditorDlg::On1Monster3() 
{
	state(0);
	cel=22;
}

void CLeditorDlg::On1Monster4() 
{
	state(0);
	cel=23;
}

void CLeditorDlg::On1Monster5() 
{
	state(0);
	cel=24;
}

void CLeditorDlg::On1Monster6() 
{
	state(0);
	cel=25;
}

void CLeditorDlg::On1Monster7() 
{
	state(0);
	cel=26;
}

void CLeditorDlg::OnButton9() 
{
		if (chng)
	{
		if (MessageBox("Do you want to SAVE changes?",\
			"Cancel",MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON1)==IDYES)
			OnButton1();
	}
	char zb[1024];
	CFileDialog zz(true);
	zz.m_ofn.lpstrFilter="*.dat";
	GetCurrentDirectory(sizeof(zb),zb);
	zz.m_ofn.lpstrInitialDir=zb;
	zz.m_ofn.lpstrFile=fname;
	if (zz.DoModal()==IDOK)
	{
		strcpy(fname,zz.GetPathName().GetBuffer(1024));
		loadfl();
		chng=false;

	}
	OnPaint();

}

void CLeditorDlg::On1Boss1() 
{
	state(0);
	cel=40;
}

void CLeditorDlg::On1Master1() 
{
	state(0);
	cel=90;
}
