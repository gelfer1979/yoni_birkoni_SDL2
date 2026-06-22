// packmanDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxtempl.h>

extern "C" char *getcwd(char *buf, size_t size);
#include "yony.h"
#include "yonyDlg.h"
#include "sdl/GRAPH.H"
#include "sdl/INPUT.H"
#include "sdl/sound.h"
#include "sdl/dshow.h"
#include "trapkeys.h"
#include "enemys.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char* THIS_FILE = (char*) __FILE__;
#endif

const int np=115;//number of video buffers(pictures)
const int sb=14 ;//sound buffers count
const int maxenemys=20;//max count of enemys
int ctis=30,ctis1=50;//count ticks of player/enemys change
const int shag=2;//player shag(in pixels)
const int cburp=250;//player afther eat burp number
const int mh=8;//max hearts
const int mast=500;//master is birth (tics)
const int boss=750;//master is birth (tics)
bool g_bTimerWorking=true;

UINT tcount=10;//cycle (in msec) of player/enemys timer
int i; //counter
int score=0;//playes score
int rmax,rmax1;//->3,->2
int ccount=0,mccount=500;//counter and max count cycles of chang. pl
float is = 0;
int is1=0,tis=0,tis1=0;//counters of changes of packman/enemy pictures
int acount=0;//count of rest apples
bool pchang=false;//player was changed into big
int mx=40,my=60;//my coordinates
int bmx, bmy ;//my old coordinates
int burp=0;//burp counter
int z[140]; //array of pictures
int s[sb]; //array of sounds
char board[19][26];//board for playing
struct z enemy[maxenemys+1];
int nenemys=0;//count of enemys
int num[11];//number (0-9) pictures
int c[26];//character (a-z) pictures
int death[10];//player death
int mdeath[4];//enemy death
int masterb[12];//master birth
int bi;//backimage+board picture
int tmx,tmy;//player board coordinates
int kmx=0,kmy=0;//player position shift
int hcount;//count of player lives
bool pdeath=false;//player death reg
int ndeath=0;//if death - show picture (ndeath)
int nimb=0,mnimb=155;//player nimb time,max nimb time
int lnum=0;
bool lock=true;
HWND mwnd;
char bufr[1024];//file name buffer
int zast[6][2];//startgame-options-higscore-exit images
int sel;//select option picture
int gover;//game over picture
int pause;//pause picture
int bonp;//bonus picture
void (*bg)();
bool gpause=false;
int ns=0;
int score1=0;
bool passw=false;//start level from password
int bonus1;
int bonus;//bonus coounters

int scim,lvim,lcompl,enter,gspeed;//score and level images
char hisctmp[1024];//path to temp directory (hiscores)
char optionstmp[1024];//path to temp directory (current level)
char curlevel[1024];//path to temp directory (current level)
bool first=true;//first run
bool g_exit_to_menu = false;
int nbitmap;//bitmap than used to restore screen
bool putopt,playnow=false;
int ibm1,ibm2,ibm3;
bool mpressed=false;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

extern void clear_page1_dirty();

void waitforent()
{
	clear_page1_dirty();
	
	// 1. Wait until any initial keys, mouse clicks, or touch inputs are released
	bool pressed = true;
	do {
		PeekAndPump();
		unsigned char ka[256];
		getkeys(ka);
		bool keys = ka[0x01] || ka[0x1c] || ka[0x39] || ka[0xc8] || ka[0xd0] || ka[0xcb] || ka[0xcd];
		
		int mx, my;
		bool lb, rb, mb;
		getmouse(mx, my, lb, rb, mb);
		
		pressed = keys || lb || g_joystick_active;
		Sleep(5);
	} while (pressed);
	
	resetkeyboard();
	
	// 2. Wait for the next ENTER/SPACE key or a mouse/touch press
	int k = 0;
	bool clicked = false;
	do {
		PeekAndPump();
		k = getkey();
		
		int mx, my;
		bool lb, rb, mb;
		getmouse(mx, my, lb, rb, mb);
		if (lb || g_joystick_active) {
			clicked = true;
		}
		Sleep(5);
	} while (k != 0x1c && k != 0x9c && k != 0x39 && !clicked);
	
	// 3. Wait until the dismissing input is released before returning to the menu
	pressed = true;
	do {
		PeekAndPump();
		unsigned char ka[256];
		getkeys(ka);
		bool keys = ka[0x01] || ka[0x1c] || ka[0x39] || ka[0xc8] || ka[0xd0] || ka[0xcb] || ka[0xcd];
		
		int mx, my;
		bool lb, rb, mb;
		getmouse(mx, my, lb, rb, mb);
		
		pressed = keys || lb || g_joystick_active;
		Sleep(5);
	} while (pressed);
	
	resetkeyboard();
}

bool enters(char *e)

{
	clear_page1_dirty();
#ifdef SDL_INPUT_H
	start_text_input();
#endif
	int k,z;
	strcpy(e,"");
	do{
		PeekAndPump();
		k=getkey();
		z=tolower(MapVirtualKey(k,1));
		if ((z>='a')&&(z<='z')&&(strlen(e)<10)) 
		{
			putimage(250+strlen(e)*30,320,c[z-'a'],0);
			updatebitmap(250+strlen(e)*30,320,30,30,-1,nbitmap);
			size_t len = strlen(e);
			e[len] = (char)z;
			e[len + 1] = '\0';
		} else if (k==0x0e) //backspace pressed
		{
			e[strlen(e)-1]='\0';
			putimage(250+strlen(e)*30,320,::z[0],0);
			updatebitmap(250+strlen(e)*30,320,30,30,-1,nbitmap);
		}
		else if (k==0x01)//esc pressed
		{
			loadbmp(0,0,"begin.gra");
			//for (i=0;i<6;i++) putsprite(315,270+i*50,zast[i][0],0);
			putsprite(315,320,zast[1][1],0);
		}
	}while ((k!=0x01)&&(k!=0x1c)&&(k!=0x9c));
#ifdef SDL_INPUT_H
	stop_text_input();
#endif
	if (strlen(e)==0) strcpy(e,"noname");
	if (k==0x01) return false; else return true;
}
 
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackmanDlg dialog

CPackmanDlg::CPackmanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPackmanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPackmanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



void showh()
{
	putimagetoscreen(565,0,800,31,565,0,bi,1);
	if (hcount>mh) hcount=mh;
	for (i=0;i<hcount;i++)
	{
		putsprite(770-(i*30),1,z[9],1);
	}
}

void showscore(int x,int y,int score1)
{
	int s=0;//num count
	int ns[8];
	do{
		ns[s]=score1%10;
		score1/=10;
		s++;
	}while (score1!=0);
	s--;
	while (s>=0)
	{
		putsprite(x,y,num[ns[s]],1);
		putsprite(x, y, num[ns[s]],0);
		x+=30;
		s--;
	}
}

int mrand(int n)
{
	if (n==0) return 0;
	if (n>2) return rand()/rmax;
	else return rand()/rmax1;
}



void CPackmanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPackmanDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPackmanDlg, CDialog)
	//{{AFX_MSG_MAP(CPackmanDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackmanDlg message handlers


void restoreall1()
{
	restorescreen();
	//show credits 
	//load score and level images
	restoreimage(scim);
	restoreimage(lvim);
	// load elements pictures
	restoreimage(z[0]);
	restoreimage(z[3]);
	restoreimage(z[5]);
	restoreimage(z[6]);
	restoreimage(z[7]);
	restoreimage(z[9]);
	restoreimage(z[10]);
	restoreimage(z[11]);
	restoreimage(z[12]);
	restoreimage(z[20]);
	restoreimage(z[21]);
	restoreimage(z[22]);
	restoreimage(z[23]);
	restoreimage(z[24]);
	restoreimage(z[25]);
	restoreimage(z[26]);
	restoreimage(z[40]);
	restoreimage(z[90]);
	restoreimage(z[100]);


	restoreimage(z[50]);
	restoreimage(z[51]);
	restoreimage(z[52]);
	restoreimage(z[53]);
	restoreimage(z[54]);
	restoreimage(z[55]);
	restoreimage(z[56]);
	restoreimage(z[70]);
	restoreimage(z[120]);
	restoreimage(z[130]);

	restoreimage(masterb[0]);
	restoreimage(masterb[1]);
	restoreimage(masterb[2]);
	restoreimage(masterb[3]);
	restoreimage(masterb[4]);
	restoreimage(masterb[5]);

	restoreimage(masterb[6]);
	restoreimage(masterb[7]);
	restoreimage(masterb[8]);
	restoreimage(masterb[9]);
	restoreimage(masterb[10]);
	restoreimage(masterb[11]);

	restoreimage(gover);//game over picture
	restoreimage(pause);//game over picture
	restoreimage(bonp);//bonus picture
	restoreimage(lcompl);//level complete picture
	restoreimage(gspeed);//game speed in options screen
	//-----------
	restoreimage(bi);
	//-
	restoreimage(z[1]);
	restoreimage(z[2]);
	restoreimage(z[4]);
	//--
	
	for (i=0;i<10;i++)
	{
		restoreimage(num[i]);
	}
	for (i=0;i<26;i++)
	{
		restoreimage(c[i]);
	}

	for (i=0;i<9;i++)
	{
		restoreimage(death[i]);
	}

	restoreimage(mdeath[0]);
	restoreimage(mdeath[1]);
	restoreimage(mdeath[2]);

	restoreimage(enter);//press enter or spacebar to continue picture

	//load and show menu
	restoreimage(zast[0][0]);
	restoreimage(zast[0][1]);
	restoreimage(zast[1][0]);
	restoreimage(zast[1][1]);
	restoreimage(zast[2][0]);
	restoreimage(zast[2][1]);
	restoreimage(zast[4][0]);
	restoreimage(zast[4][1]);
	restoreimage(zast[3][0]);
	restoreimage(zast[3][1]);
	restoreimage(zast[5][0]);
	restoreimage(zast[5][1]);
	restoreimage(sel);
}

void loadimages ()
{
	//show credits 
	//load score and level images
	scim=loadbmptoptr("score.gra");
	lvim=loadbmptoptr("level.gra");
	// load elements pictures
	z[0]=loadbmptoptr("back.gra");
	z[3]=loadbmptoptr("pchang.gra");
	z[5]=loadbmptoptr("i.gra");
	z[6]=loadbmptoptr("i1.gra");
	z[7]=loadbmptoptr("i2.gra");
	z[9]=loadbmptoptr("heart.gra");
	z[10]=loadbmptoptr("angel.gra");
	z[11]=loadbmptoptr("is.gra");
	z[12]=loadbmptoptr("is1.gra");
	z[20]=loadbmptoptr("monster1.gra");
	z[21]=loadbmptoptr("monster2.gra");
	z[22]=loadbmptoptr("monster3.gra");
	z[23]=loadbmptoptr("monster4.gra");
	z[24]=loadbmptoptr("monster5.gra");
	z[25]=loadbmptoptr("monster6.gra");
	z[26]=loadbmptoptr("monster7.gra");
	z[40]=loadbmptoptr("boss1.gra");
	z[90]=loadbmptoptr("master1.gra");
	z[100]=loadbmptoptr("master1_1.gra");


	z[50]=loadbmptoptr("monstr12.gra");
	z[51]=loadbmptoptr("monstr22.gra");
	z[52]=loadbmptoptr("monstr32.gra");
	z[53]=loadbmptoptr("monstr42.gra");
	z[54]=loadbmptoptr("monstr52.gra");
	z[55]=loadbmptoptr("monstr62.gra");
	z[56]=loadbmptoptr("monstr72.gra");
	z[70]=loadbmptoptr("boss12.gra");
	z[120]=loadbmptoptr("master12.gra");
	z[130]=loadbmptoptr("master12_1.gra");

	masterb[0]=loadbmptoptr("masterb1.gra");
	masterb[1]=loadbmptoptr("masterb2.gra");
	masterb[2]=loadbmptoptr("masterb3.gra");
	masterb[3]=loadbmptoptr("masterb4.gra");
	masterb[4]=loadbmptoptr("masterb5.gra");
	masterb[5]=loadbmptoptr("masterb6.gra");

	masterb[6]=loadbmptoptr("bossb1.gra");
	masterb[7]=loadbmptoptr("bossb2.gra");
	masterb[8]=loadbmptoptr("bossb3.gra");
	masterb[9]=loadbmptoptr("bossb4.gra");
	masterb[10]=loadbmptoptr("bossb5.gra");
	masterb[11]=loadbmptoptr("bossb6.gra");

	gover=loadbmptoptr("gameover.gra");//game over picture
	pause=loadbmptoptr("pause.gra");//game over picture
	bonp=loadbmptoptr("bonus.gra");//bonus picture
	lcompl=loadbmptoptr("lcompl.gra");//level complete picture
	gspeed=loadbmptoptr("gspeed.gra");//game speed in options screen

	char ld[10];
	//load number pictures
	strcpy(ld,"0.gra");
	for (i=0;i<10;i++)
	{
		ld[0]='0'+i;
		num[i]=loadbmptoptr(ld);
	}
	//load char's
	strcpy(ld,"charA.gra");
	for (i=0;i<26;i++)
	{
		ld[4] = (i == ('i' - 'a')) ? 'i' : ('A' + i);
		c[i]=loadbmptoptr(ld);
	}

	//load player death pictures
	strcpy(ld,"lose1.gra");
	for (i=0;i<9;i++)
	{
		ld[4]='1'+i;
		death[i]=loadbmptoptr(ld);
	}

	mdeath[0]=loadbmptoptr("mdeath1.gra");
	mdeath[1]=loadbmptoptr("mdeath2.gra");
	mdeath[2]=loadbmptoptr("mdeath3.gra");

	enter=loadbmptoptr("press.gra");//press enter or spacebar to continue picture

	//load and show menu
	zast[0][0]=loadbmptoptr("start.gra");
	zast[0][1]=loadbmptoptr("start1.gra");
	zast[1][0]=loadbmptoptr("password.gra");
	zast[1][1]=loadbmptoptr("password1.gra");
	zast[2][0]=loadbmptoptr("highscore.gra");
	zast[2][1]=loadbmptoptr("highscore1.gra");
	zast[4][0]=loadbmptoptr("help1.gra");
	zast[4][1]=loadbmptoptr("help2.gra");
	zast[3][0]=loadbmptoptr("options.gra");
	zast[3][1]=loadbmptoptr("options1.gra");
	zast[5][0]=loadbmptoptr("exit.gra");
	zast[5][1]=loadbmptoptr("exit1.gra");
	sel=loadbmptoptr("sel.gra");

}

void reloadimages()
{
	//show credits 
	//load score and level images
	reloadbmptoptr(scim,"score.gra");
	reloadbmptoptr(lvim,"level.gra");
	// load elements pictures
	reloadbmptoptr(z[0],"back.gra");
	reloadbmptoptr(z[3],"pchang.gra");
	reloadbmptoptr(z[5],"i.gra");
	reloadbmptoptr(z[6],"i1.gra");
	reloadbmptoptr(z[7],"i2.gra");
	reloadbmptoptr(z[9],"heart.gra");
	reloadbmptoptr(z[10],"angel.gra");
	reloadbmptoptr(z[11],"is.gra");
	reloadbmptoptr(z[12],"is1.gra");
	reloadbmptoptr(z[20],"monster1.gra");
	reloadbmptoptr(z[21],"monster2.gra");
	reloadbmptoptr(z[22],"monster3.gra");
	reloadbmptoptr(z[23],"monster4.gra");
	reloadbmptoptr(z[24],"monster5.gra");
	reloadbmptoptr(z[25],"monster6.gra");
	reloadbmptoptr(z[26],"monster7.gra");
	reloadbmptoptr(z[40],"boss1.gra");
	reloadbmptoptr(z[90],"master1.gra");
	reloadbmptoptr(z[100],"master1_1.gra");


	reloadbmptoptr(z[50],"monstr12.gra");
	reloadbmptoptr(z[51],"monstr22.gra");
	reloadbmptoptr(z[52],"monstr32.gra");
	reloadbmptoptr(z[53],"monstr42.gra");
	reloadbmptoptr(z[54],"monstr52.gra");
	reloadbmptoptr(z[55],"monstr62.gra");
	reloadbmptoptr(z[56],"monstr72.gra");
	reloadbmptoptr(z[70],"boss12.gra");
	reloadbmptoptr(z[120],"master12.gra");
	reloadbmptoptr(z[130],"master12_1.gra");

	reloadbmptoptr(masterb[0],"masterb1.gra");
	reloadbmptoptr(masterb[1],"masterb2.gra");
	reloadbmptoptr(masterb[2],"masterb3.gra");
	reloadbmptoptr(masterb[3],"masterb4.gra");
	reloadbmptoptr(masterb[4],"masterb5.gra");
	reloadbmptoptr(masterb[5],"masterb6.gra");

	reloadbmptoptr(masterb[6],"bossb1.gra");
	reloadbmptoptr(masterb[7],"bossb2.gra");
	reloadbmptoptr(masterb[8],"bossb3.gra");
	reloadbmptoptr(masterb[9],"bossb4.gra");
	reloadbmptoptr(masterb[10],"bossb5.gra");
	reloadbmptoptr(masterb[11],"bossb6.gra");

	reloadbmptoptr(gover,"gameover.gra");//game over picture
	reloadbmptoptr(pause,"pause.gra");//game over picture
	reloadbmptoptr(bonp,"bonus.gra");//bonus picture
	reloadbmptoptr(lcompl,"lcompl.gra");//level complete picture
	reloadbmptoptr(gspeed,"gspeed.gra");//game speed in options screen

	char ld[10];
	//load number pictures
	strcpy(ld,"0.gra");
	for (i=0;i<10;i++)
	{
		ld[0]='0'+i;
		reloadbmptoptr(num[i],ld);
	}
	//load char's
	strcpy(ld,"charA.gra");
	for (i=0;i<26;i++)
	{
		ld[4] = (i == ('i' - 'a')) ? 'i' : ('A' + i);
		reloadbmptoptr(c[i],ld);
	}

	//load player death pictures
	strcpy(ld,"lose1.gra");
	for (i=0;i<9;i++)
	{
		ld[4]='1'+i;
		reloadbmptoptr(death[i],ld);
	}

	reloadbmptoptr(mdeath[0],"mdeath1.gra");
	reloadbmptoptr(mdeath[1],"mdeath2.gra");
	reloadbmptoptr(mdeath[2],"mdeath3.gra");

	reloadbmptoptr(enter,"press.gra");//press enter or spacebar to continue picture

	//load and show menu
	reloadbmptoptr(zast[0][0],"start.gra");
	reloadbmptoptr(zast[0][1],"start1.gra");
	reloadbmptoptr(zast[1][0],"password.gra");
	reloadbmptoptr(zast[1][1],"password1.gra");
	reloadbmptoptr(zast[2][0],"highscore.gra");
	reloadbmptoptr(zast[2][1],"highscore1.gra");
	reloadbmptoptr(zast[4][0],"help1.gra");
	reloadbmptoptr(zast[4][1],"help2.gra");
	reloadbmptoptr(zast[3][0],"options.gra");
	reloadbmptoptr(zast[3][1],"options1.gra");
	reloadbmptoptr(zast[5][0],"exit.gra");
	reloadbmptoptr(zast[5][1],"exit1.gra");
	reloadbmptoptr(sel,"sel.gra");
}

BOOL CPackmanDlg::OnInitDialog()
{
	//HCURSOR dd=AfxGetApp()->LoadStandartCursor//(IDB_BITMAP1);
	if (!initgraph(this->m_hWnd,800,600,32,1,np,10))
	{
		MessageBox("Graphics initialization error.");
		exit(-1);
	}
	//HCURSOR dd=AfxGetApp()->LoadCursor(IDC_CURSOR1);
	//SetCursor(dd);
	//hiscore and options path
	GetTempPath(1024,hisctmp);
	strcpy(optionstmp,hisctmp);
	strcpy(curlevel, hisctmp);
	strcat(hisctmp,"\\hiscore.dat");
	strcat(optionstmp,"\\options.dat");
	strcat(curlevel, "\\curlevel.dat");
	if (GetFileAttributes(hisctmp)==0xFFFFFFFF)
	{
		CopyFile("hiscore.dat",hisctmp,false);
		SetFileAttributes(hisctmp,FILE_ATTRIBUTE_NORMAL);
	}
	if (GetFileAttributes(optionstmp)==0xFFFFFFFF)
	{
		CopyFile("options.dat",optionstmp,false);
		SetFileAttributes(optionstmp,FILE_ATTRIBUTE_NORMAL);
	}
	if (GetFileAttributes(curlevel) == 0xFFFFFFFF)
	{
		CopyFile("curlevel.dat", curlevel, false);
		SetFileAttributes(curlevel, FILE_ATTRIBUTE_NORMAL);
	}
	
	CFile fl;
	//read game speed from file
	if (fl.Open(optionstmp, CFile::modeRead | CFile::typeBinary))
	{
		UINT readBytes = fl.Read(&tcount, sizeof(tcount));
		fl.Close();
		if (readBytes != sizeof(tcount) || (tcount != 10 && tcount != 14 && tcount != 18 && tcount != 22))
		{
			tcount = 10;
		}
	}
	else
	{
		tcount = 10;
	}
	//get current level from file
	if (fl.Open(curlevel, CFile::modeRead | CFile::typeBinary))
	{
		fl.Read(&lnum, sizeof(lnum));
		lnum *= -1;
		fl.Close();
	}
	else
	{
		if (fl.Open(curlevel, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			lnum = 1;
			fl.Write(&lnum, sizeof(lnum));
			fl.Close();
			lnum = -1;
		}
	}
	//lnum = -5;
	//----
	mwnd=this->m_hWnd;
	dsinit(mwnd);
	rmax=RAND_MAX/3+1;
	rmax1=RAND_MAX/2+1;
	//set random time generator
	srand((unsigned)time(NULL));
	//board initialization (from file)
	//trapsyskeys();//disable system program switching keys (alt-tab,...)
	if (!initdinput()) 
	{
		if (MessageBox("You dont have DirectX 8 or above installed. Do you want to install DirectX 8.1 from game CD?","Error",MB_ICONERROR|MB_YESNO)==IDYES)
			ShellExecute(NULL,"open","DirectX_8.1b\\dxsetup.exe",NULL,NULL,SW_SHOW);
		ShellExecute(NULL,"open","readme.txt",NULL,NULL,SW_SHOW); 
		exit(-1);
	}
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	//controls initialization
	if (!initkeyboard(this->m_hWnd,true,10))
	{
		MessageBox("Keyboard initialization error","Error",MB_ICONERROR|MB_OK);
		exit(-1);
	}
	//sound initialization
	initsound(this->m_hWnd,DS_NORMAL,sb);
	//load sound effects
	s[0]=loadwav("pchang.san");//player change
	s[2]=loadwav("bchang.san");//player back change
 	s[3]=loadwav("burp.san");//burp sound (player)
	s[4]=loadwav("heart.san");//heart gotn
	s[5]=loadwav("death1.san");//packman death 1
	s[6]=loadwav("death2.san");//packman death 2
	s[7]=loadwav("death3.san");//packman death 3
	s[8]=loadwav("mdeath.san");//monstaer death
	//s[9]=loadwav("winlev.san");//win level!!!

	// graphics initialization
	//remove mouse cursor


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPackmanDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
		CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void filltable(bool norest)
{
	if (norest) nenemys = 0;

	putimagetoptr(0, 0, 800, 600, 0, 0, z[4], bi);

	//draw play board
	for (i=0;i<19;i++)
	for (int i1=0;i1<26;i1++)
		{
			if (board[i][i1]==5)
			{
				if (norest) {mx=i1*30+10;my=i*30+30;board[i][i1]=0;}
			}
			else if (board[i][i1]>19)
			{//set enemy characteristics
				if (norest)
				{
					enemy[nenemys].x=i1*30+10;
					enemy[nenemys].y=i*30+30;
					enemy[nenemys].bx = i1 * 30 + 10;
					enemy[nenemys].by = i * 30 + 30;
					enemy[nenemys].level=board[i][i1]-20;
					enemy[nenemys].stat=0;
					if (enemy[nenemys].level<20) enemy[nenemys].nstat=0;
					else if (enemy[nenemys].level<70) enemy[nenemys].nstat=boss;
					else enemy[nenemys].nstat=mast;
					enemy[nenemys].mnstat=4;/*8-enemy[f].level;*/
					enemy[nenemys].go=3; //enemy ->
					if (board[i][i1]<90) enemy[nenemys].shag=2;
					else enemy[nenemys].shag=3;
					nenemys++;
					board[i][i1]=0;
				}
			}
			else  
			{
				virtputsprite(i1*30+10,i*30+30,z[(unsigned char)board[i][i1]],bi);
			}
			if ((board[i][i1]==2)&&norest) acount++;
		}
	// draw words score and bonus
#ifdef __ANDROID__
	virtputsprite(10, 35, c['e' - 'a'], bi);
	virtputsprite(35, 35, c['s' - 'a'], bi);
	virtputsprite(60, 35, c['c' - 'a'], bi);
#else
	virtputsprite(10, 0, c['e' - 'a'], bi);
	virtputsprite(35, 0, c['s' - 'a'], bi);
	virtputsprite(60, 0, c['c' - 'a'], bi);
#endif
	virtputsprite(110, 0, scim, bi);
	virtputsprite(400, 0, bonp, bi);

	putimagetoscreen(0, 0, 799, 599, 0, 0, bi, 1);
	showh();//show lives
	flip(0);
	putimagetoscreen(0, 0, 799, 599, 0, 0, bi, 1);
	showh();//show lives
}


void StartLevel()
{
	bonus=750;
	lock=true;
	ccount=0;
	acount=0;
	pchang=false;
	tis=0;is=0;nimb=1;
	pdeath=false;
	ndeath=0;
	CString lv;
	CFile fl;

	lv.Format("%d\\",lnum-1);
	
	//---
	if (passw == false)
	{
		//braha aharona and level complete
		playnow = false;
		strcpy(bufr, (lv + "end.san").GetBuffer(1024));
		if (GetFileAttributes(bufr) != 0xFFFFFFFF)
		{
			dsstop();
			putsprite(80, 280, lcompl, 0);
			putsprite(0, 340, enter, 0);
			updatebitmap(0, 0, 0, 0, -1, nbitmap);
			s[11] = loadwav(bufr);
			playbuf(s[11], false);
			//while (bufplaying(s[11])) PeekAndPump();
			waitforent();
			if (s[11] != 0) pdestroybuf();
			putimagetoscreen(0, 30, 800, 600, 0, 30, bi, 0);
		}
		dsstart();
		//while (bufplaying(s[9]));
		freebitmap();
		freebitmap();
		freebitmap();
		prelease();
		prelease();
		prelease();
		if (s[10] != 0) { pdestroybuf(); s[10] = 0; }
		if (s[12] != 0) { pdestroybuf(); s[12] = 0; }
	}
	else passw = false;

	printf("[DEBUG] StartLevel: lnum=%d\n", lnum);
	char cwd_buf[1024];
	if (getcwd(cwd_buf, sizeof(cwd_buf))) {
		printf("[DEBUG] StartLevel: Working Directory = %s\n", cwd_buf);
	}
	lv.Format("%d\\",lnum);
	printf("[DEBUG] StartLevel: formatted lv = '%s', normalized = '%s'\n", lv.c_str(), normalize_path(lv.c_str()).c_str());
	DWORD attrs = GetFileAttributes(lv);
	printf("[DEBUG] StartLevel: GetFileAttributes(lv) = 0x%08X\n", (unsigned int)attrs);
	//if level not exist -> go to main screen
	if (attrs==0xFFFFFFFF) {
		printf("[DEBUG] StartLevel: Level not found! Returning to main menu.\n");
		lnum=0;lock=true;ShowCursor(true);g_exit_to_menu = true;return;
	}
	//load map
	std::string map_path = (lv+"1.dat").c_str();
	printf("[DEBUG] StartLevel: Loading map from '%s', normalized = '%s'\n", map_path.c_str(), normalize_path(map_path).c_str());
	if (fl.Open(lv+"1.dat",CFile::modeRead))
	{
		fl.Read(board,sizeof(board));
		fl.Close();
		printf("[DEBUG] StartLevel: Map loaded successfully.\n");
	}
	else
	{
		printf("[DEBUG] StartLevel: Failed to load map from '%s'!\n", map_path.c_str());
	}
	//load level images
	strcpy(bufr,(lv+"wall.gra").GetBuffer(1024));
	z[1]=loadbmptoptr(bufr);
	ibm1=fillbitmap(z[1]);
	strcpy(bufr,(lv+"eat.gra").GetBuffer(1024));
	z[2]=loadbmptoptr(bufr);
	ibm2=fillbitmap(z[2]);
	strcpy(bufr,(lv+"eat.san").GetBuffer(1024));
	s[12]=loadwav(bufr);//eat apple
	strcpy(bufr,(lv+"backim.gra").GetBuffer(1024));
	z[4]=loadbmptoptr(bufr);
	ibm3=fillbitmap(z[4]);
	playnow=true;
	
	filltable(true);



	//show level number
	putsprite(275,285,lvim,0);
	showscore(445,280,lnum);

	//show level preview (example:brahot)
	strcpy(bufr,(lv+"start.san").GetBuffer(1024));
	if (GetFileAttributes(bufr)!=0xFFFFFFFF)
	{
		dsstop();
		s[10]=loadwav(bufr);
		playbuf(s[10],false); 
	}
	strcpy(bufr,(lv+"start.gra").GetBuffer(1024));
	if (GetFileAttributes(bufr)!=0xFFFFFFFF)
	{
		loadbmp(150,150,bufr);
	}

	//wait for player start
	putsprite(0,315,enter,0);
	showscore(290,0,score);
	showscore(475,0,bonus);

	waitforent();
	dsstart();

	//play background sound
	strcpy(bufr,(lv+"back.mp3").GetBuffer(1024));
	if (GetFileAttributes(bufr)!=0xFFFFFFFF)
		dsplay(bufr,true);
	//clear level

	putimagetoscreen(0,30,800,600,0,30,bi,1);
	putsprite(mx,my,z[int(is) + 5], 1);
	bmx = mx; bmy = my;
	putimagetoscreen(0, 30, 800, 600, 0, 30, bi, 0);

	//
	//resetkeyboard();
	lock=false;
	//----------------

}


bool eat(int kmy, int kmx, bool player)//if apple was eaten
{
	//show free place afther apple
	putimagetoscreen(kmx*30+10,kmy*30+30,kmx*30+40,kmy*30+60,kmx*30+10,kmy*30+30,z[4],1);
	putimagetoscreen(kmx * 30 + 10, kmy * 30 + 30, kmx * 30 + 40, kmy * 30 + 60, kmx * 30 + 10, kmy * 30 + 30, z[4], 0);
	putimagetoptr(kmx*30+10,kmy*30+30,kmx*30+40,kmy*30+60,kmx*30+10,kmy*30+30,z[4],bi);
	//updatebitmap(kmx*30+10,kmy*30+30,kmx*30+40,kmy*30+60,bi,nbitmap);
	//free place in table
	int z=board[kmy][kmx];
	board[kmy][kmx]=0;
	//check what apple was eaten
	if (z==2) 
		{
		 acount--;
		 if (player) 
		 {score++;burp++;playbuf(s[12],false);}
		 if (acount==0) 
		 {
			 lnum++;

			 //write progression to file
			 CFile fl;
			 if (fl.Open(curlevel, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
			 {
				 //bufr[0] = lnum;
				 fl.Write(&lnum , sizeof(lnum));
				 fl.Close();
			 }
			score+=bonus;
			bonus=0;
			//clear place of score
			putimagetoscreen(290,0,399,31,290,0,bi,0);
			//show score
			showscore(290,0,score);
			StartLevel();
			return true;
		 }
	}
		else if (player)
		{
			if (z==3)
			{
				pchang=true;ccount=0;
				if ((!bufplaying(s[10]))&&(!bufplaying(s[3]))&&(!bufplaying(s[4]))&&(!bufplaying(s[2]))) playbuf(s[0],false);
				score+=5;burp+=5;
			}
			else
			{
				hcount++;
				if ((!bufplaying(s[10]))&&(!bufplaying(s[0]))&&(!bufplaying(s[2]))&&(!bufplaying(s[3]))) playbuf(s[4],false);
			}
		}
		if (player)
		{
			if (burp>=cburp)
			{
				burp-=cburp;
				if ((!bufplaying(s[10]))&&(!bufplaying(s[2]))&&(!bufplaying(s[0]))) playbuf(s[3],false);
			}
			//clear place of score
			putimagetoscreen(290,0,399,31,290,0,bi,0);
			//show score
			showscore(290,0,score);
		}
		return false;
}

void CALLBACK EXPORT TimerProc1(HWND, UINT, UINT, DWORD)
//timer of enemys/player picture change/moving
{
	static int smx,smy;
	int smx1=0,smy1=0;
	int kk=getkey();
	if ((kk==0x1c)||(kk==0x9c)||(kk==0x39))
	{//key enter or spacebar pressed
		if (!lock) 
		{

			gpause=true;
			lock=true;
			putsprite(325,285,pause,0);
			putsprite(0,320,enter,0);
		}
		else if (gpause)
		{
			//putimagetoscreen(0,55,0,280,bi,0);
			//int iz = getimage(0, 0, 799, 599, 1);
			//putimage(0, 0, iz , 0);
			filltable(false);

			gpause=false;
			lock=false;
		}
	} 
	else if (kk==0x01)
	{//key esc pressed
		if (!lock) 
		{
			ShowCursor(true);
			g_exit_to_menu = true;
		}
		/*else if (gpause)
		{
			putimagetoscreen(320,280,480,320,320,280,bi,0);
			gpause=false;
			lock=false;
		}*/
	}
	if (lock) return;
	bonus1++;if (bonus1>10) 
	{
		bonus1=0;bonus-=1;
		if (bonus<0) bonus=0;
	}
	if ((score/1000)>(score1/1000)) 
	{
		hcount++;
		if ((!bufplaying(s[10]))&&(!bufplaying(s[0]))&&(!bufplaying(s[2]))&&(!bufplaying(s[3]))) playbuf(s[4],false);
	}
	score1=score;
	//clear old packman in backbuffer
	putimagetoscreen(bmx, bmy, bmx + 30, bmy + 30, bmx, bmy, z[4], 1);
	bmx=mx,bmy=my;//backup main coordinates
	//--------------player change/move
		tis++;if (tis>=ctis)
	{tis=0;is+=0.2f;if (is>1) 
		{
			is=0;
			if (pdeath) ndeath++;
		}
	}//player picture change
	if (nimb>0) nimb++;
	if (nimb==mnimb) nimb=0;
	if (pdeath)
	{
		if (ndeath==18) 
		{
			pdeath=false;ctis*=3;ndeath=0;nimb=1;
			//if playser was death totally...
			if (hcount==0) 
			{ 
				lock=true;
				putsprite(289,285,gover,0);
				putsprite(0,330,enter,0);
				dsplay("gameover.mp3",false);
				waitforent(); 
				//---------------
				ShowCursor(true);
				g_exit_to_menu = true;
				return;
			}
		}
	}
	else
	{
		//if packman was changed, check if time not expired
		if (pchang==true) 
		{
			ccount++;if (ccount==mccount) 
			{pchang=false;ccount=0;
			if ((!bufplaying(s[10]))&&(!bufplaying(s[3]))&&(!bufplaying(s[4]))&&(!bufplaying(s[0]))) playbuf(s[2],false);}
		}
		kmx=0;kmy=0;
		if (((mx-10)%30)!=0) kmx=1;
		if (((my-30)%30)!=0) kmy=1;
		tmx=(mx-10)/30;tmy=(my-30)/30;
		if ((GetAsyncKeyState(0x26) & 0x8000) || (GetAsyncKeyState(0x57) & 0x8000)) smy1 =-1;
		if ((GetAsyncKeyState(0x28) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000)) smy1 = 1;
		if ((GetAsyncKeyState(0x25) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000)) smx1 =-1;
		if ((GetAsyncKeyState(0x27) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000)) smx1 = 1;
			//---
		if (smx1!=0) smx=smx1;
		if (smy1!=0) smy=smy1;

 		if (smy<0) 
		{
			if (my==30) {my=570;tmy=18;kmy=0;}
			else if ((board[(my-30-shag)/30][tmx]!=1)\
				&&(board[(my-30-shag)/30][tmx+kmx]!=1)) 
			my-=shag;
			else smy=0;
		}

		else if (smy>0) 
		{
			if (my==570) {my=30;tmy=0;kmy=0;}
			else if ((board[(my-1+shag)/30][tmx]!=1)\
				&&(board[(my-1+shag)/30][tmx+kmx]!=1)) 
				my+=shag;
			else smy=0;
		}

		tmy=(my-30)/30;if (((my-30)%30)!=0) kmy=1; else kmy=0;

		if (smx<0)
		{
			if (mx==10) {mx=760;tmx=25;kmx=0;}
			else if ((board[tmy][(mx-10-shag)/30]!=1)\
				&&(board[tmy+kmy][(mx-10-shag)/30]!=1)) 
				mx-=shag;
			else smx=0;
		}

		else if (smx>0)
		{
			if (mx==760) {mx=10;tmx=0;kmx=0;}
			else if ((board[tmy][(mx+19+shag)/30]!=1)\
				&&(board[tmy+kmy][(mx+19+shag)/30]!=1)) 
				mx+=shag;
			else smx=0;
		}

		tmx=(mx-10)/30;if (((mx-10)%30)!=0) kmx=1; else kmx=0;

		//check if apple will eat
		if (board[tmy][tmx]>1) if (eat(tmy,tmx,true)==true) return;
		if (board[tmy+kmy][tmx]>1) if (eat(tmy+kmy,tmx,true)==true) return;
		if (board[tmy][tmx+kmx]>1) if (eat(tmy,tmx+kmx,true)==true) return;
		if (board[tmy+kmy][tmx+kmx]>1) if (eat(tmy+kmy,tmx+kmx,true)==true) return;
	}
	//----------------------------- enemys section
	UpdateEnemiesBehavior();
//!!!!! draw enemy and player in backbuffer
		for (i = 0; i < nenemys; i++)
		{
			if (enemy[i].stat == 1)//enemy is booming
			{
				if (tis1 == 0) enemy[i].nstat++;
				if (enemy[i].nstat >= enemy[i].mnstat) { enemy[i].nstat = 0; enemy[i].stat = 2; tis1 = 1; }
				//draw enemy on the screen (in new position)
				putsprite(enemy[i].x, enemy[i].y, mdeath[enemy[i].nstat/2], 1);
			}
			else if (enemy[i].stat == 2)//enemy is waiting for rebirth
			{

				if (tis1 == 0) enemy[i].nstat++;
				if (enemy[i].nstat >= enemy[i].mnstat) { enemy[i].nstat = 0; enemy[i].stat = 0; }
				if ((enemy[i].nstat /2) != 0) putsprite(enemy[i].x, enemy[i].y, z[enemy[i].level + 20], 1);
				else putsprite(enemy[i].x, enemy[i].y, mdeath[2], 1);
			}
			else if (enemy[i].stat == -1)//master is birth
			{
				if (enemy[i].level >= 70) putsprite(enemy[i].x, enemy[i].y, masterb[enemy[i].nstat/5], 1);
				else putsprite(enemy[i].x, enemy[i].y, masterb[enemy[i].nstat/5 + 6], 1);
			}
			else
			{
				//draw enemy on the screen (in new position)
				if ((enemy[i].level >= 70) /* && (tis1 % 25 != 0)*/)
					putsprite(enemy[i].x, enemy[i].y, z[enemy[i].level + 30 + is1], 1);
				else putsprite(enemy[i].x, enemy[i].y, z[enemy[i].level + 20 + is1], 1);
			}
		}
		if (pdeath) putsprite(mx, my, death[ndeath / 2], 1);//show player picture
		else if (nimb > 0 && pchang == false) putsprite(mx, my, z[10], 1);
		else if (pchang == false) putsprite(mx, my, z[int(is)+5], 1);//show player picture
		else putsprite(mx, my, z[int(is)+11], 1);
		//show bonus on screen
		putimagetoscreen(475, 0, 565, 30, 475, 0, bi, 1);
		showscore(475, 0, bonus);
		//show score on screen 
		// 
		//clear place of score
		putimagetoscreen(290, 0, 399, 31, 290, 0, bi, 1);
		//show score
		showscore(290, 0, score);
		//show lives
		showh();

		//flip screen
		flip(0);
}

void newgame()
{
	score=0;score1=0;
	hcount=3;
	g_exit_to_menu = false;
	StartLevel();
	if (g_exit_to_menu) return;
	//::SetTimer(mwnd,1,tcount,&TimerProc1);
	DWORD dwStartTime=GetTickCount();
	DWORD dwTempTime;
	do{
		dwTempTime=GetTickCount();
		if (dwTempTime>=dwStartTime) 
		{
			if ((dwTempTime-dwStartTime)>=tcount)
			{
				dwStartTime=dwTempTime;
				TimerProc1(NULL,0,0,0);
				if (g_exit_to_menu) break;
			}
		}
		else dwStartTime=0;
		PeekAndPump();
		if (g_exit_to_menu) break;
	}while (g_bTimerWorking);
}


void hiscore(int scor)
{
	char a[11][10];
	int b[11];
	char *w,d[14];
	bool sadd=false;
	char pname[12]="";

	CFile fl;

	// Reset the keyboard queue so a leftover ENTER from menu navigation
	// doesn't instantly dismiss the high score screen.
	if (scor == 0) resetkeyboard();

	if (fl.Open(hisctmp,CFile::modeRead))
	{
		fl.Read(bufr,(UINT)fl.GetLength());
		w=bufr;
		for (i=0;i<10;i++)
		{//read and convert file data
			char* cr = strchr(w,'\r');
			char* lf = strchr(w,'\n');
			// Support both CRLF (\r\n) and LF-only (\n) line endings
			char* eol = cr ? cr : lf;
			if (!eol) break; // malformed file
			strncpy(a[i],w,eol-w);
			a[i][eol-w]='\0';
			w = (cr && *(cr+1)=='\n') ? cr+2 : (cr ? cr+1 : lf+1);
			cr = strchr(w,'\r');
			lf = strchr(w,'\n');
			eol = cr ? cr : lf;
			if (!eol) break; // malformed file
			strncpy(d,w,eol-w);
			d[eol-w]='\0';
			w = (cr && *(cr+1)=='\n') ? cr+2 : (cr ? cr+1 : lf+1);
			b[i]=atoi(d);
			if ((!sadd)&&(scor>b[i])) 
				{//set name to record table
				loadbmp(160,220,"enternm.gra");
				updatebitmap(0,0,0,0,-1,nbitmap);
				if (enters(pname))
				{
					b[i+1]=b[i]; memmove(a[i+1], a[i], sizeof(a[i]));
					strcpy(a[i],pname);b[i]=scor;
					i++;sadd=true;
				}
				loadbmp(0,0,"hiscscr.gra");
				}
		}
		fl.Close();
		if ((scor==0)||(sadd==true))
		{
			putsprite(250,5,zast[2][0],0);
			for (i=0;i<10;i++)
			{//show score table
				putsprite(50,50+i*50,num[i],0);
				for (UINT i1=0;i1<strlen(a[i]);i1++)
				{
					putsprite(120+i1*30,50+i*50,c[a[i][i1]-'a'],0);
				}
				showscore(500,50+i*50,b[i]);
			}
			if (sadd)
			{//if score table changed - show it
				if (fl.Open(hisctmp,CFile::modeCreate|CFile::modeWrite))
				{
					for (i=0;i<10;i++)
					{//keep score table in file
						strcpy(d,a[i]);strcat(d,"\r\n");
						fl.Write(d,strlen(d));
						itoa(b[i],d,10);
						strcat(d,"\r\n");
						fl.Write(d,strlen(d));
					}
					fl.Close();
					
				}
			}
			putsprite (0,570,enter,0);
			updatebitmap(0,0,0,0,-1,nbitmap);
			mark_screen_dirty(); // Force screen refresh before waiting
			waitforent();
		}
	}
}


static void wait_for_release()
{
	bool keys_pressed = true;
	bool mouse_pressed = true;
	unsigned char ka[256];
	int mx, my;
	bool lb, rb, mb;
	
	do {
		PeekAndPump();
		
		getkeys(ka);
		keys_pressed = ka[0x01] || ka[0x1c] || ka[0x39] || ka[0xc8] || ka[0xd0] || ka[0xcb] || ka[0xcd];
		
		getmouse(mx, my, lb, rb, mb);
		mouse_pressed = lb;
		
		Sleep(5);
	} while (keys_pressed || mouse_pressed);
	
	resetkeyboard();
	mpressed = false;
}

void begingame()
{
	wait_for_release();
	ShowCursor(false);
	score1=0;
	ctis=6;
	ns=0;
	lock=true;
	if (lnum>0)
	{
		playnow=false;
		freebitmap();
		freebitmap();
		freebitmap();
		prelease();
		prelease();
		prelease();
		prelease();
		if (s[10]>0) {pdestroybuf();s[10]=0;}
		if (s[12]>0) {pdestroybuf();s[12]=0;}
	}
	if (lnum < 0) lnum *= -1;
	if (lnum == 0) lnum = 1;
	//lnum = 1;
	dsplay("start.mp3",true);
	bi=loadbmptoptr("hiscscr.gra");
	bool y;
	do{
	y=removebitmap();
	}while(y);
	nbitmap=fillbitmap(bi);

	if (score>0)
	{//if packman have some score...
		putimage(0,0,bi,0);
		hiscore(score);
		score=0;
	} 
	reloadbmptoptr(bi,"begin.gra");
	for (i=0;i<6;i++) virtputsprite(315,270+i*50,zast[i][0],bi);

	updatebitmap(0,0,0,0,bi,nbitmap);

	putimage(0,0,bi,0);
	putopt=true;
	ShowCursor(true);
	putsprite(315,270,zast[0][1],0);
	//track main menu
	int k;
	resetkeyboard();
	int ns1;
	do{
		ns1=ns;
		PeekAndPump();
		k=getkey();
			if (k==0xc8) {
				ns--;if (ns<0) ns=0;
				wait_for_release();
			}//up pressed
			else if (k==0xd0) {
				ns++;if (ns>5) ns=5 ;
				wait_for_release();
			}//down pressed
			else if ((k==0x1c)||(k==0x9c)||(k==0x39)||mpressed)//enter of spacebar pressed
			{
				wait_for_release();
				ShowCursor(false);
				mpressed=false;
				switch (ns)//select from menu
				{
					//begin new game
				case 0:passw = true; return;
					//SELECT NEW GAME
				case 1:
					{
#ifdef SDL_INPUT_H
						g_select_new_game_active = true;
#endif
						ShowCursor(true);
						putopt=false;
						loadbmp(0,0,"greenscr.gra");
						int passimg=loadbmptoptr("pass.gra");
						putsprite(130,400,passimg,0);
						prelease();

						valloc(0);
						setfillcolor(0x00009600); // Green fill: RGB(0, 150, 0)
						bar(200, 480, 380, 530);
						setcolor(0x00FFFFFF); // White border/text: RGB(255, 255, 255)
						rectangle(200, 480, 380, 530);

						setfillcolor(0x000000B4); // Red fill: RGB(180, 0, 0)
						bar(420, 480, 600, 530);
						setcolor(0x00FFFFFF); // White border/text
						rectangle(420, 480, 600, 530);
						vfree();

						// Draw button text using sprite characters
						putsprite(245, 490, c['y' - 'a'], 0);
						putsprite(275, 490, c['e' - 'a'], 0);
						putsprite(305, 490, c['s' - 'a'], 0);

						putsprite(480, 490, c['n' - 'a'], 0);
						putsprite(510, 490, c['o' - 'a'], 0);
						
						//vait for y or n
						bool ddone = false;
						while (!ddone) {
							PeekAndPump();
							int k = getkey();
							if (k == 0x15) { // 'Y'
								lnum = 1;
								CFile fl;
								if (fl.Open(curlevel, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
									fl.Write(&lnum, sizeof(lnum));
									fl.Close();
								}
								ddone = true;
							} else if (k == 0x01) { // ESC
								ddone = true;
							}
							Sleep(50); // wait 50 msec
						}
						
						putopt=true;
						ShowCursor(false);
#ifdef SDL_INPUT_H
						g_select_new_game_active = false;
#endif
						//put first screen back 
						reloadbmptoptr(bi, "begin.gra");
						for (i = 0; i < 6; i++) virtputsprite(315, 270 + i * 50, zast[i][0], bi);
						putimage(0, 0, bi, 0);
						updatebitmap(0, 0, 0, 0, bi, nbitmap);
						putsprite(315, 320, zast[1][1], 0);
						break;
					}
					break;
					//show highscore
				case 2:
					putopt=false;
					loadbmp(0,0,"hiscscr.gra");
					hiscore(0);
					putopt=true;
					reloadbmptoptr(bi,"begin.gra");
					for (i=0;i<6;i++) virtputsprite(315,270+i*50,zast[i][0],bi);
					putimage(0,0,bi,0);
					updatebitmap(0,0,0,0,bi,nbitmap);
					putsprite(315,370,zast[2][1],0);
					break;

				case 4://show help
					putopt=false;
					reloadbmptoptr(bi,"heplscreen.gra");
					//freebitmap();
					putimage(0,0,bi,0);
					updatebitmap(0,0,0,0,bi,nbitmap);
					waitforent();
					putopt=true;
					reloadbmptoptr(bi,"begin.gra");
					for (i=0;i<6;i++) virtputsprite(315,270+i*50,zast[i][0],bi);
					putimage(0,0,bi,0);
					updatebitmap(0,0,0,0,bi,nbitmap);
					putsprite(315,470,zast[4][1],0);
					break; 
				case 3:
					{
						putopt=false;
						reloadbmptoptr(bi,"greenscr.gra");
						virtputsprite(295,5,zast[4][0],bi);
						virtputsprite(250,60,gspeed,bi);
						for (int i=4;i>0;i--) virtputsprite((4-i)*100+250,110,num[i],bi);
						virtputsprite(0,570,enter,bi);
						putimage(0,0,bi,0);
						updatebitmap(0,0,0,0,bi,nbitmap);//-

						UINT es1;
						do {
						PeekAndPump();
						es1=tcount;
						k=getkey();
						if (k==0xcb) {if (tcount>10) tcount-=4;}
						else if (k==0xcd) {if (tcount<22) tcount+=4;}
						if (es1!=tcount)
						{
							putimagetoscreen(es1*25 - 40,110,es1*25 - 40+30,140,es1*25 - 40,110,bi,0);
							//updatebitmap(es1*5+23,110,30,30,-1,nbitmap);//-
							//updatebitmap(tcount*5+23,110,30,30,-1,nbitmap);//-
							//set options
							CFile fl;
							if (fl.Open(optionstmp, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
							{
								fl.Write(&tcount, sizeof(tcount));
								fl.Close();
							}
						}
						putsprite(tcount*25 - 40,110,sel,0);
						} while (((k!=0x1c)&&(k!=0x9c)&&(k!=0x39)));
						reloadbmptoptr(bi,"begin.gra");
						for (i=0;i<6;i++) virtputsprite(315,270+i*50,zast[i][0],bi);
						putimage(0,0,bi,0);
						updatebitmap(0,0,0,0,bi,nbitmap);
						putsprite(315,420,zast[3][1],0);
						putopt=true;
					}
					break; 
				case 5:
					loadbmp(0,0,"credits.gra");
					resetkeyboard();
					updatebitmap(0,0,0,0,-1,nbitmap);
					putopt=false;
					waitforent();
					closegraph();
					dsclose();
					closesound();
					std::exit(0);
				}
				wait_for_release();
				ShowCursor(true);
			}
		if (ns1!=ns) 
		{
			putimagetoscreen(315,270+ns1*50,530,310+ns1*50,315,270+ns1*50,bi,0);
			putsprite(315,270+ns1*50,zast[ns1][0],0);
			putsprite(315,270+ns*50,zast[ns][1],0);
		}
	}while (0==0);
}

void  CPackmanDlg::OnPaint() 
{
	if (IsIconic())
	{
		gpause=true;
		lock=true;

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
	if (first)
	{
		loadimages();//load images
		first=false;
		bg=begingame; 
		bg();
	}
	else
	{
		//TRACE("%s ","!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		if (!IsIconic())
		{
			//MessageBox("OKLL");
			//DestroyWindow();return;
			if (!initkeyboard(this->m_hWnd,true,10))
			{
				MessageBox("Keyboard initialization error","Error",MB_ICONERROR|MB_OK);
				exit(-1);
			}
			restorescreen();
			restoreall1();
			reloadimages();
			/*if (lock)
			{
				reloadbmptoptr(bi,"begin.gra");
				putimage(0,0,bi,0);
			}*/
			if (playnow)
			{
				putbitmap(ibm1);
				putbitmap(ibm2);
				putbitmap(ibm3);

				filltable(false);

				putsprite(325,285,pause,0);
				putsprite(0,320,enter,0);

				//clear place of score
				putimagetoscreen(290,0,399,31,290,0,bi,1);
				//show score
				showscore(290,0,score);
				//show lifes
				showscore(475,0,bonus);
			}
			else if (g_select_new_game_active)
			{
				loadbmp(0,0,"greenscr.gra");
				int passimg=loadbmptoptr("pass.gra");
				putsprite(130,400,passimg,0);
				prelease();

				valloc(0);
				setfillcolor(0x00009600); // Green fill: RGB(0, 150, 0)
				bar(200, 480, 380, 530);
				setcolor(0x00FFFFFF); // White border/text: RGB(255, 255, 255)
				rectangle(200, 480, 380, 530);

				setfillcolor(0x000000B4); // Red fill: RGB(180, 0, 0)
				bar(420, 480, 600, 530);
				setcolor(0x00FFFFFF); // White border/text
				rectangle(420, 480, 600, 530);
				vfree();

				// Draw button text using sprite characters
				putsprite(245, 490, c['y' - 'a'], 0);
				putsprite(275, 490, c['e' - 'a'], 0);
				putsprite(305, 490, c['s' - 'a'], 0);

				putsprite(480, 490, c['n' - 'a'], 0);
				putsprite(510, 490, c['o' - 'a'], 0);
			}
			else
			{
				putbitmap(nbitmap);
				putimage(0,0,bi,0);
			}
			if (putopt) putsprite(315,270+ns*50,zast[ns][1],0);
		}
	}
	
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPackmanDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CPackmanDlg::OnDestroy() 
{
	dsclose();
	closesound();
	//::KillTimer(mwnd,1);
	g_bTimerWorking=false;
	//untrapsyskeys();
	resetkeyboard();
	closekeyboard();
	closegraph();	 
	CDialog::OnDestroy();
}

LRESULT CPackmanDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_DSHOW)
	{
		dsevent();
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CPackmanDlg::OnOK()
{
	return;
}


void CPackmanDlg::OnCancel()
{

}

void CPackmanDlg::OnKillFocus(CWnd* pNewWnd)
{
	(void)pNewWnd;
}

void CPackmanDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	(void)nFlags;
	if ((point.x>315)&&(point.x<530))
	{
		if ((point.y>=270)&&(point.y<=300)) {ns=0;mpressed=true;}
		else if ((point.y>=320)&&(point.y<=350)) {ns=1;mpressed=true;}
		else if ((point.y>=370)&&(point.y<=400)) {ns=2;mpressed=true;}
		else if ((point.y>=420)&&(point.y<=450)) {ns=3;mpressed=true;}
		else if ((point.y>=470)&&(point.y<=500)) {ns=4;mpressed=true;}
		else if ((point.y>=520)&&(point.y<=550)) {ns=5;mpressed=true;}
	}
}

void CPackmanDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	(void)nFlags;
	if ((point.x>315)&&(point.x<530))
	{
		if ((point.y>=270)&&(point.y<=300)) {ns=0;}
		else if ((point.y>=320)&&(point.y<=350)) {ns=1;}
		else if ((point.y>=370)&&(point.y<=400)) {ns=2;}
		else if ((point.y>=420)&&(point.y<=450)) {ns=3;}
		else if ((point.y>=470)&&(point.y<=500)) {ns=4;}
		else if ((point.y>=520)&&(point.y<=550)) {ns=5;}
	}
}

