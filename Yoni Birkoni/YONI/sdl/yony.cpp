#include "compat.h"
#include "GRAPH.H"
#include "INPUT.H"
#include "sound.h"
#include "dshow.h"
#define protected public
#include "yonyDlg.h"
#undef protected

#include <cmath>
#ifndef _WIN32
#include <unistd.h>
#endif

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
static bool g_touch_is_esc = false;

// Convert normalised finger coords (0..1) to logical 800x600
// SDL_FINGERDOWN/MOTION/UP x/y are already normalised 0..1 relative to window size,
// so multiplying by the logical resolution gives correct logical pixel coords.
static void finger_to_logical(float fx, float fy, float &log_x, float &log_y) {
    log_x = fx * 800.0f;
    log_y = fy * 600.0f;
}

static void update_joystick_drag(float curr_x, float curr_y) {
    if (g_joystick_curr_x == curr_x && g_joystick_curr_y == curr_y) return;
    g_joystick_curr_x = curr_x;
    g_joystick_curr_y = curr_y;
    g_joystick_dirty = true;
    
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
    static Uint32 startup_ticks = 0;
    if (startup_ticks == 0) {
        startup_ticks = SDL_GetTicks();
    }
    bool is_startup = (SDL_GetTicks() - startup_ticks < 500);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                closegraph();
                dsclose();
                closesound();
                std::exit(0);
                break;
                
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_EXPOSED ||
                    event.window.event == SDL_WINDOWEVENT_SHOWN ||
                    event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                    event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    mark_screen_dirty();
                }
                break;
            case SDL_RENDER_TARGETS_RESET:
            case SDL_RENDER_DEVICE_RESET:
                mark_screen_dirty();
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (!is_startup) {
                    input_push_key_event(event.key.keysym.scancode, event.type == SDL_KEYDOWN);
                }
                break;
                
            case SDL_FINGERDOWN:
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
                if (!is_startup) {
                    float lx, ly;
                    finger_to_logical(event.tfinger.x, event.tfinger.y, lx, ly);
                    
                    if (playnow && lx >= 5.0f && lx <= 160.0f && ly >= 0.0f && ly <= 85.0f) {
                        g_joystick_active = false;
                        g_touch_is_esc = true;
                    } else {
                        g_joystick_active = true;
                        g_joystick_center_x = lx;
                        g_joystick_center_y = ly;
                        g_joystick_curr_x = lx;
                        g_joystick_curr_y = ly;
                        g_joystick_dir = 0;
                        g_joystick_has_moved = false;
                        g_touch_is_esc = false;
                    }
                    g_joystick_dirty = true;
                }
#endif
                break;
                
            case SDL_FINGERMOTION:
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
                if (!is_startup && g_joystick_active && !g_touch_is_esc) {
                    float lx, ly;
                    finger_to_logical(event.tfinger.x, event.tfinger.y, lx, ly);
                    update_joystick_drag(lx, ly);
                }
#endif
                break;
                
            case SDL_FINGERUP:
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
                if (!is_startup) {
                    if (g_touch_is_esc) {
                        float tx, ty;
                        finger_to_logical(event.tfinger.x, event.tfinger.y, tx, ty);
                        if (tx >= 5.0f && tx <= 160.0f && ty >= 0.0f && ty <= 85.0f) {
                            input_push_simulated_key(0x01, true); // ESC
                        }
                        g_touch_is_esc = false;
                        g_joystick_dirty = true;
                    } else if (g_joystick_active) {
                        float tx, ty;
                        finger_to_logical(event.tfinger.x, event.tfinger.y, tx, ty);
                        float dx = tx - g_joystick_center_x;
                        float dy = ty - g_joystick_center_y;
                        float dist = std::sqrt(dx * dx + dy * dy);
                        if (!g_joystick_has_moved || dist < 30.0f) {
                            if (g_select_new_game_active) {
                                if (ty >= 480.0f && ty <= 530.0f) {
                                    if (tx >= 200.0f && tx <= 380.0f) {
                                        input_push_simulated_key(0x15, true); // 'Y' (Yes)
                                    } else if (tx >= 420.0f && tx <= 600.0f) {
                                        input_push_simulated_key(0x01, true); // ESC (Cancel)
                                    } else {
                                        input_push_simulated_key(0x1C, true); // ENTER (tap elsewhere)
                                    }
                                } else {
                                    input_push_simulated_key(0x1C, true); // ENTER (tap elsewhere)
                                }
                            } else {
                                // Check if the tap is within the main menu buttons bounds
                                if (tx >= 315.0f && tx <= 530.0f) {
                                    int tapped_ns = -1;
                                    if (ty >= 270.0f && ty <= 300.0f) tapped_ns = 0;
                                    else if (ty >= 320.0f && ty <= 350.0f) tapped_ns = 1;
                                    else if (ty >= 370.0f && ty <= 400.0f) tapped_ns = 2;
                                    else if (ty >= 420.0f && ty <= 450.0f) tapped_ns = 3;
                                    else if (ty >= 470.0f && ty <= 500.0f) tapped_ns = 4;
                                    else if (ty >= 520.0f && ty <= 550.0f) tapped_ns = 5;
                                    
                                    if (tapped_ns != -1) {
                                        extern int ns;
                                        ns = tapped_ns;
                                        input_push_simulated_key(0x1C, true); // ENTER
                                    }
                                }
                            }
                        }
                        g_joystick_active = false;
                        g_joystick_dir = 0;
                        g_joystick_dirty = true;
                    }
                }
#endif
                break;
                
#if defined(__EMSCRIPTEN__)
            case SDL_MOUSEBUTTONDOWN:
                if (!is_startup) {
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
                }
                break;
                
            case SDL_MOUSEMOTION:
                if (!is_startup) {
                    if (g_joystick_active) {
                        update_joystick_drag((float)event.motion.x, (float)event.motion.y);
                    }
                    input_push_mouse_motion(event.motion.xrel, event.motion.yrel);
                    theDlg.OnMouseMove(0, CPoint(event.motion.x, event.motion.y));
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (!is_startup) {
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
                                        } else {
                                            input_push_simulated_key(0x1C, true);
                                        }
                                    } else {
                                        input_push_simulated_key(0x1C, true);
                                    }
                                } else if (playnow && tx >= 10.0f && tx <= 100.0f && ty >= 0.0f && ty <= 40.0f) {
                                    input_push_simulated_key(0x01, true); // ESC
                                } else {
                                    // Check if the tap is within the main menu buttons bounds
                                    if (tx >= 315.0f && tx <= 530.0f) {
                                        int tapped_ns = -1;
                                        if (ty >= 270.0f && ty <= 300.0f) tapped_ns = 0;
                                        else if (ty >= 320.0f && ty <= 350.0f) tapped_ns = 1;
                                        else if (ty >= 370.0f && ty <= 400.0f) tapped_ns = 2;
                                        else if (ty >= 420.0f && ty <= 450.0f) tapped_ns = 3;
                                        else if (ty >= 470.0f && ty <= 500.0f) tapped_ns = 4;
                                        else if (ty >= 520.0f && ty <= 550.0f) tapped_ns = 5;
                                        
                                        if (tapped_ns != -1) {
                                            extern int ns;
                                            ns = tapped_ns;
                                            input_push_simulated_key(0x1C, true); // ENTER
                                        }
                                    }
                                }
                            }
                            g_joystick_active = false;
                            g_joystick_dir = 0;
                        }
                    }
                    input_push_mouse_button(event.button.button, false);
                }
                break;
#else
            case SDL_MOUSEBUTTONDOWN:
                if (!is_startup) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        theDlg.OnLButtonDown(0, CPoint(event.button.x, event.button.y));
                        float tx = (float)event.button.x;
                        float ty = (float)event.button.y;

                        SDL_Log("Click logical: %.1f, %.1f  g_new_game=%d playnow=%d",
                                tx, ty, (int)g_select_new_game_active, (int)playnow);
                        if (g_select_new_game_active) {
                            if (ty >= 470.0f && ty <= 540.0f) {
                                if (tx >= 190.0f && tx <= 390.0f) {
                                    input_push_simulated_key(0x15, true); // 'Y' (Yes)
                                } else if (tx >= 410.0f && tx <= 610.0f) {
                                    input_push_simulated_key(0x01, true); // ESC (No/Cancel)
                                }
                            }
                        } else if (playnow && tx >= 5.0f && tx <= 150.0f && ty >= 0.0f && ty <= 85.0f) {
                            input_push_simulated_key(0x01, true); // ESC
                        }
                    }
                    input_push_mouse_button(event.button.button, true);
                }
                break;
                
            case SDL_MOUSEMOTION:
                if (!is_startup) {
                    input_push_mouse_motion(event.motion.xrel, event.motion.yrel);
                    theDlg.OnMouseMove(0, CPoint(event.motion.x, event.motion.y));
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (!is_startup) {
                    input_push_mouse_button(event.button.button, false);
                }
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
    
    // Automatically change working directory to where the executable is located
    char* base_path = SDL_GetBasePath();
    if (base_path) {
#ifndef _WIN32
        chdir(base_path);
#endif
        printf("[DEBUG] Changed working directory to executable path: %s\n", base_path);
        SDL_free(base_path);
    }
    
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
#endif

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
    dsclose();
    closesound();
    
    return 0;
}
