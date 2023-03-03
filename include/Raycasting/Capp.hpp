#pragma once

#include <chrono>
#include <string>

#include "SDL.h"
#include "SDL_ttf.h"
#include "MapManager.hpp"
#include "Player.hpp"
#include "Raycaster.hpp"

class Capp
{
    public:
    Capp();
    bool run();

    private:
    bool initialise();

    void input();
    void update();
    void render();

    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    TTF_Font *m_font;
    SDL_Surface *m_FPStextSurface;
    SDL_Texture *m_FPStextTexture;
    SDL_Rect m_FPStextTextureRect;
    std::string m_FPSstring;
    SDL_Color m_FPStextColor;
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
    Uint32 m_windowFlags;
    bool m_isRunning;

    std::chrono::high_resolution_clock::time_point m_previousTimePoint;
    
    MapManager m_mapManager;
    Player m_player;
    Raycaster m_raycaster;
    double m_accelForward;
    double m_accelSide;
    double m_angularSpeed;
    bool m_mouseMoved;
    bool m_isSprinting;
    unsigned int m_fov;

    //const unsigned int DELTA_TIME_MILLISECONDS = 5;
    const unsigned char MINIMAP_SCALE_FACTOR = 4;
};