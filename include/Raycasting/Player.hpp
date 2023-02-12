#pragma once

#include "SDL.h"
#include "MapManager.hpp"

class Player
{
    public:
    Player() = default;

    void initialisePlayer(MapManager &mapManager);
    void movePlayer(MapManager &mapManager, double accelForward, double accelSide, double dt);
    inline void rotatePlayer(double angularSpeed, double dt) { m_angle += m_rotationSpeed * angularSpeed * dt; };
    inline double getX() { return m_xPosition; }
    inline double getY() { return m_yPosition; }
    inline double getAngle() { return m_angle; }
    inline double getVelocity() {return m_vMagnitude; }
    int SDL_renderPlayer(SDL_Renderer *renderer, MapManager &mapManager, const unsigned int screenWidth, const unsigned int screenHeight);
    int SDL_renderPlayerMiniMap(SDL_Renderer *renderer, MapManager &mapManager, const unsigned int screenWidth, const unsigned int screenHeight, unsigned char scaleFactor);

    private:
    double m_rotationSpeed;
    double m_xPosition;
    double m_yPosition;
    double m_angle;
    double m_vMagnitude;
    double m_vx;
    double m_vy;

    const double ACCELERATION = 20;
    const double LINEAR_DRAG = 5;
    const double MOVE_SPEED = 5;
    const double SPRINT_SPEED = 10;
    const double PLAYER_SIZE = 1;
};