//!!!required strmiids.lib in link options/object modules
//!!! requred:
// ::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
// {
//	if (message==WM_DSHOW)
//	{
//		dsevent();
//	}

#define WM_DSHOW WM_APP + 1

bool dsinit(HWND mwnd);//init dshow interface

void dsclose();//close dshow interface

bool dsplay(char *fname,bool loop);//play file

bool dsstop();//stop playing

bool dsstart();//start playing

void dsevent(); //dshow event