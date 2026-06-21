#include "dshow.h"
#include <SDL_mixer.h>
#include <string>

static Mix_Music* g_music = nullptr;
static std::string g_music_file = "";
static bool g_music_loop = false;

bool dsinit(HWND mwnd) {
    (void)mwnd;
    // SDL_mixer is opened in initsound
    return true;
}

void dsclose() {
    if (g_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(g_music);
        g_music = nullptr;
    }
}

bool dsplay(const char *fname, bool loop) {
    if (!fname) return false;
    
    if (g_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(g_music);
        g_music = nullptr;
    }
    
    g_music = Mix_LoadMUS(normalize_path(fname).c_str());
    if (!g_music) {
        // Fallback or debug print
        printf("Failed to load music: %s, error: %s\n", fname, Mix_GetError());
        return false;
    }
    
    g_music_file = fname;
    g_music_loop = loop;
    
    if (Mix_PlayMusic(g_music, loop ? -1 : 1) < 0) {
        printf("Failed to play music: %s\n", Mix_GetError());
        return false;
    }
    
    return true;
}

bool dsstop() {
    Mix_HaltMusic();
    return true;
}

bool dsstart() {
    if (g_music) {
        Mix_ResumeMusic();
        return true;
    }
    return false;
}

void dsevent() {
    // Stub for processing DirectShow window messages (handled internally by SDL_mixer)
}
