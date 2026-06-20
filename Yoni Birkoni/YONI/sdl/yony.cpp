#include "compat.h"
#include "GRAPH.H"
#include "INPUT.H"
#include "sound.h"
#include "dshow.h"
#define protected public
#include "yonyDlg.h"
#undef protected

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global function pointer and logic from YonyDlg.cpp
extern void (*bg)();
extern void begingame();
extern void loadimages();

// Public input queues in INPUT.CPP
extern void input_push_key_event(SDL_Scancode scancode, bool is_down);
extern void input_push_mouse_motion(int xrel, int yrel);
extern void input_push_mouse_button(int button, bool is_down);

// Global CPackmanDlg instance
CPackmanDlg theDlg;

// PeekAndPump handles the event loop and passes inputs to MFC message handlers
BOOL PeekAndPump() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                closegraph();
                closesound();
                dsclose();
                std::exit(0);
                break;
                
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                input_push_key_event(event.key.keysym.scancode, event.type == SDL_KEYDOWN);
                break;
                
            case SDL_MOUSEMOTION:
                input_push_mouse_motion(event.motion.xrel, event.motion.yrel);
                theDlg.OnMouseMove(0, CPoint(event.motion.x, event.motion.y));
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    theDlg.OnLButtonDown(0, CPoint(event.button.x, event.button.y));
                }
                input_push_mouse_button(event.button.button, true);
                break;
                
            case SDL_MOUSEBUTTONUP:
                input_push_mouse_button(event.button.button, false);
                break;
        }
    }
    
    // Present the backbuffer to the window if dirty
    flip(0);
    
    // Yield CPU time
    SDL_Delay(1);
    
#ifdef __EMSCRIPTEN__
    // Yield to the browser's rendering engine (requires Asyncify)
    emscripten_sleep(1);
#endif
    
    return TRUE;
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    // Run MFC OnInitDialog equivalent
    theDlg.OnInitDialog();
    
    // Load asset images
    loadimages();
    
    // Assign entry point pointer and start main game loop
    bg = begingame;
    bg();
    
    // Cleanup subsystems
    closegraph();
    closesound();
    dsclose();
    
    return 0;
}
