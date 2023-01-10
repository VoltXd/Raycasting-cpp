#pragma once

#include "MapManager.hpp"
#include "Player.hpp"
#include "SDL.h"
#include "Player.hpp"

class Raycaster
{
    public:
    Raycaster();
    ~Raycaster();

    void initialiseRaycaster(const unsigned int numberOfRays);
    void calculateRaysDistance(Player &player, MapManager &mapManager, unsigned int fov = 90);
    void SDL_renderRaycast2DMap(SDL_Renderer *renderer, MapManager &mapManager, Player &player, const unsigned int screenWidth, const unsigned int screenHeigth);
    void SDL_renderRaycast2DMiniMap(SDL_Renderer *renderer, MapManager &mapManager, Player &player, const unsigned int screenWidth, const unsigned int screenHeigth, const unsigned int scaleFactor);
    void SDL_renderRaycast(SDL_Renderer *renderer, const unsigned int screenWidth, const unsigned int screenHeigth);
    

    private:
    unsigned int m_numberOfRays;
    double *m_raysDistance;
    double *m_raysX;
    double *m_raysY;
    char *m_raysColorR;
    char *m_raysColorG;
    char *m_raysColorB;
};