#ifndef SDL_GRAPH_H
#define SDL_GRAPH_H

#include "compat.h"



bool _fastcall initgraph(HWND mwin, int sizex, int sizey, int bpp, int numbbuf, int numimages, int numbitmap);
HDC valloc(int vnum);
void vfree();
bool flip(int numpage);
void setbkcolor(int mcolor);
void cleardevice();
void setcolor(int mcolor);
void setlinewidth(int width);
bool setfontindirect(LOGFONT *lf);
bool setfont(const char *fontname = "Times New Roman", DWORD charset = DEFAULT_CHARSET,
             int height = 0, int width = 0, int orientation = 0, int weight = 0, bool italic = false,
             bool underline = false, bool strikeout = false);
void textout(int mx, int my, const char *mstr);
void moveto(int mx, int my);
void lineto(int mx, int my);
void line(int mx, int my, int mx1, int my1);
void rectangle(int mx, int my, int mx1, int my1);
void setfillcolor(int mcol);
void bar(int mx, int my, int mx1, int my1);
void fillcircle(int sx, int sy, int rad);
void fillellipse(int sx, int sy, int rad, int rae);
void circle(int sx, int sy, int rae);
void ellipse(int sx, int sy, int rae, int ral);
void putpixel(int sx, int sy, int colr);
int getpixel(int sx, int sy);
bool loadbmp(int mx, int my, const char *mpath);
int loadbmptoptr(const char *mpath);
bool putsprite(int mx, int my, int bnum, int mpage);
bool putimage(int mx, int my, int bnum, int mpage);
int getimage(int mx, int my, int mx1, int my1, int mpage);
bool virtputimage(int mx, int my, int bnum, int bnum1);
bool virtputsprite(int mx, int my, int bnum, int bnum1);
int virtgetimage(int mx, int my, int mx1, int my1, int mpage);
bool putimagetoscreen(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mpage);
bool putspritetoscreen(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mpage);
bool putimagetoptr(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mnum1);
bool putspritetoptr(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mnum1);
void prelease();
void release(int n);
void closegraph();
void restoreimage(int num);
void restorescreen();
bool reloadbmptoptr(int bnum, const char *mpath);
int fillbitmap(int numi);
bool freebitmap();
bool updatebitmap(int mx, int my, int width1, int height1, int bnum, int bmnum);
bool removebitmap();
bool putbitmap(int nbm);

#endif // SDL_GRAPH_H
