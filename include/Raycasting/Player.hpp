#pragma once

#include "SDL.h"
#include "MapManager.hpp"

class Player
{
    public:
    Player() = default;

    void initialisePlayer(MapManager &mapManager);
    void movePlayer(MapManager &mapManager, double vForward, double vSide, double dt);
    inline void rotatePlayer(double angularSpeed, double dt) { m_angle += m_rotationSpeed * angularSpeed * dt; };
    inline double getX() { return m_xPosition; }
    inline double getY() { return m_yPosition; }
    inline double getAngle() { return m_angle; }
    int SDL_renderPlayer(SDL_Renderer *renderer, MapManager &mapManager, const unsigned int screenWidth, const unsigned int screenHeight);
    int SDL_renderPlayerMiniMap(SDL_Renderer *renderer, MapManager &mapManager, const unsigned int screenWidth, const unsigned int screenHeight, unsigned char scaleFactor);

    private:
    double m_moveSpeed;
    double m_rotationSpeed;
    double m_xPosition;
    double m_yPosition;
    double m_angle;
    const double PLAYER_SIZE = 1;
};