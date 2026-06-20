#include "GRAPH.H"
#include <cmath>
extern bool playnow;
#include <vector>
#include <string>
#include <SDL_ttf.h>

// Global SDL graphics context
static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static SDL_Surface* g_screen = nullptr;
static SDL_Surface* g_active_surface = nullptr;
static bool g_screen_dirty = false;
static bool g_page1_dirty = false;

// Drawing state
static int g_cx = 0, g_cy = 0; // Current cursor position
static int g_stroke_color = 0xFFFFFF;
static int g_fill_color = 0x000000;
static int g_bk_color = 0x000000;
static int g_line_width = 1;

// Offscreen surface storage
static std::vector<SDL_Surface*> offb;

// Backup bitmaps storage
static std::vector<SDL_Surface*> bb;
static std::vector<int> bb1;
static int nbm = 0;

// Font rendering state
static TTF_Font* g_font = nullptr;
static int g_font_height = 16;
static std::string g_font_name = "Arial";

// Helper to resolve surface pointers (0 represents backbuffer/screen)
static inline SDL_Surface* get_surface(int index) {
    if (index == 0) return g_screen;
    if (index > 0 && index < (int)offb.size()) return offb[index];
    return nullptr;
}

static inline void mark_dirty(SDL_Surface* surface) {
    if (surface == g_screen) {
        g_screen_dirty = true;
    } else if (offb.size() > 1 && surface == offb[1]) {
        g_page1_dirty = true;
    }
}

// Helper to draw single pixel directly to surface pixels
static inline void putpixel_impl(SDL_Surface* surface, int x, int y, Uint32 pixel) {
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) return;
    Uint32* const target_pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * sizeof(Uint32));
    *target_pixel = pixel;
    mark_dirty(surface);
}

// Helper to read single pixel from surface pixels
static inline Uint32 getpixel_impl(SDL_Surface* surface, int x, int y) {
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) return 0;
    Uint32* const target_pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * sizeof(Uint32));
    return *target_pixel;
}

bool _fastcall initgraph(HWND mwin, int sizex, int sizey, int bpp, int numbbuf, int numimages, int numbitmap) {
    (void)mwin; (void)bpp; (void)numbbuf;
    
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() < 0) return false;
    
    Uint32 flags = SDL_WINDOW_SHOWN;
#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
    
    g_window = SDL_CreateWindow("Yoni Birkoni", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sizex, sizey, flags);
    if (!g_window) return false;
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer) return false;
    
    SDL_RenderSetLogicalSize(g_renderer, sizex, sizey);
    
    g_screen = SDL_CreateRGBSurfaceWithFormat(0, sizex, sizey, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!g_screen) return false;
    
    g_active_surface = g_screen;
    
    // Allocate offscreen surfaces list (1-based index support)
    offb.assign(numimages + 1, nullptr);
    
    // Create dedicated backbuffer for page 1
    offb[1] = SDL_CreateRGBSurfaceWithFormat(0, sizex, sizey, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!offb[1]) return false;
    
    // Allocate bitmap stack list
    bb.assign(numbitmap + 1, nullptr);
    bb1.assign(numbitmap + 1, -1);
    nbm = 0;
    
    return true;
}

HDC valloc(int vnum) {
    g_active_surface = get_surface(vnum);
    return (HDC)g_active_surface;
}

void vfree() {
    g_active_surface = g_screen;
}

extern bool g_joystick_active;
extern float g_joystick_center_x, g_joystick_center_y;
extern float g_joystick_curr_x, g_joystick_curr_y;

static void draw_circle_renderer(SDL_Renderer* renderer, int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            int dist_sq = dx * dx + dy * dy;
            if (dist_sq >= (radius - 2) * (radius - 2) && dist_sq <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

static void draw_filled_circle_renderer(SDL_Renderer* renderer, int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

bool flip(int numpage) {
    (void)numpage;
    if (!g_renderer || !g_screen) return false;
    
    if (playnow && g_page1_dirty && offb.size() > 1 && offb[1]) {
        SDL_BlitSurface(offb[1], nullptr, g_screen, nullptr);
        g_screen_dirty = true;
        g_page1_dirty = false;
    }
    
    if (!g_screen_dirty && !g_joystick_active) return true;
    g_screen_dirty = false;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, g_screen);
    if (!texture) return false;
    
    SDL_RenderClear(g_renderer);
    SDL_RenderCopy(g_renderer, texture, nullptr, nullptr);
    
    if (g_joystick_active) {
        // Draw outer ring (cyan, slightly transparent)
        draw_circle_renderer(g_renderer, (int)g_joystick_center_x, (int)g_joystick_center_y, 50, 0, 242, 254, 80);
        // Draw inner filled knob (cyan, more opaque)
        draw_filled_circle_renderer(g_renderer, (int)g_joystick_curr_x, (int)g_joystick_curr_y, 20, 0, 242, 254, 130);
    }
    
    SDL_RenderPresent(g_renderer);
    
    SDL_DestroyTexture(texture);
    return true;
}

void setbkcolor(int mcolor) {
    g_bk_color = mcolor;
}

void cleardevice() {
    if (!g_active_surface) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_bk_color & 0xFF, (g_bk_color >> 8) & 0xFF, (g_bk_color >> 16) & 0xFF);
    SDL_FillRect(g_active_surface, nullptr, color);
    mark_dirty(g_active_surface);
}

void setcolor(int mcolor) {
    g_stroke_color = mcolor;
}

void setlinewidth(int width) {
    g_line_width = width > 0 ? width : 1;
}

bool setfontindirect(LOGFONT *lf) {
    if (!lf) return false;
#if defined(_WIN32) && defined(UNICODE)
    char faceName[32] = "";
    WideCharToMultiByte(CP_ACP, 0, lf->lfFaceName, -1, faceName, sizeof(faceName), NULL, NULL);
    return setfont(faceName, lf->lfCharSet, lf->lfHeight, lf->lfWidth, lf->lfEscapement, lf->lfWeight, lf->lfItalic, lf->lfUnderline, lf->lfStrikeOut);
#else
    return setfont(lf->lfFaceName, lf->lfCharSet, lf->lfHeight, lf->lfWidth, lf->lfEscapement, lf->lfWeight, lf->lfItalic, lf->lfUnderline, lf->lfStrikeOut);
#endif
}

bool setfont(const char *fontname, DWORD charset, int height, int width, int orientation, int weight, bool italic, bool underline, bool strikeout) {
    (void)charset; (void)width; (void)orientation; (void)weight; (void)italic; (void)underline; (void)strikeout;
    g_font_name = fontname ? fontname : "Arial";
    g_font_height = height > 0 ? height : 16;
    
    if (g_font) {
        TTF_CloseFont(g_font);
        g_font = nullptr;
    }
    
    std::string name_str = fontname ? fontname : "Arial";
    std::vector<std::string> paths = {
        name_str + ".ttf",
        "arial.ttf",
#ifdef _WIN32
        "C:\\Windows\\Fonts\\" + name_str + ".ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
#elif defined(__APPLE__)
        "/Library/Fonts/" + name_str + ".ttf",
        "/Library/Fonts/Arial.ttf"
#else
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
#endif
    };
    
    for (const auto& path : paths) {
        g_font = TTF_OpenFont(path.c_str(), g_font_height);
        if (g_font) break;
    }
    
    return g_font != nullptr;
}

void textout(int mx, int my, const char *mstr) {
    if (!g_active_surface || !mstr || mstr[0] == '\0') return;
    
    if (!g_font) {
        setfont("Arial", 0, 16, 0, 0, 0, false, false, false);
    }
    if (!g_font) return;
    
    SDL_Color color = { (Uint8)(g_stroke_color & 0xFF), (Uint8)((g_stroke_color >> 8) & 0xFF), (Uint8)((g_stroke_color >> 16) & 0xFF), 255 };
    SDL_Surface* text_surf = TTF_RenderText_Blended(g_font, mstr, color);
    if (!text_surf) return;
    
    SDL_Rect dst = { mx, my, text_surf->w, text_surf->h };
    SDL_BlitSurface(text_surf, nullptr, g_active_surface, &dst);
    mark_dirty(g_active_surface);
    SDL_FreeSurface(text_surf);
}

void moveto(int mx, int my) {
    g_cx = mx;
    g_cy = my;
}

void lineto(int mx, int my) {
    line(g_cx, g_cy, mx, my);
    g_cx = mx;
    g_cy = my;
}

void line(int mx, int my, int mx1, int my1) {
    if (!g_active_surface) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_stroke_color & 0xFF, (g_stroke_color >> 8) & 0xFF, (g_stroke_color >> 16) & 0xFF);
    
    int dx = std::abs(mx1 - mx), sx = mx < mx1 ? 1 : -1;
    int dy = -std::abs(my1 - my), sy = my < my1 ? 1 : -1;
    int err = dx + dy, e2;
    
    while (true) {
        putpixel_impl(g_active_surface, mx, my, color);
        if (mx == mx1 && my == my1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; mx += sx; }
        if (e2 <= dx) { err += dx; my += sy; }
    }
}

void rectangle(int mx, int my, int mx1, int my1) {
    line(mx, my, mx1, my);
    line(mx1, my, mx1, my1);
    line(mx1, my1, mx, my1);
    line(mx, my1, mx, my);
}

void setfillcolor(int mcol) {
    g_fill_color = mcol;
}

void bar(int mx, int my, int mx1, int my1) {
    if (!g_active_surface) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_fill_color & 0xFF, (g_fill_color >> 8) & 0xFF, (g_fill_color >> 16) & 0xFF);
    SDL_Rect r = { mx, my, mx1 - mx + 1, my1 - my + 1 };
    SDL_FillRect(g_active_surface, &r, color);
    mark_dirty(g_active_surface);
}

void fillcircle(int sx, int sy, int rad) {
    if (!g_active_surface || rad < 0) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_fill_color & 0xFF, (g_fill_color >> 8) & 0xFF, (g_fill_color >> 16) & 0xFF);
    
    for (int dy = -rad; dy <= rad; dy++) {
        int dx = (int)std::sqrt(rad * rad - dy * dy);
        for (int x = -dx; x <= dx; x++) {
            putpixel_impl(g_active_surface, sx + x, sy + dy, color);
        }
    }
}

void fillellipse(int sx, int sy, int rad, int rae) {
    if (!g_active_surface || rad < 0 || rae < 0) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_fill_color & 0xFF, (g_fill_color >> 8) & 0xFF, (g_fill_color >> 16) & 0xFF);
    
    for (int dy = -rae; dy <= rae; dy++) {
        int dx = (int)(rad * std::sqrt(1.0f - (float)(dy * dy) / (float)(rae * rae)));
        for (int x = -dx; x <= dx; x++) {
            putpixel_impl(g_active_surface, sx + x, sy + dy, color);
        }
    }
}

void circle(int sx, int sy, int rae) {
    if (!g_active_surface || rae < 0) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_stroke_color & 0xFF, (g_stroke_color >> 8) & 0xFF, (g_stroke_color >> 16) & 0xFF);
    
    int x = rae;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        putpixel_impl(g_active_surface, sx + x, sy + y, color);
        putpixel_impl(g_active_surface, sx + y, sy + x, color);
        putpixel_impl(g_active_surface, sx - y, sy + x, color);
        putpixel_impl(g_active_surface, sx - x, sy + y, color);
        putpixel_impl(g_active_surface, sx - x, sy - y, color);
        putpixel_impl(g_active_surface, sx - y, sy - x, color);
        putpixel_impl(g_active_surface, sx + y, sy - x, color);
        putpixel_impl(g_active_surface, sx + x, sy - y, color);
        
        y += 1;
        if (err <= 0) {
            err += 2 * y + 1;
        } else {
            x -= 1;
            err += 2 * (y - x) + 1;
        }
    }
}

void ellipse(int sx, int sy, int rae, int ral) {
    if (!g_active_surface || rae < 0 || ral < 0) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, g_stroke_color & 0xFF, (g_stroke_color >> 8) & 0xFF, (g_stroke_color >> 16) & 0xFF);
    
    float dx, dy, d1, d2;
    float x = 0;
    float y = ral;
    
    d1 = (ral * ral) - (rae * rae * ral) + (0.25f * rae * rae);
    dx = 2 * ral * ral * x;
    dy = 2 * rae * rae * y;
    
    while (dx < dy) {
        putpixel_impl(g_active_surface, sx + x, sy + y, color);
        putpixel_impl(g_active_surface, sx - x, sy + y, color);
        putpixel_impl(g_active_surface, sx + x, sy - y, color);
        putpixel_impl(g_active_surface, sx - x, sy - y, color);
        
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ral * ral);
            d1 = d1 + dx + (ral * ral);
        } else {
            x++;
            y--;
            dx = dx + (2 * ral * ral);
            dy = dy - (2 * rae * rae);
            d1 = d1 + dx - dy + (ral * ral);
        }
    }
    
    d2 = ((ral * ral) * ((x + 0.5f) * (x + 0.5f))) + ((rae * rae) * ((y - 1) * (y - 1))) - (rae * rae * ral * ral);
    
    while (y >= 0) {
        putpixel_impl(g_active_surface, sx + x, sy + y, color);
        putpixel_impl(g_active_surface, sx - x, sy + y, color);
        putpixel_impl(g_active_surface, sx + x, sy - y, color);
        putpixel_impl(g_active_surface, sx - x, sy - y, color);
        
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rae * rae);
            d2 = d2 + (rae * rae) - dy;
        } else {
            y--;
            x++;
            dx = dx + (2 * ral * ral);
            dy = dy - (2 * rae * rae);
            d2 = d2 + dx - dy + (rae * rae);
        }
    }
}

void putpixel(int sx, int sy, int colr) {
    if (!g_active_surface) return;
    Uint32 color = SDL_MapRGB(g_active_surface->format, colr & 0xFF, (colr >> 8) & 0xFF, (colr >> 16) & 0xFF);
    putpixel_impl(g_active_surface, sx, sy, color);
}

int getpixel(int sx, int sy) {
    if (!g_active_surface) return 0;
    Uint32 pixel = getpixel_impl(g_active_surface, sx, sy);
    Uint8 r, g, b;
    SDL_GetRGB(pixel, g_active_surface->format, &r, &g, &b);
    return r | (g << 8) | (b << 16);
}

bool loadbmp(int mx, int my, const char *mpath) {
    SDL_Surface* target = g_active_surface ? g_active_surface : g_screen;
    if (!target || !mpath) return false;
    SDL_Surface* bmp = SDL_LoadBMP(normalize_path(mpath).c_str());
    if (!bmp) return false;
    
    SDL_Surface* converted = SDL_ConvertSurface(bmp, target->format, 0);
    SDL_FreeSurface(bmp);
    if (!converted) return false;
    
    SDL_Rect dst = { mx, my, converted->w, converted->h };
    SDL_BlitSurface(converted, nullptr, target, &dst);
    mark_dirty(target);
    SDL_FreeSurface(converted);
    return true;
}

int loadbmptoptr(const char *mpath) {
    if (!mpath) return -1;
    SDL_Surface* bmp = SDL_LoadBMP(normalize_path(mpath).c_str());
    if (!bmp) return -1;
    
    SDL_Surface* converted = SDL_ConvertSurface(bmp, g_screen->format, 0);
    SDL_FreeSurface(bmp);
    if (!converted) return -1;
    
    // Find free slot (skipping slot 1, which is the dedicated backbuffer)
    for (size_t i = 2; i < offb.size(); ++i) {
        if (!offb[i]) {
            offb[i] = converted;
            return (int)i;
        }
    }
    offb.push_back(converted);
    return (int)(offb.size() - 1);
}

bool putsprite(int mx, int my, int bnum, int mpage) {
    SDL_Surface* src = get_surface(bnum);
    SDL_Surface* dst = get_surface(mpage);
    if (!src || !dst) return false;
    
    SDL_SetColorKey(src, SDL_TRUE, 0); // Black (0) is transparent
    SDL_Rect dstrect = { mx, my, src->w, src->h };
    SDL_BlitSurface(src, nullptr, dst, &dstrect);
    mark_dirty(dst);
    return true;
}

bool putimage(int mx, int my, int bnum, int mpage) {
    SDL_Surface* src = get_surface(bnum);
    SDL_Surface* dst = get_surface(mpage);
    if (!src || !dst) return false;
    
    SDL_SetColorKey(src, SDL_FALSE, 0); // Opaque blit
    SDL_Rect dstrect = { mx, my, src->w, src->h };
    SDL_BlitSurface(src, nullptr, dst, &dstrect);
    mark_dirty(dst);
    return true;
}

int getimage(int mx, int my, int mx1, int my1, int mpage) {
    SDL_Surface* src = get_surface(mpage);
    if (!src) return -1;
    
    int w = mx1 - mx + 1;
    int h = my1 - my + 1;
    if (w <= 0 || h <= 0) return -1;
    
    SDL_Surface* dst = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, src->format->format);
    if (!dst) return -1;
    
    SDL_Rect srcrect = { mx, my, w, h };
    SDL_BlitSurface(src, &srcrect, dst, nullptr);
    
    // Find free slot (skipping slot 1, which is the dedicated backbuffer)
    for (size_t i = 2; i < offb.size(); ++i) {
        if (!offb[i]) {
            offb[i] = dst;
            return (int)i;
        }
    }
    offb.push_back(dst);
    return (int)(offb.size() - 1);
}

bool virtputimage(int mx, int my, int bnum, int bnum1) {
    return putimage(mx, my, bnum, bnum1);
}

bool virtputsprite(int mx, int my, int bnum, int bnum1) {
    return putsprite(mx, my, bnum, bnum1);
}

int virtgetimage(int mx, int my, int mx1, int my1, int mpage) {
    return getimage(mx, my, mx1, my1, mpage);
}

bool putimagetoscreen(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mpage) {
    SDL_Surface* src = get_surface(mnum);
    SDL_Surface* dst = get_surface(mpage);
    if (!src || !dst) return false;
    
    SDL_SetColorKey(src, SDL_FALSE, 0);
    SDL_Rect srcrect = { mx, my, mx1 - mx + 1, my1 - my + 1 };
    SDL_Rect dstrect = { mx3, my3, srcrect.w, srcrect.h };
    SDL_BlitSurface(src, &srcrect, dst, &dstrect);
    mark_dirty(dst);
    return true;
}

bool putspritetoscreen(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mpage) {
    SDL_Surface* src = get_surface(mnum);
    SDL_Surface* dst = get_surface(mpage);
    if (!src || !dst) return false;
    
    SDL_SetColorKey(src, SDL_TRUE, 0);
    SDL_Rect srcrect = { mx, my, mx1 - mx + 1, my1 - my + 1 };
    SDL_Rect dstrect = { mx3, my3, srcrect.w, srcrect.h };
    SDL_BlitSurface(src, &srcrect, dst, &dstrect);
    mark_dirty(dst);
    return true;
}

bool putimagetoptr(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mnum1) {
    return putimagetoscreen(mx, my, mx1, my1, mx3, my3, mnum, mnum1);
}

bool putspritetoptr(int mx, int my, int mx1, int my1, int mx3, int my3, int mnum, int mnum1) {
    return putspritetoscreen(mx, my, mx1, my1, mx3, my3, mnum, mnum1);
}

void prelease() {
    // Delete last offscreen surface (skipping slot 1, which is the dedicated backbuffer)
    for (int i = (int)offb.size() - 1; i >= 2; --i) {
        if (offb[i]) {
            SDL_FreeSurface(offb[i]);
            offb[i] = nullptr;
            break;
        }
    }
}
 
void release(int n) {
    if (n > 1 && n < (int)offb.size() && offb[n]) {
        SDL_FreeSurface(offb[n]);
        offb[n] = nullptr;
    }
}

void closegraph() {
    if (g_font) {
        TTF_CloseFont(g_font);
        g_font = nullptr;
    }
    TTF_Quit();
    
    for (auto surf : offb) {
        if (surf) SDL_FreeSurface(surf);
    }
    offb.clear();
    
    for (auto surf : bb) {
        if (surf) SDL_FreeSurface(surf);
    }
    bb.clear();
    nbm = 0;
    
    if (g_screen) {
        SDL_FreeSurface(g_screen);
        g_screen = nullptr;
    }
    
    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }
    
    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }
    
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void restoreimage(int num) {
    (void)num;
}

void restorescreen() {
}

bool reloadbmptoptr(int bnum, const char *mpath) {
    if (bnum <= 1 || bnum >= (int)offb.size() || !mpath) return false;
    if (offb[bnum]) {
        SDL_FreeSurface(offb[bnum]);
        offb[bnum] = nullptr;
    }
    SDL_Surface* bmp = SDL_LoadBMP(normalize_path(mpath).c_str());
    if (!bmp) return false;
    
    offb[bnum] = SDL_ConvertSurface(bmp, g_screen->format, 0);
    SDL_FreeSurface(bmp);
    return offb[bnum] != nullptr;
}

int fillbitmap(int numi) {
    if (nbm >= (int)bb.size()) return -1;
    SDL_Surface* src = get_surface(numi);
    if (!src) return -1;
    
    SDL_Surface* copy = SDL_ConvertSurface(src, src->format, 0);
    if (!copy) return -1;
    
    bb[nbm] = copy;
    bb1[nbm] = numi;
    nbm++;
    return nbm - 1;
}

bool freebitmap() {
    if (nbm > 0) nbm--; else return false;
    if (!bb[nbm]) return false;
    
    SDL_Surface* dst = get_surface(bb1[nbm]);
    if (dst) {
        SDL_BlitSurface(bb[nbm], nullptr, dst, nullptr);
        mark_dirty(dst);
    }
    SDL_FreeSurface(bb[nbm]);
    bb[nbm] = nullptr;
    return true;
}

bool updatebitmap(int mx, int my, int width1, int height1, int bnum, int bmnum) {
    if (bmnum < 0 || bmnum >= (int)bb.size() || !bb[bmnum]) return false;
    SDL_Surface* src = get_surface(bnum);
    if (!src) return false;
    
    int w = (width1 == 0) ? src->w : width1;
    int h = (height1 == 0) ? src->h : height1;
    
    SDL_Rect r = { mx, my, w, h };
    SDL_BlitSurface(src, &r, bb[bmnum], &r);
    return true;
}

bool removebitmap() {
    if (nbm > 0) nbm--; else return false;
    if (bb[nbm]) {
        SDL_FreeSurface(bb[nbm]);
        bb[nbm] = nullptr;
    }
    return true;
}

bool putbitmap(int nbm_idx) {
    if (nbm_idx < 0 || nbm_idx >= (int)bb.size() || !bb[nbm_idx]) return false;
    SDL_Surface* dst = get_surface(bb1[nbm_idx]);
    if (!dst) return false;
    SDL_BlitSurface(bb[nbm_idx], nullptr, dst, nullptr);
    mark_dirty(dst);
    return true;
}

void clear_page1_dirty() {
    g_page1_dirty = false;
}
