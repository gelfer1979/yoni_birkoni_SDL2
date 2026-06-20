   // Mouse block -----------------------------------------------------
#include "stdafx.h"
#include "input.h"
#include <dinput.h>
//---------------------------------------------------------------------------
static LPDIRECTINPUT8 dinp;
static LPDIRECTINPUTDEVICE8 dmouse,dkey;
static HRESULT res;
static DIMOUSESTATE mydata;
static DIDEVICEOBJECTDATA mydata1;
static DIPROPDWORD mprop;

bool initdinput()
     {
	try{
      res=DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&dinp,NULL);
         if (res!=DI_OK) return false;
         return true;
	}catch (...) {return false;}
     }

bool initmouse(void *mwnd,bool mlevel,int mbuffersize,bool axismode)
	{


         res=dinp->CreateDevice(GUID_SysMouse, &dmouse, NULL);
         if (res!=DI_OK) return false;

         res=dmouse->SetDataFormat(&c_dfDIMouse);
         if (res!=DI_OK) return false;

         if (mlevel) dmouse->SetCooperativeLevel((HWND)mwnd,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
         else dmouse->SetCooperativeLevel((HWND)mwnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

         mprop.diph.dwSize=sizeof(mprop);
         mprop.diph.dwHeaderSize=sizeof(DIPROPHEADER);
         mprop.diph.dwObj=0;
         mprop.diph.dwHow=DIPH_DEVICE;
         mprop.dwData=mbuffersize;

         res=dmouse->SetProperty(DIPROP_BUFFERSIZE,&mprop.diph);
         if (res!=DI_OK) return false;

         if (axismode)
         mprop.dwData=DIPROPAXISMODE_ABS;
         else mprop.dwData=DIPROPAXISMODE_REL;

         res=dmouse->SetProperty(DIPROP_AXISMODE,&mprop.diph);
         if (res!=DI_OK) return false;


         res=dmouse->Acquire();
         if (res!=DI_OK) return false;

         return true;
         }

void getmouse (int &mx,int &my,bool &lb,bool &rb,bool &mb)
         {
          do
            {
             res=dmouse->GetDeviceState(sizeof(mydata),&mydata);
             if (res==DIERR_INPUTLOST) res=dmouse->Acquire();
            }
          while (res==DIERR_INPUTLOST);
          if (res!=DI_OK) return ;
          mx=mydata.lX;
          my=mydata.lY;
          if (mydata.rgbButtons[0]&0x080) lb=true;else lb=false;
          if (mydata.rgbButtons[2]&0x080) mb=true;else mb=false;
          if (mydata.rgbButtons[1]&0x080) rb=true;else rb=false;
         }

bool getbufmouse(int &minfo,int &mdata)
         {

          DWORD numel=1;
           do
             {
              res=dmouse->GetDeviceData(sizeof(mydata1),&mydata1,&numel,0);
              if (res==DIERR_INPUTLOST) res=dmouse->Acquire();
             }
           while (res==DIERR_INPUTLOST);
           if (res==DI_BUFFEROVERFLOW)
            {
              minfo=-1;return true;
            }
           if (res!=DI_OK) return false;
           minfo=(long)mydata1.dwOfs;
           mdata=mydata1.dwData;
           return true;
         }

void closemouse()
         {
           dmouse->Unacquire();
           dmouse->Release();
           dmouse=NULL;
         }

// Keyboard block -----------------------------------------------------
bool initkeyboard(void *mwnd,bool klevel,int kbuffersize)
	{


          res=dinp->CreateDevice(GUID_SysKeyboard, &dkey, NULL);
         if (res!=DI_OK) return false;

         res=dkey->SetDataFormat(&c_dfDIKeyboard);
         if (res!=DI_OK) return false;

         if (klevel) dkey->SetCooperativeLevel((HWND)mwnd,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
         else dkey->SetCooperativeLevel((HWND)mwnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

         mprop.diph.dwSize=sizeof(mprop);
         mprop.diph.dwHeaderSize=sizeof(DIPROPHEADER);
         mprop.diph.dwObj=0;
         mprop.diph.dwHow=DIPH_DEVICE;
         mprop.dwData=kbuffersize;

         res=dkey->SetProperty(DIPROP_BUFFERSIZE,&mprop.diph);
         if (res!=DI_OK) return false;

         //res=dkey->Acquire();
         //if (res!=DI_OK) return false;

         return true;
         }

bool getkeys (unsigned char ka[256])
         {
			 if (dkey==NULL) return false;
          do
            {
             res=dkey->GetDeviceState(256,(LPVOID)&ka);
             if (res==DIERR_INPUTLOST) res=dkey->Acquire();
            }
          while (res==DIERR_INPUTLOST);
          if (res==DI_OK) return true; else return false;
         }

bool presskey(unsigned char mkey)
         {
			 if (dkey==NULL) return false;
          unsigned char ka[256];
          do
            {
			 res=dkey->Acquire();
             res=dkey->GetDeviceState(256,(LPVOID)&ka);
            }
          while (res==DIERR_INPUTLOST);
          if (res!=DI_OK) return false;
          if ((ka[mkey]&0x080)==0) return false;
          else return true;
         } 

bool getbufkeys(int &kinfo,bool &kdata)
         {
			 if (dkey==NULL) return false;
          DWORD numel=1;
           do
             {
			  dkey->Acquire();
              res=dkey->GetDeviceData(sizeof(mydata1),&mydata1,&numel,0);
             }
           while (res==DIERR_INPUTLOST);
           if (res==DI_BUFFEROVERFLOW)
            {
              kinfo=-1;return true;
            }
           if (res!=DI_OK) return false;
           kinfo=mydata1.dwOfs;
           if ((mydata1.dwData&0x080)>0) kdata=true;else kdata=false;
           return true;
         }

int getkey()
{
	static int k,k1=0;
	bool z;
	getbufkeys(k,z);
	if (z) {if (k!=k1) {k1=k;return k;}}
		else k1=0;
	return 0;
}

void resetkeyboard()
{
	if (dkey==NULL) return;
	dkey->Acquire();

	//DIPROPDWORD za;
	//za.diph.dwSize       = sizeof(DIPROPDWORD); 
	//za.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	//za.diph.dwObj        = 0; // device property 
	//za.diph.dwHow        = DIPH_DEVICE; 
	//res=dkey->GetProperty(DIPROP_BUFFERSIZE,&za.diph);
	//DWORD numel=za.dwData;
	//TRACE("%d ",numel);
	//DIDEVICEOBJECTDATA *zb=new DIDEVICEOBJECTDATA[numel];
	//dkey->GetDeviceData(sizeof(zb),&zb,&numel,0);
	DWORD numel=INFINITE;
	res=dkey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),NULL,&numel,0);
}


void closekeyboard()
         {
			 if (dkey==NULL) return;
           dkey->Unacquire();
           dkey->Release();
           dkey=NULL;
         } 





//---------------------------------------------------------------------------


