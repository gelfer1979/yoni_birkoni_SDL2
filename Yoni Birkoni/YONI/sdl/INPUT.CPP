#include "INPUT.H"
#include <queue>
#include <vector>

// Key translation table from SDL Scancodes to DirectInput scan codes
static SDL_Scancode dik_to_sdl_table[256];
static unsigned char sdl_to_dik_table[SDL_NUM_SCANCODES];
static bool tables_initialized = false;

static void init_key_tables() {
    if (tables_initialized) return;
    
    std::memset(dik_to_sdl_table, 0, sizeof(dik_to_sdl_table));
    std::memset(sdl_to_dik_table, 0, sizeof(sdl_to_dik_table));
    
    #define MAP_KEY(dik, sdl) \
        if (dik < 256) dik_to_sdl_table[dik] = sdl; \
        if (sdl < SDL_NUM_SCANCODES) sdl_to_dik_table[sdl] = dik;

    MAP_KEY(0x01, SDL_SCANCODE_ESCAPE);
    MAP_KEY(0x02, SDL_SCANCODE_1);
    MAP_KEY(0x03, SDL_SCANCODE_2);
    MAP_KEY(0x04, SDL_SCANCODE_3);
    MAP_KEY(0x05, SDL_SCANCODE_4);
    MAP_KEY(0x06, SDL_SCANCODE_5);
    MAP_KEY(0x07, SDL_SCANCODE_6);
    MAP_KEY(0x08, SDL_SCANCODE_7);
    MAP_KEY(0x09, SDL_SCANCODE_8);
    MAP_KEY(0x0A, SDL_SCANCODE_9);
    MAP_KEY(0x0B, SDL_SCANCODE_0);
    MAP_KEY(0x0C, SDL_SCANCODE_MINUS);
    MAP_KEY(0x0D, SDL_SCANCODE_EQUALS);
    MAP_KEY(0x0E, SDL_SCANCODE_BACKSPACE);
    MAP_KEY(0x0F, SDL_SCANCODE_TAB);
    
    MAP_KEY(0x10, SDL_SCANCODE_Q);
    MAP_KEY(0x11, SDL_SCANCODE_W);
    MAP_KEY(0x12, SDL_SCANCODE_E);
    MAP_KEY(0x13, SDL_SCANCODE_R);
    MAP_KEY(0x14, SDL_SCANCODE_T);
    MAP_KEY(0x15, SDL_SCANCODE_Y);
    MAP_KEY(0x16, SDL_SCANCODE_U);
    MAP_KEY(0x17, SDL_SCANCODE_I);
    MAP_KEY(0x18, SDL_SCANCODE_O);
    MAP_KEY(0x19, SDL_SCANCODE_P);
    MAP_KEY(0x1A, SDL_SCANCODE_LEFTBRACKET);
    MAP_KEY(0x1B, SDL_SCANCODE_RIGHTBRACKET);
    MAP_KEY(0x1C, SDL_SCANCODE_RETURN);
    MAP_KEY(0x1D, SDL_SCANCODE_LCTRL);
    
    MAP_KEY(0x1E, SDL_SCANCODE_A);
    MAP_KEY(0x1F, SDL_SCANCODE_S);
    MAP_KEY(0x20, SDL_SCANCODE_D);
    MAP_KEY(0x21, SDL_SCANCODE_F);
    MAP_KEY(0x22, SDL_SCANCODE_G);
    MAP_KEY(0x23, SDL_SCANCODE_H);
    MAP_KEY(0x24, SDL_SCANCODE_J);
    MAP_KEY(0x25, SDL_SCANCODE_K);
    MAP_KEY(0x26, SDL_SCANCODE_L);
    MAP_KEY(0x27, SDL_SCANCODE_SEMICOLON);
    MAP_KEY(0x28, SDL_SCANCODE_APOSTROPHE);
    MAP_KEY(0x29, SDL_SCANCODE_GRAVE);
    MAP_KEY(0x2A, SDL_SCANCODE_LSHIFT);
    MAP_KEY(0x2B, SDL_SCANCODE_BACKSLASH);
    
    MAP_KEY(0x2C, SDL_SCANCODE_Z);
    MAP_KEY(0x2D, SDL_SCANCODE_X);
    MAP_KEY(0x2E, SDL_SCANCODE_C);
    MAP_KEY(0x2F, SDL_SCANCODE_V);
    MAP_KEY(0x30, SDL_SCANCODE_B);
    MAP_KEY(0x31, SDL_SCANCODE_N);
    MAP_KEY(0x32, SDL_SCANCODE_M);
    MAP_KEY(0x33, SDL_SCANCODE_COMMA);
    MAP_KEY(0x34, SDL_SCANCODE_PERIOD);
    MAP_KEY(0x35, SDL_SCANCODE_SLASH);
    MAP_KEY(0x36, SDL_SCANCODE_RSHIFT);
    MAP_KEY(0x37, SDL_SCANCODE_KP_MULTIPLY);
    MAP_KEY(0x38, SDL_SCANCODE_LALT);
    MAP_KEY(0x39, SDL_SCANCODE_SPACE);
    MAP_KEY(0x3A, SDL_SCANCODE_CAPSLOCK);
    
    MAP_KEY(0x3B, SDL_SCANCODE_F1);
    MAP_KEY(0x3C, SDL_SCANCODE_F2);
    MAP_KEY(0x3D, SDL_SCANCODE_F3);
    MAP_KEY(0x3E, SDL_SCANCODE_F4);
    MAP_KEY(0x3F, SDL_SCANCODE_F5);
    MAP_KEY(0x40, SDL_SCANCODE_F6);
    MAP_KEY(0x41, SDL_SCANCODE_F7);
    MAP_KEY(0x42, SDL_SCANCODE_F8);
    MAP_KEY(0x43, SDL_SCANCODE_F9);
    MAP_KEY(0x44, SDL_SCANCODE_F10);
    MAP_KEY(0x45, SDL_SCANCODE_NUMLOCKCLEAR);
    MAP_KEY(0x46, SDL_SCANCODE_SCROLLLOCK);
    
    MAP_KEY(0x47, SDL_SCANCODE_KP_7);
    MAP_KEY(0x48, SDL_SCANCODE_KP_8);
    MAP_KEY(0x49, SDL_SCANCODE_KP_9);
    MAP_KEY(0x4A, SDL_SCANCODE_KP_MINUS);
    MAP_KEY(0x4B, SDL_SCANCODE_KP_4);
    MAP_KEY(0x4C, SDL_SCANCODE_KP_5);
    MAP_KEY(0x4D, SDL_SCANCODE_KP_6);
    MAP_KEY(0x4E, SDL_SCANCODE_KP_PLUS);
    MAP_KEY(0x4F, SDL_SCANCODE_KP_1);
    MAP_KEY(0x50, SDL_SCANCODE_KP_2);
    MAP_KEY(0x51, SDL_SCANCODE_KP_3);
    MAP_KEY(0x52, SDL_SCANCODE_KP_0);
    MAP_KEY(0x53, SDL_SCANCODE_KP_PERIOD);
    
    MAP_KEY(0x57, SDL_SCANCODE_F11);
    MAP_KEY(0x58, SDL_SCANCODE_F12);
    
    // Arrow keys
    MAP_KEY(0xC8, SDL_SCANCODE_UP);
    MAP_KEY(0xD0, SDL_SCANCODE_DOWN);
    MAP_KEY(0xCB, SDL_SCANCODE_LEFT);
    MAP_KEY(0xCD, SDL_SCANCODE_RIGHT);
    
    #undef MAP_KEY
    tables_initialized = true;
}

static SDL_Scancode dik_to_sdl(unsigned char dik) {
    init_key_tables();
    return dik_to_sdl_table[dik];
}

unsigned char sdl_to_dik(SDL_Scancode sdl) {
    init_key_tables();
    if (sdl >= SDL_NUM_SCANCODES) return 0;
    return sdl_to_dik_table[sdl];
}

// Mouse events queue
struct MouseEventInternal {
    int info;
    int data;
};
static std::vector<MouseEventInternal> g_mouse_events;
static size_t g_mouse_event_idx = 0;

// Key events queue
struct KeyEventInternal {
    int info;
    bool data;
};
static std::vector<KeyEventInternal> g_key_events;
static size_t g_key_event_idx = 0;

// Pressed key code queue for getkey()
static std::queue<int> g_pressed_keys;

// Public event helper called by main loop
void input_push_key_event(SDL_Scancode scancode, bool is_down) {
    unsigned char dik = sdl_to_dik(scancode);
    if (dik == 0) return;
    
    // For getkey()
    if (is_down) {
        g_pressed_keys.push(dik);
    }
    
    // For getbufkeys()
    g_key_events.push_back({ dik, is_down });
}

void input_push_mouse_motion(int xrel, int yrel) {
    if (xrel != 0) g_mouse_events.push_back({ mousex, xrel });
    if (yrel != 0) g_mouse_events.push_back({ mousey, yrel });
}

void input_push_mouse_button(int button, bool is_down) {
    int btn = (button == SDL_BUTTON_LEFT) ? lbutton :
              (button == SDL_BUTTON_RIGHT) ? rbutton : mbutton;
    g_mouse_events.push_back({ btn, is_down ? 128 : 0 });
}

bool initdinput() {
    init_key_tables();
    return true;
}

bool initmouse(void *mwnd, bool mlevel, int mbuffersize, bool axismode) {
    (void)mwnd; (void)mlevel; (void)mbuffersize; (void)axismode;
    g_mouse_events.clear();
    g_mouse_event_idx = 0;
    return true;
}

void getmouse(int &mx, int &my, bool &lb, bool &rb, bool &mb) {
    int x, y;
    Uint32 state = SDL_GetMouseState(&x, &y);
    mx = x;
    my = y;
    lb = (state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    rb = (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    mb = (state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
}

bool getbufmouse(int &minfo, int &mdata) {
    if (g_mouse_event_idx < g_mouse_events.size()) {
        minfo = g_mouse_events[g_mouse_event_idx].info;
        mdata = g_mouse_events[g_mouse_event_idx].data;
        g_mouse_event_idx++;
        return true;
    }
    return false;
}

void closemouse() {
    g_mouse_events.clear();
    g_mouse_event_idx = 0;
}

bool initkeyboard(void *mwnd, bool klevel, int kbuffersize) {
    (void)mwnd; (void)klevel; (void)kbuffersize;
    g_key_events.clear();
    g_key_event_idx = 0;
    while (!g_pressed_keys.empty()) g_pressed_keys.pop();
    return true;
}

bool getkeys(unsigned char ka[256]) {
    std::memset(ka, 0, 256);
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    for (int dik = 0; dik < 256; ++dik) {
        SDL_Scancode sdl = dik_to_sdl((unsigned char)dik);
        if (sdl != SDL_SCANCODE_UNKNOWN) {
            if (state[sdl]) {
                ka[dik] = 128;
            }
        }
    }
    return true;
}

bool presskey(unsigned char mkey) {
    SDL_Scancode sdl = dik_to_sdl(mkey);
    if (sdl != SDL_SCANCODE_UNKNOWN) {
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        return state[sdl] != 0;
    }
    return false;
}

bool getbufkeys(int &kinfo, bool &kdata) {
    if (g_key_event_idx < g_key_events.size()) {
        kinfo = g_key_events[g_key_event_idx].info;
        kdata = g_key_events[g_key_event_idx].data;
        g_key_event_idx++;
        return true;
    }
    return false;
}

int getkey() {
    if (!g_pressed_keys.empty()) {
        int k = g_pressed_keys.front();
        g_pressed_keys.pop();
        return k;
    }
    return 0;
}

void resetkeyboard() {
    while (!g_pressed_keys.empty()) g_pressed_keys.pop();
    g_key_events.clear();
    g_key_event_idx = 0;
}

void closekeyboard() {
    resetkeyboard();
}
