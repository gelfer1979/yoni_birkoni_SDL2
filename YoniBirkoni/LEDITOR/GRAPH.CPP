//---------------------------------------------------------------------------
#include "stdafx.h"
#include "graph.h"
#include <ddraw.h>
//---------------------------------------------------------------------------
//#pragma package(smart_init)

static LPDIRECTDRAW ddraw; // Any DirectDraw object
static HRESULT res;        // results of DirectDraw operation
static DDSURFACEDESC pscreen; // destination surface structure
static LPDIRECTDRAWSURFACE dscreen,*backb,*offb; // videopage
static DDSCAPS caps;   // creating attributes
static HDC dwnd;     // pointer of ddraw window
static int vnum1=-1,tbnum,toffnum,offnum=0;      // number of active page
static HPEN pen1;      // active pen
static HBRUSH brush1;  // active brush
static int bkcol;      // background color
static int maxx,maxy;
static HWND awin;//application window handler


bool _fastcall initgraph(HWND mwin,int sizex,int sizey,int bpp,int numbbuf,int numoffss)
{
	awin=mwin;
       if (DirectDrawCreate (NULL,&ddraw,NULL)!=DD_OK) return false;
       if (ddraw->SetCooperativeLevel (mwin,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN)!=DD_OK)
                                                       return false;
       tbnum=numbbuf;toffnum=numoffss;
       if (ddraw->SetDisplayMode(sizex,sizey,bpp)!=DD_OK) return false;
       if (numbbuf>0)
          {
           pscreen.dwSize = sizeof(pscreen);
           pscreen.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
           pscreen.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
           DDSCAPS_FLIP | DDSCAPS_COMPLEX;
           pscreen.dwBackBufferCount = numbbuf;
           res=ddraw->CreateSurface(&pscreen,&dscreen,NULL);
                     if (res!=DD_OK) return false;
           backb= new LPDIRECTDRAWSURFACE [numbbuf];
           caps.dwCaps = DDSCAPS_BACKBUFFER;
           for (int i=0;i<numbbuf;i++)
           {
             res=dscreen->GetAttachedSurface(&caps,&backb[i]);
                                               if (res!=DD_OK) return false;
           }
          }
       else
          {
           pscreen.dwSize = sizeof(pscreen);
           pscreen.dwFlags = DDSD_CAPS;
           pscreen.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
           res=ddraw->CreateSurface(&pscreen,&dscreen,NULL);
                     if (res!=DD_OK) return false;
          }
		if ((dscreen->GetDC(&dwnd))!=DD_OK) return false;
          /*SetTextColor(dwnd,RGB(255,255,255));
          pen1=CreatePen(PS_SOLID,1,RGB(255,255,255));
          SelectObject(dwnd,pen1);
          brush1=CreateSolidBrush(RGB(255,255,255));
          SelectObject(dwnd,brush1);*/
          dscreen->ReleaseDC(dwnd);
          maxx=sizex-1;maxy=sizey-1;
          if (numoffss>0) offb=new LPDIRECTDRAWSURFACE [numoffss];
return true;
    }


HDC valloc(int vnum)
        {
          if (vnum==0) dscreen->GetDC(&dwnd);
                       else backb[vnum-1]->GetDC(&dwnd);
           vnum1=vnum;
		   setcolor(RGB(255,0,0));
          SelectObject(dwnd,pen1);
          SelectObject(dwnd,brush1);
		  return dwnd;
        }


void vfree()
        {
        if (vnum1==0) dscreen->ReleaseDC(dwnd);
        else if (vnum1!=-1) backb[vnum1-1]->ReleaseDC(dwnd);
         vnum1=-1;
   }


bool flip (int numpage)
   { if (numpage==0)
     {
      do {
           res=dscreen->Flip(NULL,DDFLIP_WAIT);
           if (res==DDERR_SURFACELOST) dscreen->Restore();
         } while (res==DDERR_SURFACELOST);

      if (res!=DD_OK) return false;
     }
   else
     {

      do {
           res=dscreen->Flip(backb[numpage-1],DDFLIP_WAIT);
           if (res==DDERR_SURFACELOST) dscreen->Restore();
         } while (res==DDERR_SURFACELOST);

      if (res!=DD_OK) return false;
     }
return true;
}

void setbkcolor(int mcolor)
        {
         SetBkColor (dwnd,mcolor);
         bkcol=mcolor;
        }

void cleardevice ()
        {
        RECT r1;
        r1.left=0;r1.top=0;r1.right=maxx+1;r1.bottom=maxy+1;
         FillRect (dwnd,&r1,CreateSolidBrush(bkcol));
        }

void setcolor (int mcolor)
        {
          SetTextColor (dwnd,mcolor);
          DeleteObject(pen1);
          pen1=CreatePen(PS_SOLID,1,mcolor);
          SelectObject(dwnd,pen1);
        }
void textout (int mx,int my,char *mstr)
        {
          TextOut(dwnd,mx,my,mstr,strlen(mstr));
        }

bool setfont(char *fontname,DWORD charset,
			 int height,int width,int orientation,int weight,bool italic,
			 bool underline,bool strikeout)
		{
			HFONT f=CreateFont(height,width,orientation,orientation,weight,(DWORD)italic,
				(DWORD)underline,(DWORD)strikeout,charset,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,fontname);
			if (f==NULL) return false;
			SelectObject(dwnd,f);
			return true;
		}

bool setfontindirect(LOGFONT *lf)
		{
			HFONT f=CreateFontIndirect(lf);
			if (f==NULL) return false;
			SelectObject(dwnd,f);
			return true;
		}

void moveto(int mx,int my)
        {
          MoveToEx (dwnd,mx,my,NULL);
        }

void lineto (int mx,int my)
        {
          LineTo (dwnd,mx,my);
        }

void line (int mx,int my,int mx1,int my1)
        {
          MoveToEx (dwnd,mx,my,NULL);
          LineTo (dwnd,mx1,my1);
        }

void rectangle(int mx,int my,int mx1,int my1)
        {
         MoveToEx(dwnd,mx,my,NULL);LineTo(dwnd,mx1,my);LineTo(dwnd,mx1,my1);
         LineTo(dwnd,mx,my1);LineTo(dwnd,mx,my);
        }

void setfillcolor (int mcol)
        {
        DeleteObject(brush1);
        brush1=CreateSolidBrush(mcol);
        SelectObject(dwnd,brush1);
        }

void bar (int mx,int my,int mx1,int my1)
        {
         Rectangle (dwnd,mx,my,mx1,my1);
        }

void fillcircle (int sx,int sy,int rad)
        {
         Ellipse (dwnd,sx-rad,sy-rad,sx+rad,sy+rad);
        }

void fillellipse (int sx,int sy,int rad,int rae)
        {
         Ellipse (dwnd,sx-rad,sy-rae,sx+rad,sy+rae);
        }

void circle (int sx,int sy,int rae)
        {
         Arc (dwnd,sx-rae,sy-rae,sx+rae,sy+rae,sx+rae,sy+rae,sx+rae,sy+rae);
        }

void ellipse (int sx,int sy,int rae,int ral)
        {
         Arc (dwnd,sx-rae,sy-ral,sx+rae,sy+ral,sx+rae,sy+ral,sx+rae,sy+ral);
        }

void putpixel(int sx,int sy,int colr)
        {
         SetPixelV(dwnd,sx,sy,colr);
        }

int getpixel(int sx,int sy)
        {
         return GetPixel(dwnd,sx,sy);
        }

bool loadbmp (int mx,int my,char *mpath)
        {
         HBITMAP hbm;
         HDC hdcimage;
         hbm = (HBITMAP)LoadImage(NULL, mpath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
         if (hbm==NULL) return false;
         hdcimage = CreateCompatibleDC(NULL);
         if (!hdcimage) return false;
         SelectObject(hdcimage, hbm);
		 if ((dscreen->GetDC(&dwnd))!=DD_OK) return false;
      if (BitBlt(dwnd,mx, my, maxx+1,maxy+1,hdcimage,0,0,SRCCOPY)==false)
                                                      return false;

	DeleteObject (hbm);
    DeleteDC(hdcimage);
	dscreen->ReleaseDC(dwnd);

     return true;
        }

int loadbmptoptr(char *mpath)
        {
         HBITMAP hbm;
         HDC hdcimage,hdc;
         BITMAP bm;
         hbm = (HBITMAP)LoadImage(NULL, mpath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
         if (hbm==NULL) return -1;
         hdcimage = CreateCompatibleDC(NULL);
         if (!hdcimage) return -1;

         SelectObject(hdcimage, hbm);
         GetObject(hbm,sizeof(bm),&bm);

         pscreen.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	 pscreen.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	 pscreen.dwHeight = bm.bmHeight;
	 pscreen.dwWidth = bm.bmWidth;
	 res = ddraw->CreateSurface(&pscreen, &offb[offnum], NULL);
                                     if (res!=DD_OK) return -1;
        offb[offnum]->GetDC(&hdc);

         if (BitBlt(hdc, 0, 0,bm.bmWidth,bm.bmHeight,hdcimage,0,0,SRCCOPY)==false)
                                                      return -1;
         offb[offnum]->ReleaseDC(hdc);
         DeleteDC(hdcimage);
         DeleteObject (hbm);
         offnum++;
         return offnum-1;
        }

bool putsprite (int mx,int my,int bnum,int mpage)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;
     pscreen.dwSize = sizeof(pscreen);
     pscreen.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;

     

	 
	 res=offb[bnum]->GetSurfaceDesc(&pscreen);
         if (res!=DD_OK) return false; 
     rect2.left=0;rect2.top=0;
     rect1.left=mx;rect1.top=my;
      rect2.right=pscreen.dwWidth-1;
      rect2.bottom=pscreen.dwHeight-1;
     rect1.right=mx+pscreen.dwWidth-1;
     rect1.bottom=my+pscreen.dwHeight-1;
     if (mx<0) {rect1.left=0;rect2.left=-mx;}
     if (my<0) {rect1.top=0;rect2.top=-my;}
     if (rect1.right>maxx) {rect1.right=maxx;rect2.right=maxx-mx;}
     if (rect1.bottom>maxy) {rect1.bottom=maxy;rect2.bottom=maxy-my;}
     mblt.dwSize=sizeof(mblt);
     mblt.ddckSrcColorkey.dwColorSpaceLowValue=0;
     mblt.ddckSrcColorkey.dwColorSpaceHighValue=0;
     if (mpage==0)
     res=dscreen->Blt(&rect1,offb[bnum],&rect2,DDBLT_WAIT|DDBLT_KEYSRCOVERRIDE,&mblt);
     else res=backb[mpage-1]->Blt(&rect1,offb[bnum],&rect2,DDBLT_WAIT|DDBLT_KEYSRCOVERRIDE,&mblt);
	       if (res!=DD_OK) return false;
                 else return true;
    }

/*bool putimage (int mx,int my,int bnum,int mpage)
    {
     HDC dwnd1;
	 offb[bnum]->GetDC(&dwnd1);
	 BOOL z=BitBlt(dwnd, mx, my, maxx+1,maxy+1,dwnd1,0,0,SRCCOPY);
	 offb[bnum]->ReleaseDC(dwnd1);
	 if (!z) return false; else return true;
    } */

bool putimage (int mx,int my,int bnum,int mpage)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;
     pscreen.dwSize = sizeof(pscreen);
     pscreen.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
     res=offb[bnum]->GetSurfaceDesc(&pscreen);
         if (res!=DD_OK) return false;
     rect2.left=0;rect2.top=0;
     rect1.left=mx;rect1.top=my;
      rect2.right=pscreen.dwWidth-1;
      rect2.bottom=pscreen.dwHeight-1;
     rect1.right=mx+pscreen.dwWidth-1;
     rect1.bottom=my+pscreen.dwHeight-1;
     if (mx<0) {rect1.left=0;rect2.left=-mx;}
     if (my<0) {rect1.top=0;rect2.top=-my;}
     if (rect1.right>maxx) {rect1.right=maxx;rect2.right=maxx-mx;}
     if (rect1.bottom>maxy) {rect1.bottom=maxy;rect2.bottom=maxy-my;}
     mblt.dwSize=sizeof(mblt);
     if (mpage==0)
     res=dscreen->Blt(&rect1,offb[bnum],&rect2,DDBLT_WAIT,&mblt);
     else res=backb[mpage-1]->Blt(&rect1,offb[bnum],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return false;
                 else return true;
    }


int getimage (int mx,int my,int mx1,int my1,int mpage)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;

     if (mx>mx1) {int mx2=mx;mx=mx1;mx1=mx2;}
     if (my>my1) {int mx2=my;my=my1;my1=mx2;}

     pscreen.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
     pscreen.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
     pscreen.dwHeight = my1-my;
     pscreen.dwWidth = mx1-mx;
     res = ddraw->CreateSurface(&pscreen, &offb[offnum], NULL);
         if (res!=DD_OK) return -1;
     rect2.left=mx;rect2.top=my;
     rect1.left=0;rect1.top=0;
      rect2.right=mx1;rect2.bottom=my1;
     rect1.right=mx1-mx;
     rect1.bottom=my1-my;
     mblt.dwSize=sizeof(mblt);
     if (mpage==0)
     res=offb[offnum]->Blt(&rect1,dscreen,&rect2,DDBLT_WAIT,&mblt);
     else res=offb[offnum]->Blt(&rect1,backb[mpage-1],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return -1;
     offnum++;
     return offnum-1;
    }

bool virtputimage(int mx,int my,int bnum,int bnum1)
    {
      int maxx,maxy;
         DDBLTFX mblt;
     RECT rect1,rect2;
     pscreen.dwSize = sizeof(pscreen);
     pscreen.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
     res=offb[bnum]->GetSurfaceDesc(&pscreen);
         if (res!=DD_OK) return false;
     rect2.left=0;rect2.top=0;
     rect1.left=mx;rect1.top=my;
      rect2.right=pscreen.dwWidth-1;
      rect2.bottom=pscreen.dwHeight-1;
     rect1.right=mx+pscreen.dwWidth-1;
     rect1.bottom=my+pscreen.dwHeight-1;
     if (mx<0) {rect1.left=0;rect2.left=-mx;}
     if (my<0) {rect1.top=0;rect2.top=-my;}
     res=offb[bnum1]->GetSurfaceDesc(&pscreen);
     maxx=pscreen.dwWidth-1;
     maxy=pscreen.dwHeight-1;
     if (rect1.right>maxx) {rect1.right=maxx;rect2.right=maxx-mx;}
     if (rect1.bottom>maxy) {rect1.bottom=maxy;rect2.bottom=maxy-my;}
     mblt.dwSize=sizeof(mblt);
     res=offb[bnum1]->Blt(&rect1,offb[bnum],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return false;
     return true;
    }

bool virtputsprite(int mx,int my,int bnum,int bnum1)
    {
      int maxx,maxy;
         DDBLTFX mblt;
     RECT rect1,rect2;
     pscreen.dwSize = sizeof(pscreen);
     pscreen.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
     res=offb[bnum]->GetSurfaceDesc(&pscreen);
         if (res!=DD_OK) return false;
     rect2.left=0;rect2.top=0;
     rect1.left=mx;rect1.top=my;
      rect2.right=pscreen.dwWidth-1;
      rect2.bottom=pscreen.dwHeight-1;
     rect1.right=mx+pscreen.dwWidth-1;
     rect1.bottom=my+pscreen.dwHeight-1;
     if (mx<0) {rect1.left=0;rect2.left=-mx;}
     if (my<0) {rect1.top=0;rect2.top=-my;}
     res=offb[bnum1]->GetSurfaceDesc(&pscreen);
     maxx=pscreen.dwWidth-1;
     maxy=pscreen.dwHeight-1;
     if (rect1.right>maxx) {rect1.right=maxx;rect2.right=maxx-mx;}
     if (rect1.bottom>maxy) {rect1.bottom=maxy;rect2.bottom=maxy-my;}
     mblt.dwSize=sizeof(mblt);
     mblt.ddckSrcColorkey.dwColorSpaceLowValue=0;
     mblt.ddckSrcColorkey.dwColorSpaceHighValue=0;
     res=offb[bnum1]->Blt(&rect1,offb[bnum],&rect2,DDBLT_WAIT|DDBLT_KEYSRCOVERRIDE,&mblt);
     if (res!=DD_OK) return false;
     return true;
    }

int virtgetimage (int mx,int my,int mx1,int my1,int mpage)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;

     if (mx>mx1) {int mx2=mx;mx=mx1;mx1=mx2;}
     if (my>my1) {int mx2=my;my=my1;my1=mx2;}

     pscreen.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
     pscreen.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
     pscreen.dwHeight = my1-my;
     pscreen.dwWidth = mx1-mx;
     res = ddraw->CreateSurface(&pscreen, &offb[offnum], NULL);
         if (res!=DD_OK) return -1;
     rect2.left=mx;rect2.top=my;
     rect1.left=0;rect1.top=0;
      rect2.right=mx1;rect2.bottom=my1;
     rect1.right=mx1-mx;
     rect1.bottom=my1-my;
     mblt.dwSize=sizeof(mblt);
     res=offb[offnum]->Blt(&rect1,offb[mpage],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return -1;
     offnum++;
     return offnum-1;
    }

bool putimagetoscreen (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mpage)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;

     if (mx>mx1) {int mx2=mx;mx=mx1;mx1=mx2;}
     if (my>my1) {int mx2=my;my=my1;my1=mx2;}


     rect2.left=mx;rect2.top=my;
     rect1.left=mx3;rect1.top=my3;
     rect2.right=mx1;rect2.bottom=my1;
     rect1.right=mx1-mx+mx3;
     rect1.bottom=my1-my+my3;
     if (rect1.right>maxx) {rect2.right-=rect1.right-maxx;rect1.right=maxx;}
     if (rect1.bottom>maxy) {rect2.bottom-=rect1.bottom-maxy;rect1.bottom=maxy;}
     mblt.dwSize=sizeof(mblt);
     if (mpage==0)
     res=dscreen->Blt(&rect1,offb[mnum],&rect2,DDBLT_WAIT,&mblt);
     else res=backb[mpage-1]->Blt(&rect1,offb[mnum],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return false;
     return true;
    }

bool putspritetoscreen (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mpage)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;

     if (mx>mx1) {int mx2=mx;mx=mx1;mx1=mx2;}
     if (my>my1) {int mx2=my;my=my1;my1=mx2;}


     rect2.left=mx;rect2.top=my;
     rect1.left=mx3;rect1.top=my3;
     rect2.right=mx1;rect2.bottom=my1;
     rect1.right=mx1-mx+mx3;
     rect1.bottom=my1-my+my3;
     if (rect1.right>maxx) {rect2.right-=rect1.right-maxx;rect1.right=maxx;}
     if (rect1.bottom>maxy) {rect2.bottom-=rect1.bottom-maxy;rect1.bottom=maxy;}
     mblt.dwSize=sizeof(mblt);
     mblt.ddckSrcColorkey.dwColorSpaceLowValue=0;
     mblt.ddckSrcColorkey.dwColorSpaceHighValue=0;
     if (mpage==0)
     res=dscreen->Blt(&rect1,offb[mnum],&rect2,DDBLT_WAIT|DDBLT_KEYSRCOVERRIDE,&mblt);
     else res=backb[mpage-1]->Blt(&rect1,offb[mnum],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return false;
     return true;
    }

bool putimagetoptr (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mnum1)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;
     int maxx,maxy;

     if (mx>mx1) {int mx2=mx;mx=mx1;mx1=mx2;}
     if (my>my1) {int mx2=my;my=my1;my1=mx2;}

     pscreen.dwSize = sizeof(pscreen);
     pscreen.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
     res=offb[mnum1]->GetSurfaceDesc(&pscreen);
         if (res!=DD_OK) return false;
     maxx=pscreen.dwWidth-1;
     maxy=pscreen.dwHeight-1;

     rect2.left=mx;rect2.top=my;
     rect1.left=mx3;rect1.top=my3;
     rect2.right=mx1;rect2.bottom=my1;
     rect1.right=mx1-mx+mx3;
     rect1.bottom=my1-my+my3;
     if (rect1.right>maxx) {rect2.right-=rect1.right-maxx;rect1.right=maxx;}
     if (rect1.bottom>maxy) {rect2.bottom-=rect1.bottom-maxy;rect1.bottom=maxy;}
     mblt.dwSize=sizeof(mblt);
     res=offb[mnum1]->Blt(&rect1,offb[mnum],&rect2,DDBLT_WAIT,&mblt);
     if (res!=DD_OK) return false;
     return true;
    }

bool putspritetoptr (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mnum1)
    {
     DDBLTFX mblt;
     RECT rect1,rect2;
     int maxx,maxy;

     if (mx>mx1) {int mx2=mx;mx=mx1;mx1=mx2;}
     if (my>my1) {int mx2=my;my=my1;my1=mx2;}

     pscreen.dwSize = sizeof(pscreen);
     pscreen.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
     res=offb[mnum1]->GetSurfaceDesc(&pscreen);
         if (res!=DD_OK) return false;
     maxx=pscreen.dwWidth-1;
     maxy=pscreen.dwHeight-1;

     rect2.left=mx;rect2.top=my;
     rect1.left=mx3;rect1.top=my3;
     rect2.right=mx1;rect2.bottom=my1;
     rect1.right=mx1-mx+mx3;
     rect1.bottom=my1-my+my3;
     if (rect1.right>maxx) {rect2.right-=rect1.right-maxx;rect1.right=maxx;}
     if (rect1.bottom>maxy) {rect2.bottom-=rect1.bottom-maxy;rect1.bottom=maxy;}
     mblt.dwSize=sizeof(mblt);
     mblt.ddckSrcColorkey.dwColorSpaceLowValue=0;
     mblt.ddckSrcColorkey.dwColorSpaceHighValue=0;
     res=offb[mnum1]->Blt(&rect1,offb[mnum],&rect2,DDBLT_WAIT|DDBLT_KEYSRCOVERRIDE,&mblt);
     if (res!=DD_OK) return false;
     return true;
    }

void prelease()
{
 offnum--;
 offb[offnum]->Release();
 offb[offnum]=NULL;
}

void closegraph ()
    {
      ddraw->RestoreDisplayMode();
	  if (tbnum>0) {
      for (int i=0;i<tbnum;i++)
          {
			  backb[i]->Release();
			  backb[i]=NULL;
          }
           delete [] backb;
          }

      if (offnum>0) {
      for (int i=0;i<offnum;i++)
          {
		  if (offb[i]!=NULL)
		  {
			  offb[i]->Release();
			  offb[i]=NULL;
		  }
          }
            delete [] offb;
          }
	  dscreen->Release();
      dscreen=NULL;
	  ddraw->SetCooperativeLevel (awin,DDSCL_NORMAL);
      ddraw->Release();
      ddraw=NULL;
         
    }

void release(int n)//trouble!!!!
{
	if (offb[n]!=NULL)
	{
		offb[n]->Release();
		offb[n]=NULL;
	}
	//offnum--;
	//for (int i=n;i<offnum;i++) offb[i]=offb[i+1];
}

