//---------------------------------------------------------------------------
#include "stdafx.h"
#include "sound.h"
#include <mmreg.h>
#include <dsound.h>


const DWORD DSBCAPS_CTRLALL=DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN\
	|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPOSITIONNOTIFY;
static LPDIRECTSOUND dsnd;
static HRESULT res;
static DSBUFFERDESC bufdesc;
static LPDIRECTSOUNDBUFFER *sbuf;
static unsigned long int tbuf=0;
bool is=true;//sound is not initialised

bool initsound (HWND hwnd,unsigned long int daccess,unsigned long numbuf)
{
	res=DirectSoundCreate((GUID*)NULL,&dsnd,NULL);
	if (res!=DS_OK) return false;
	res=dsnd->SetCooperativeLevel(hwnd,daccess);
	if (res!=DS_OK) return false;
	bufdesc.dwSize=sizeof(bufdesc);
	sbuf=new LPDIRECTSOUNDBUFFER [numbuf];
	is=false;
	return true;
}

unsigned long int loadwav(char *mpath)
    {
	 if (is) return 0;
     static unsigned char *mbuf;
     HFILE fl=_lopen(mpath,OF_READ|OF_SHARE_COMPAT);
     if (fl==HFILE_ERROR) return 0;
     LONG filesize=_llseek(fl,0,FILE_END);
     _llseek(fl,0,FILE_BEGIN);
     HGLOBAL mglob=GlobalAlloc(GMEM_MOVEABLE,filesize);
     if (mglob==NULL) return 0;
     mbuf=(unsigned char *)GlobalLock(mglob);
     if (_hread(fl,mbuf,filesize)!=filesize) return 0;
     _lclose(fl);
     bufdesc.dwFlags=DSBCAPS_CTRLALL|DSBCAPS_STATIC|DSBCAPS_STICKYFOCUS; 
	 //int kk;memcpy(&kk,mbuf+0x010,4);
	 //kk+=20;
	 int kk=19;int zz;
	 do {
		 kk++;
		 memcpy(&zz,mbuf+kk,4);
	 } while (zz!=0x061746164);//while zz!="data";
	 kk+=4;zz=kk+4;
	 memcpy(&kk,mbuf+kk,4);
     bufdesc.dwBufferBytes=kk;
     bufdesc.lpwfxFormat=LPWAVEFORMATEX(mbuf+20);
     res=dsnd->CreateSoundBuffer(&bufdesc,&sbuf[tbuf],NULL);
     if (res!=DS_OK) return 0;
     unsigned long msize;
     void *mbuf1=mbuf;
     sbuf[tbuf]->Lock(0,kk,&mbuf1,&msize,NULL,0,NULL);
     memcpy(mbuf1,mbuf+zz,msize);
     sbuf[tbuf]->Unlock(mbuf+zz,msize,NULL,0);
     mbuf1=NULL;
     GlobalUnlock(mglob);
     GlobalFree(mglob);
     tbuf++;
     return tbuf;
    }

void pdestroybuf()
{
	tbuf--;
	sbuf[tbuf]->Release(); 
// apply buffer
}

unsigned long int loadwavparth(char *mpath,unsigned long int filesize)
    {
	 if (is) return 0;
     static unsigned char *mbuf;
     HFILE fl=_lopen(mpath,OF_READ|OF_SHARE_COMPAT);
     if (fl==HFILE_ERROR) return 0;
     HGLOBAL mglob=GlobalAlloc(GMEM_MOVEABLE,filesize+44);
     if (mglob==NULL) return 0;
     mbuf=(unsigned char *)GlobalLock(mglob);
     if (_hread(fl,mbuf,filesize+44)!=(signed long int)filesize+44) return 0;
     _lclose(fl);
     bufdesc.dwFlags=DSBCAPS_CTRLALL|DSBCAPS_STICKYFOCUS;
     bufdesc.dwBufferBytes=filesize;
     bufdesc.lpwfxFormat=LPWAVEFORMATEX(mbuf+20);
     res=dsnd->CreateSoundBuffer(&bufdesc,&sbuf[tbuf],NULL);
     if (res!=DS_OK) return 0;
     unsigned long msize;
     void *mbuf1=mbuf;
     sbuf[tbuf]->Lock(0,filesize,&mbuf1,&msize,NULL,0,NULL);
     memcpy(mbuf1,mbuf+44,msize);
     sbuf[tbuf]->Unlock(mbuf1,msize,NULL,0);
     mbuf1=NULL;
     GlobalUnlock(mglob);
     GlobalFree(mglob);
   /*  LPDIRECTSOUNDNOTIFY mnot;
     res=sbuf[tbuf]->QueryInterface(IID_IDirectSoundNotify,&mnot);
     if (res!=DS_OK) return 0;
     HANDLE ph=CreateEvent(NULL,false,false,&applysbbuf);
     DSBPOSITIONNOTIFY posn;
     posn.Offset=DSBPN_OFFSETSTOP;
     posn.hEventNotify=ph; */

     tbuf++;
     return tbuf;
    }


unsigned long int getbufsize(unsigned long int nbuf)
    {
	 if (is) return 0;
     DSBCAPS bcaps;
     bcaps.dwSize=sizeof(bcaps);
     res=sbuf[nbuf-1]->GetCaps(&bcaps);
     if (res!=DS_OK) return 0; else return bcaps.dwBufferBytes;
    }

bool playbuf(unsigned long int nbuf,bool mloop)
    {
	 if (is) return false;
     if (mloop) res=sbuf[nbuf-1]->Play(0,0,DSBPLAY_LOOPING);
     else res=sbuf[nbuf-1]->Play(0,0,NULL);
     if (res!=DS_OK) return false; else return true;
    }

unsigned long int getbufpos(unsigned long int nbuf)
    {
	 if (is) return 0;
     unsigned long int pos1,pos2;
     sbuf[nbuf-1]->GetCurrentPosition(&pos1,&pos2);
     return pos1;
    }

bool setbufpos(unsigned long int nbuf,unsigned long int npos)
    {
	 if (is) return false;
     res=sbuf[nbuf-1]->SetCurrentPosition(npos);
     if (res!=DS_OK) return false; else return true;
    }

bool stopbuf(unsigned long int nbuf)
    {
	 if (is) return false;
     res=sbuf[nbuf-1]->Stop();
     if (res!=DS_OK) return false; else return true;
    }

bool bufplaying (unsigned long int nbuf)
    {
	 if (is) return false;
     unsigned long int npos;
     res=sbuf[nbuf-1]->GetStatus(&npos);
     if ((npos|DSBSTATUS_LOOPING)==(DSBSTATUS_PLAYING|DSBSTATUS_LOOPING)) return true; else
     return false;
    }

long int getbufvol (unsigned long int nbuf)
    {
	 if (is) return 0;
     long int yy;
          res=sbuf[nbuf-1]->GetVolume(&yy);
     if (res!=DS_OK) return 0; else return yy;
    }

bool setbufvol (unsigned long int nbuf,long int yy)
    {
	 if (is) return false;
          res=sbuf[nbuf-1]->SetVolume(yy);
     if (res!=DS_OK) return false; else return true;
    }

unsigned long int getbuffreq (unsigned long int nbuf)
    {
	 if (is) return 0;
     unsigned long int yy;
          res=sbuf[nbuf-1]->GetFrequency(&yy);
     if (res!=DS_OK) return 0; else return yy;
    }

bool setbuffreq (unsigned long int nbuf,unsigned long int yy)
    {
	 if (is) return false;
          res=sbuf[nbuf-1]->SetFrequency(yy);
     if (res!=DS_OK) return false; else return true;
    }

long int getbufpan (unsigned long int nbuf)
    {
	 if (is) return false;
     long int yy;
          res=sbuf[nbuf-1]->GetPan(&yy);
     if (res!=DS_OK) return 1; else return yy;
    }

bool setbufpan (unsigned long int nbuf,long int yy)
    {
	 if (is) return false;
          res=sbuf[nbuf-1]->SetPan(yy);
     if (res!=DS_OK) return false; else return true;
    }

void closesound()
{
	if (is) return;
	delete sbuf;
}
//CreateEvent(NULL,false,false,applysbbuf);


//---------------------------------------------------------------------------

