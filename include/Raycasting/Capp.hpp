#pragma once

#include <chrono>

#include "SDL.h"
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
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
    bool m_isRunning;

    std::chrono::high_resolution_clock::time_point m_previousTimePoint;
    
    MapManager m_mapManager;
    Player m_player;
    Raycaster m_raycaster;
    double m_vForward;
    double m_vSide;
    double m_angularSpeed;
    bool m_mouseMoved;

    //const unsigned int DELTA_TIME_MILLISECONDS = 5;
    const unsigned char MINIMAP_SCALE_FACTOR = 4;
};