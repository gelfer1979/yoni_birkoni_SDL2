#include "sound.h"
#include <SDL_mixer.h>
#include <vector>
#include <cmath>

static std::vector<Mix_Chunk*> wav_buffers;
static std::vector<int> buffer_channels;
static std::vector<int> buffer_volumes;
static std::vector<int> buffer_panning;

static bool audio_initialized = false;

static inline Mix_Chunk* get_chunk(unsigned long int nbuf) {
    if (nbuf < wav_buffers.size()) return wav_buffers[nbuf];
    return nullptr;
}

bool initsound(HWND hwnd, unsigned long int daccess, unsigned long numbuf) {
    (void)hwnd; (void)daccess;
    
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        printf("Error: SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
        return false;
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error: Mix_OpenAudio failed: %s\n", Mix_GetError());
        return false;
    }
    
    // Initialize SDL_mixer formats (MP3 support)
    int flags = MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        printf("Warning: Mix_Init failed to initialize MP3 support: %s\n", Mix_GetError());
    }
    
    // Allocate buffer storage
    wav_buffers.assign(numbuf + 1, nullptr);
    buffer_channels.assign(numbuf + 1, -1);
    buffer_volumes.assign(numbuf + 1, 128); // Standard max volume
    buffer_panning.assign(numbuf + 1, 127);  // Standard center panning
    
    audio_initialized = true;
    printf("Info: Sound initialized successfully. Buffers allocated: %lu\n", numbuf);
    return true;
}

void closesound() {
    if (!audio_initialized) return;
    
    for (auto chunk : wav_buffers) {
        if (chunk) Mix_FreeChunk(chunk);
    }
    wav_buffers.clear();
    buffer_channels.clear();
    buffer_volumes.clear();
    buffer_panning.clear();
    
    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    audio_initialized = false;
}

unsigned long int loadwav(const char *mpath) {
    if (!mpath) return 0;
    std::string norm_path = normalize_path(mpath);
    Mix_Chunk* chunk = Mix_LoadWAV(norm_path.c_str());
    if (!chunk) {
        printf("Error: Failed to load sound effect: %s, normalized: %s, error: %s\n", mpath, norm_path.c_str(), Mix_GetError());
        return 0;
    }
    
    // Find free slot
    for (size_t i = 1; i < wav_buffers.size(); ++i) {
        if (!wav_buffers[i]) {
            wav_buffers[i] = chunk;
            buffer_channels[i] = -1;
            buffer_volumes[i] = 128;
            buffer_panning[i] = 127;
            printf("Info: Loaded sound effect %s to slot %zu\n", mpath, i);
            return (unsigned long int)i;
        }
    }
    
    wav_buffers.push_back(chunk);
    buffer_channels.push_back(-1);
    buffer_volumes.push_back(128);
    buffer_panning.push_back(127);
    printf("Info: Loaded sound effect %s to new slot %zu\n", mpath, wav_buffers.size() - 1);
    return (unsigned long int)(wav_buffers.size() - 1);
}

unsigned long int loadwavparth(const char *mpath, unsigned long int filesize) {
    (void)filesize;
    return loadwav(mpath);
}

unsigned long int getbufsize(unsigned long int nbuf) {
    Mix_Chunk* chunk = get_chunk(nbuf);
    if (!chunk) return 0;
    return chunk->alen;
}

bool playbuf(unsigned long int nbuf, bool mloop) {
    Mix_Chunk* chunk = get_chunk(nbuf);
    if (!chunk) {
        printf("Warning: Attempted to play uninitialized sound buffer %lu\n", nbuf);
        return false;
    }
    
    int channel = Mix_PlayChannel(-1, chunk, mloop ? -1 : 0);
    if (channel != -1) {
        buffer_channels[nbuf] = channel;
        // Apply volume
        Mix_Volume(channel, buffer_volumes[nbuf]);
        // Apply panning
        if (buffer_panning[nbuf] != 127) {
            Uint8 left = 255 - buffer_panning[nbuf];
            Uint8 right = buffer_panning[nbuf];
            Mix_SetPanning(channel, left, right);
        }
        return true;
    } else {
        printf("Warning: Mix_PlayChannel failed for buffer %lu: %s\n", nbuf, Mix_GetError());
    }
    return false;
}

unsigned long int getbufpos(unsigned long int nbuf) {
    (void)nbuf;
    return 0;
}

bool setbufpos(unsigned long int nbuf, unsigned long int npos) {
    (void)nbuf; (void)npos;
    return true;
}

bool stopbuf(unsigned long int nbuf) {
    if (nbuf >= wav_buffers.size()) return false;
    int channel = buffer_channels[nbuf];
    if (channel != -1 && Mix_Playing(channel)) {
        Mix_HaltChannel(channel);
        buffer_channels[nbuf] = -1;
        return true;
    }
    return false;
}

bool bufplaying(unsigned long int nbuf) {
    if (nbuf >= wav_buffers.size()) return false;
    int channel = buffer_channels[nbuf];
    if (channel != -1) {
        if (Mix_Playing(channel)) return true;
        buffer_channels[nbuf] = -1; // Done playing
    }
    return false;
}

long int getbufvol(unsigned long int nbuf) {
    if (nbuf >= wav_buffers.size()) return -10000;
    int vol = buffer_volumes[nbuf];
    if (vol <= 0) return -10000;
    double db = 20.0 * std::log10((double)vol / 128.0);
    return (long int)(db * 100.0);
}

bool setbufvol(unsigned long int nbuf, long int yy) {
    if (nbuf >= wav_buffers.size()) return false;
    
    int vol = 128;
    if (yy < 0) {
        double db = yy / 100.0;
        vol = (int)(128.0 * std::pow(10.0, db / 20.0));
    }
    if (vol < 0) vol = 0;
    if (vol > 128) vol = 128;
    
    buffer_volumes[nbuf] = vol;
    int channel = buffer_channels[nbuf];
    if (channel != -1 && Mix_Playing(channel)) {
        Mix_Volume(channel, vol);
    }
    return true;
}

unsigned long int getbuffreq(unsigned long int nbuf) {
    (void)nbuf;
    return 44100;
}

bool setbuffreq(unsigned long int nbuf, unsigned long int yy) {
    (void)nbuf; (void)yy;
    return true;
}

long int getbufpan(unsigned long int nbuf) {
    if (nbuf >= wav_buffers.size()) return 0;
    int pan = buffer_panning[nbuf];
    return (long int)((pan - 127) * 10000 / 127);
}

bool setbufpan(unsigned long int nbuf, long int yy) {
    if (nbuf >= wav_buffers.size()) return false;
    
    int pan = 127 + (int)(yy * 127 / 10000);
    if (pan < 0) pan = 0;
    if (pan > 255) pan = 255;
    
    buffer_panning[nbuf] = pan;
    int channel = buffer_channels[nbuf];
    if (channel != -1 && Mix_Playing(channel)) {
        Uint8 left = 255 - pan;
        Uint8 right = pan;
        Mix_SetPanning(channel, left, right);
    }
    return true;
}

void pdestroybuf() {
    for (int i = (int)wav_buffers.size() - 1; i >= 1; --i) {
        if (wav_buffers[i]) {
            Mix_FreeChunk(wav_buffers[i]);
            wav_buffers[i] = nullptr;
            buffer_channels[i] = -1;
            break;
        }
    }
}
