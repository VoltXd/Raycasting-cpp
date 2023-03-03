#pragma once

#include <vector>

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
    void calculateRaysDistance_fishEyeAndRayDistributionCorrected(Player &player, MapManager &mapManager, unsigned int fov = 90);
    void calculateRaysDistance_OMP(Player &player, MapManager &mapManager, unsigned int fov = 90);
    void SDL_renderRaycast2DMap(SDL_Renderer *renderer, MapManager &mapManager, Player &player, const unsigned int screenWidth, const unsigned int screenHeigth);
    void SDL_renderRaycast2DMiniMap(SDL_Renderer *renderer, MapManager &mapManager, Player &player, const unsigned int screenWidth, const unsigned int screenHeigth, const unsigned int scaleFactor);
    void SDL_renderRaycast(SDL_Renderer *renderer, const double currentVelocity, const double time, const unsigned int screenWidth, const unsigned int screenHeigth);
    void SDL_renderRaycastBackground(SDL_Renderer *renderer, const double currentVelocity, const double time, const unsigned int screenWidth, const unsigned int screenHeigth);
    

    private:
    std::vector<SDL_Color> m_texture;
    unsigned int m_numberOfRays;
    double *m_raysDistance;
    double *m_raysX;
    double *m_raysY;
    unsigned char *m_raysColorR;
    unsigned char *m_raysColorG;
    unsigned char *m_raysColorB;
    double *m_raysLightFactor;
    double *m_raysAngle;
    double *m_wallHeight;
    int *m_raysTextureXIndex;
    double *m_raysTextureYStep;
    bool *m_raysIsTextured;
    double m_movingOffset;

    const unsigned char TEXTURE_SIZE = 32;
    const double MOVING_OFFSET_MAGNITUDE = 3000;

    enum class WallSide
    {
        north,
        south,
        west,
        east
    };
};