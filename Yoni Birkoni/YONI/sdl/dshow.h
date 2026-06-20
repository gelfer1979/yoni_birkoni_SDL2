#ifndef SDL_DSHOW_H
#define SDL_DSHOW_H

#include "compat.h"

#define WM_DSHOW (WM_APP + 1)



bool dsinit(HWND mwnd);
void dsclose();
bool dsplay(const char *fname, bool loop);
bool dsstop();
bool dsstart();
void dsevent();

#endif // SDL_DSHOW_H
