#include <cmath>
#include <random>
#include <iostream>
#include <chrono>

#include "Player.hpp"
#include "SDL.h"
#include "MapManager.hpp"
#include "Toolbox.hpp"

void Player::initialisePlayer(MapManager &mapManager)
{
    m_angle = 0;
    m_moveSpeed = 4;
    m_rotationSpeed = 180.0 * Math::DEGREE_TO_RADIAN;

    auto timePoint = std::chrono::high_resolution_clock::now();
    unsigned long long seed = timePoint.time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distributionX(0, mapManager.getWidth() - 1);
    std::uniform_int_distribution<int> distributionY(0, mapManager.getHeight() - 1);

    int xStart;
    int yStart;
    do
    {
        xStart = distributionX(generator);
        yStart = distributionY(generator);
    } while (mapManager.getMapElement(xStart, yStart) != 0);

    m_xPosition = xStart + 0.5;
    m_yPosition = yStart + 0.5;
}

void Player::movePlayer(MapManager &mapManager, double vForward, double vSide, double dt)
{
    double vInverseMagnitude = 1.0 / sqrt(vForward * vForward + vSide * vSide); 
    if (std::isinf(vInverseMagnitude))
        vInverseMagnitude = 0;
    vForward *= vInverseMagnitude;
    vSide *= vInverseMagnitude;

    double nextX = m_xPosition + m_moveSpeed * (vForward * cos(m_angle) + vSide * sin(m_angle)) * dt;
    double nextY = m_yPosition + m_moveSpeed * (vForward * sin(-m_angle) + vSide * cos(m_angle)) * dt;

    // Wall collision management
    double halfPlayerSize = PLAYER_SIZE * 0.5;
    if ((int)(nextX - halfPlayerSize) != (int)m_xPosition || (int)(nextX + halfPlayerSize) != (int)m_xPosition)
        if (mapManager.getMapElement((unsigned int)(nextX), (unsigned int)m_yPosition) != 0)
            nextX = (nextX -  m_xPosition > 0) ? (int)nextX - 0.001 : (int)m_xPosition;
        
    if ((int)(nextY - halfPlayerSize) != (int)m_yPosition || (int)(nextY + halfPlayerSize) != (int)m_yPosition)
        if (mapManager.getMapElement((unsigned int)m_xPosition, (unsigned int)nextY) != 0)
            nextY = (nextY -  m_yPosition > 0) ? (int)nextY - 0.001 : (int)m_yPosition;

    m_xPosition = nextX;
    m_yPosition = nextY;
}

int Player::SDL_renderPlayer(SDL_Renderer *renderer, MapManager &mapManager, const unsigned int screenWidth, const unsigned int screenHeight)
{
    // Render Player
    int w = PLAYER_SIZE * screenWidth / mapManager.getWidth();
    int h = PLAYER_SIZE * screenHeight / mapManager.getHeight();
    int x = (int)((m_xPosition - PLAYER_SIZE / 2) * screenWidth / mapManager.getWidth());
    int y = (int)((m_yPosition - PLAYER_SIZE / 2) * screenHeight / mapManager.getHeight());
    SDL_Rect playerRect = { x, y, w, h };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    // Render Player viewing direction
    int xViewLine = (int)(m_xPosition * screenWidth / mapManager.getWidth());
    int yViewLine = (int)(m_yPosition * screenHeight / mapManager.getHeight());
    int xViewLineEnd = (int)((m_xPosition + cos(m_angle)) * screenWidth / mapManager.getWidth());
    int yViewLineEnd = (int)((m_yPosition - sin(m_angle)) * screenHeight / mapManager.getHeight());
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
    SDL_RenderDrawLine(renderer, xViewLine, yViewLine, xViewLineEnd, yViewLineEnd);

    return 0;
}

int Player::SDL_renderPlayerMiniMap(SDL_Renderer *renderer, MapManager &mapManager, const unsigned int screenWidth, const unsigned int screenHeight, unsigned char scaleFactor)
{
    // Render Player
    int playerSize = std::min<unsigned int>(screenWidth, screenHeight) / std::max<unsigned int>(mapManager.getWidth(), mapManager.getHeight()) / scaleFactor;
    int w = PLAYER_SIZE * playerSize;
    int h = PLAYER_SIZE * playerSize;
    int x = (m_xPosition - PLAYER_SIZE / 2) *  playerSize;
    int y = (m_yPosition - PLAYER_SIZE / 2) *  playerSize;
    SDL_Rect playerRect = { x, y, w, h };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
    SDL_RenderFillRect(renderer, &playerRect);

    // Render Player viewing direction
    int xViewLine = m_xPosition * playerSize;
    int yViewLine = m_yPosition * playerSize;
    int xViewLineEnd = (m_xPosition + cos(m_angle)) * playerSize;
    int yViewLineEnd = (m_yPosition - sin(m_angle)) * playerSize;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 64);
    SDL_RenderDrawLine(renderer, xViewLine, yViewLine, xViewLineEnd, yViewLineEnd);

    return 0;
}
