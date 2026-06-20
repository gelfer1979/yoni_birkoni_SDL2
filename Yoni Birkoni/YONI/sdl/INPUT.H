#ifndef SDL_INPUT_H
#define SDL_INPUT_H

#include "compat.h"

const int lbutton = 12, rbutton = 13, mbutton = 14,
          mousex = 0, mousey = 4, moverflow = -1;

// Joystick states for touch control
extern bool g_joystick_active;
extern float g_joystick_center_x, g_joystick_center_y;
extern float g_joystick_curr_x, g_joystick_curr_y;
extern int g_joystick_dir;
extern bool g_select_new_game_active;

void input_push_simulated_key(unsigned char dik, bool is_down);
void start_text_input();
void stop_text_input();

bool initdinput();
bool initmouse(void *mwnd, bool mlevel, int mbuffersize, bool axismode);
void getmouse(int &mx, int &my, bool &lb, bool &rb, bool &mb);
bool getbufmouse(int &minfo, int &mdata);
void closemouse();

bool initkeyboard(void *mwnd, bool klevel, int kbuffersize);
bool getkeys(unsigned char ka[256]);
bool presskey(unsigned char mkey);
bool getbufkeys(int &kinfo, bool &kdata);
int getkey();
void resetkeyboard();
void closekeyboard();

#endif // SDL_INPUT_H
