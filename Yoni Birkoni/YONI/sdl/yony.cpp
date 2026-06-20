#include "compat.h"
#include "GRAPH.H"
#include "INPUT.H"
#include "sound.h"
#include "dshow.h"
#define protected public
#include "yonyDlg.h"
#undef protected

#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global function pointer and logic from YonyDlg.cpp
extern void (*bg)();
extern void begingame();
extern void loadimages();
extern void newgame();
extern bool playnow;

// Public input queues in INPUT.CPP
extern void input_push_key_event(SDL_Scancode scancode, bool is_down);
extern void input_push_mouse_motion(int xrel, int yrel);
extern void input_push_mouse_button(int button, bool is_down);

// Global CPackmanDlg instance
CPackmanDlg theDlg;

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
static bool g_joystick_has_moved = false;

static void update_joystick_drag(float curr_x, float curr_y) {
    g_joystick_curr_x = curr_x;
    g_joystick_curr_y = curr_y;
    
    float dx = g_joystick_curr_x - g_joystick_center_x;
    float dy = g_joystick_curr_y - g_joystick_center_y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // Clamp visual knob to outer radius (50)
    if (distance > 50.0f) {
        g_joystick_curr_x = g_joystick_center_x + (dx / distance) * 50.0f;
        g_joystick_curr_y = g_joystick_center_y + (dy / distance) * 50.0f;
        dx = g_joystick_curr_x - g_joystick_center_x;
        dy = g_joystick_curr_y - g_joystick_center_y;
        distance = 50.0f;
    }
    
    // Check deadzone (15)
    if (distance > 15.0f) {
        g_joystick_has_moved = true;
        int new_dir = 0;
        if (std::abs(dx) > std::abs(dy)) {
            new_dir = (dx > 0) ? 3 : 1; // 3 = RIGHT, 1 = LEFT
        } else {
            new_dir = (dy > 0) ? 4 : 2; // 4 = DOWN, 2 = UP
        }
        
        if (new_dir != g_joystick_dir) {
            // Push simulated key event for menu navigation / getkey() consumers
            if (new_dir == 1) input_push_simulated_key(0xCB, true); // LEFT
            else if (new_dir == 2) input_push_simulated_key(0xC8, true); // UP
            else if (new_dir == 3) input_push_simulated_key(0xCD, true); // RIGHT
            else if (new_dir == 4) input_push_simulated_key(0xD0, true); // DOWN
            
            g_joystick_dir = new_dir;
        }
    } else {
        g_joystick_dir = 0;
    }
}
#endif

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
                
            case SDL_FINGERDOWN:
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
                g_joystick_active = true;
                g_joystick_center_x = event.tfinger.x * 800.0f;
                g_joystick_center_y = event.tfinger.y * 600.0f;
                g_joystick_curr_x = g_joystick_center_x;
                g_joystick_curr_y = g_joystick_center_y;
                g_joystick_dir = 0;
                g_joystick_has_moved = false;
#endif
                break;
                
            case SDL_FINGERMOTION:
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
                if (g_joystick_active) {
                    update_joystick_drag(event.tfinger.x * 800.0f, event.tfinger.y * 600.0f);
                }
#endif
                break;
                
            case SDL_FINGERUP:
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
                if (g_joystick_active) {
                    if (!g_joystick_has_moved) {
                        float tx = event.tfinger.x * 800.0f;
                        float ty = event.tfinger.y * 600.0f;
                        if (g_select_new_game_active) {
                            if (ty >= 480.0f && ty <= 530.0f) {
                                if (tx >= 200.0f && tx <= 380.0f) {
                                    input_push_simulated_key(0x15, true); // 'Y' (Yes)
                                } else if (tx >= 420.0f && tx <= 600.0f) {
                                    input_push_simulated_key(0x01, true); // ESC (Cancel)
                                }
                            }
                        } else if (playnow && tx >= 10.0f && tx <= 100.0f && ty >= 0.0f && ty <= 40.0f) {
                            input_push_simulated_key(0x01, true); // ESC
                        } else {
                            // Tap! Simulate ENTER key
                            input_push_simulated_key(0x1C, true);
                        }
                    }
                    g_joystick_active = false;
                    g_joystick_dir = 0;
                }
#endif
                break;
                
#if defined(__EMSCRIPTEN__)
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    g_joystick_active = true;
                    g_joystick_center_x = (float)event.button.x;
                    g_joystick_center_y = (float)event.button.y;
                    g_joystick_curr_x = g_joystick_center_x;
                    g_joystick_curr_y = g_joystick_center_y;
                    g_joystick_dir = 0;
                    g_joystick_has_moved = false;
                    theDlg.OnLButtonDown(0, CPoint(event.button.x, event.button.y));
                }
                input_push_mouse_button(event.button.button, true);
                break;
                
            case SDL_MOUSEMOTION:
                if (g_joystick_active) {
                    update_joystick_drag((float)event.motion.x, (float)event.motion.y);
                }
                input_push_mouse_motion(event.motion.xrel, event.motion.yrel);
                theDlg.OnMouseMove(0, CPoint(event.motion.x, event.motion.y));
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (g_joystick_active) {
                        if (!g_joystick_has_moved) {
                            float tx = (float)event.button.x;
                            float ty = (float)event.button.y;
                            if (g_select_new_game_active) {
                                if (ty >= 480.0f && ty <= 530.0f) {
                                    if (tx >= 200.0f && tx <= 380.0f) {
                                        input_push_simulated_key(0x15, true); // 'Y' (Yes)
                                    } else if (tx >= 420.0f && tx <= 600.0f) {
                                        input_push_simulated_key(0x01, true); // ESC (Cancel)
                                    }
                                }
                            } else if (playnow && tx >= 10.0f && tx <= 100.0f && ty >= 0.0f && ty <= 40.0f) {
                                input_push_simulated_key(0x01, true); // ESC
                            } else {
                                // Tap! Simulate ENTER key
                                input_push_simulated_key(0x1C, true);
                            }
                        }
                        g_joystick_active = false;
                        g_joystick_dir = 0;
                    }
                }
                input_push_mouse_button(event.button.button, false);
                break;
#else
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    theDlg.OnLButtonDown(0, CPoint(event.button.x, event.button.y));
                }
                input_push_mouse_button(event.button.button, true);
                break;
                
            case SDL_MOUSEMOTION:
                input_push_mouse_motion(event.motion.xrel, event.motion.yrel);
                theDlg.OnMouseMove(0, CPoint(event.motion.x, event.motion.y));
                break;
                
            case SDL_MOUSEBUTTONUP:
                input_push_mouse_button(event.button.button, false);
                break;
#endif
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
    while (true) {
        bg();
        newgame();
    }
    
    // Cleanup subsystems
    closegraph();
    closesound();
    dsclose();
    
    return 0;
}
