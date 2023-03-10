#include <cmath>
#include <limits>
#include <iostream>
#include <omp.h>

#include "Raycaster.hpp"
#include "SDL.h"
#include "Toolbox.hpp"
#include "Player.hpp"
#include "MapManager.hpp"

Raycaster::Raycaster()
{
    m_raysDistance = nullptr;
    m_raysX = nullptr;
    m_raysY = nullptr;
    m_raysColorR = nullptr;
    m_raysColorG = nullptr;
    m_raysColorB = nullptr;
    m_raysLightFactor = nullptr;
    m_raysAngle = nullptr;
    m_wallHeight = nullptr;
    m_raysTextureXIndex = nullptr;
    m_raysTextureYStep = nullptr;
    m_raysIsTextured = nullptr;

    m_texture.resize(TEXTURE_SIZE * TEXTURE_SIZE);
    unsigned char xorColor;
    unsigned char ycolor;
    unsigned char xycolor;
    for (int i = 0; i < TEXTURE_SIZE; i++)
    {
        for (int j = 0; j < TEXTURE_SIZE; j++)
        {
            xycolor = j * 128 / TEXTURE_SIZE + i * 128 / TEXTURE_SIZE;
            xorColor = (255 * i / TEXTURE_SIZE) ^ (255 * j / TEXTURE_SIZE);
            ycolor = j * 255 / TEXTURE_SIZE;
            m_texture.at(i + j * TEXTURE_SIZE) = SDL_Color { xorColor, xorColor, xorColor, 255 };
        }
    }

    m_movingOffset = 0;
}

Raycaster::~Raycaster()
{
    if (m_raysDistance != nullptr)
        delete[] m_raysDistance;

    if (m_raysX != nullptr)
        delete[] m_raysX;
        
    if (m_raysY != nullptr)
        delete[] m_raysY;

    if (m_raysColorR != nullptr)
        delete[] m_raysColorR;

    if (m_raysColorG != nullptr)
        delete[] m_raysColorG;

    if (m_raysColorB != nullptr)
        delete[] m_raysColorB;

    if (m_raysLightFactor != nullptr)
        delete[] m_raysLightFactor;

    if (m_raysAngle != nullptr)
        delete[] m_raysAngle;

    if (m_wallHeight != nullptr)
        delete[] m_wallHeight;

    if (m_raysTextureXIndex != nullptr)
        delete[] m_raysTextureXIndex;

    if (m_raysTextureYStep != nullptr)
        delete[] m_raysTextureYStep;

    if (m_raysIsTextured != nullptr)
        delete[] m_raysIsTextured;
}

void Raycaster::initialiseRaycaster(const unsigned int numberOfRays)
{
    m_numberOfRays = numberOfRays;
    m_raysDistance = new double[m_numberOfRays];
    m_raysX = new double[m_numberOfRays];
    m_raysY = new double[m_numberOfRays];
    m_raysColorR = new unsigned char[m_numberOfRays];
    m_raysColorG = new unsigned char[m_numberOfRays];
    m_raysColorB = new unsigned char[m_numberOfRays];
    m_raysLightFactor = new double[m_numberOfRays];
    m_raysAngle = new double[m_numberOfRays];
    m_wallHeight = new double[m_numberOfRays];
    m_raysTextureXIndex = new int[m_numberOfRays];
    m_raysTextureYStep = new double[m_numberOfRays];
    m_raysIsTextured = new bool[m_numberOfRays];
}

void Raycaster::calculateRaysDistance(Player &player, MapManager &mapManager, unsigned int fov)
{
    const double angleStep = (double)fov * Math::DEGREE_TO_RADIAN / m_numberOfRays;
    const double renderDistance = 128;
    const double heigth = 1 * 0.5;

    // Compute angle array
    m_raysAngle[0] = player.getAngle() - (double)fov * Math::DEGREE_TO_RADIAN * 0.5; 
    for (unsigned int i = 1; i < m_numberOfRays; i++)
        m_raysAngle[i] = m_raysAngle[i - 1] + angleStep; 
    
    double currentAngle;
    bool isNextRayDistanceFound = false;
    
    double rayPositionX; 
    double rayPositionY;
    double rayDirectionX;  
    double rayDirectionY;

    int nextEdgeXaxis;
    int nextEdgeYaxis;

    double currentPointToNextXaxisEdge_X;
    double currentPointToNextXaxisEdge_Y;
    double currentPointToNextYaxisEdge_Y;
    double currentPointToNextYaxisEdge_X;

    char blockHitIndex;

    double playerToRayX;
    double playerToRayY;

    double rayDistanceUncorrected;

    WallSide sideHit;

    for (unsigned int i = 0; i < m_numberOfRays; i++)
    {
        // Get current angle
        currentAngle = m_raysAngle[i];
        
        // Find ray distance
        rayPositionX = player.getX();
        rayPositionY = player.getY();   
        rayDirectionX = cos(currentAngle);
        rayDirectionY = -sin(currentAngle);

        // Calculate next edges (for X & Y axis)
        nextEdgeXaxis = (rayDirectionX > 0) ? 1 + (int)rayPositionX : (int)rayPositionX;
        nextEdgeYaxis = (rayDirectionY > 0) ? 1 + (int)rayPositionY : (int)rayPositionY;

        for (int j = 0; j < renderDistance && !isNextRayDistanceFound; j++)
        {
            // Find the closest edge
            // 1st. Calculate point to axis vector
            currentPointToNextXaxisEdge_X = nextEdgeXaxis - rayPositionX;
            currentPointToNextXaxisEdge_Y = currentPointToNextXaxisEdge_X * rayDirectionY / rayDirectionX;
            currentPointToNextYaxisEdge_Y = nextEdgeYaxis - rayPositionY;
            currentPointToNextYaxisEdge_X = currentPointToNextYaxisEdge_Y * rayDirectionX / rayDirectionY;

            // 2nd. Compare length
            if (currentPointToNextXaxisEdge_X * currentPointToNextXaxisEdge_X + currentPointToNextXaxisEdge_Y * currentPointToNextXaxisEdge_Y < currentPointToNextYaxisEdge_X * currentPointToNextYaxisEdge_X + currentPointToNextYaxisEdge_Y * currentPointToNextYaxisEdge_Y)
            {
                // X edge is closer
                rayPositionX += currentPointToNextXaxisEdge_X;
                rayPositionY += currentPointToNextXaxisEdge_Y;

                if (rayDirectionX > 0)
                {
                    nextEdgeXaxis++;
                    sideHit = WallSide::west;
                }
                else
                {
                    nextEdgeXaxis--;
                    sideHit = WallSide::east;
                }
            }
            else
            {
                // Y edge is closer
                rayPositionX += currentPointToNextYaxisEdge_X;
                rayPositionY += currentPointToNextYaxisEdge_Y;

                if (rayDirectionY > 0)
                {
                    nextEdgeYaxis++;
                    sideHit = WallSide::south;
                }
                else
                {
                    nextEdgeYaxis--;
                    sideHit = WallSide::north;
                }
            }

            rayPositionX += 1e-6 * rayDirectionX;
            rayPositionY += 1e-6 * rayDirectionY;

            // Check if the next block is a wall
            blockHitIndex = mapManager.getMapElement((unsigned int)rayPositionX, (unsigned int)rayPositionY); 
            if (blockHitIndex != 0)
            {
                isNextRayDistanceFound = true;
                playerToRayX = rayPositionX - player.getX();
                playerToRayY = rayPositionY - player.getY();

                m_raysX[i] = rayPositionX;
                m_raysY[i] = rayPositionY;

                rayDistanceUncorrected = sqrt(playerToRayX * playerToRayX + playerToRayY * playerToRayY); 
                m_raysDistance[i] = rayDistanceUncorrected;
                m_wallHeight[i] = 2 * heigth / m_raysDistance[i];            

                m_raysIsTextured[i] = false;
                if (blockHitIndex == 1)
                {
                    // White block
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 255;
                }
                else if (blockHitIndex == 2)
                {
                    // Red block
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 0;
                }
                else if (blockHitIndex == 3)
                {
                    // Red block
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 0;
                }
                else if (blockHitIndex == 4)
                {
                    // Red block
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 255;
                }
                else if (blockHitIndex == 5)
                {
                    // Texture block
                    m_raysIsTextured[i] = true;
                    
                    // Calculation of texture X index
                    if (sideHit == WallSide::south)
                        m_raysTextureXIndex[i] = (rayPositionX - (int)(rayPositionX)) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::north)
                        m_raysTextureXIndex[i] = (1 - (rayPositionX - (int)(rayPositionX))) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::east)
                        m_raysTextureXIndex[i] = (rayPositionY - (int)(rayPositionY)) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::west)
                        m_raysTextureXIndex[i] = (1 - (rayPositionY - (int)(rayPositionY))) * TEXTURE_SIZE;

                    // Calculation of Y Step 
                    m_raysTextureYStep[i] = m_wallHeight[i] / TEXTURE_SIZE;
                }
                else 
                {
                    // DEFAULT BLACK BLOCK
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 0;
                }

                m_raysLightFactor[i] = Math::limitToInterval<double>(1 - (rayDistanceUncorrected * 0.05), 0, 1);

                m_raysColorR[i] *= m_raysLightFactor[i];
                m_raysColorG[i] *= m_raysLightFactor[i];
                m_raysColorB[i] *= m_raysLightFactor[i];
            }
        }

        if (!isNextRayDistanceFound)
        {
            m_raysDistance[i] = std::numeric_limits<double>::infinity();
            m_raysX[i] = std::numeric_limits<double>::infinity();
            m_raysY[i] = std::numeric_limits<double>::infinity();

            m_wallHeight[i] = 0;            
        }

        // Setup next Ray
        currentAngle += angleStep;
        isNextRayDistanceFound = false;
    }
}

void Raycaster::calculateRaysDistance_fishEyeAndRayDistributionCorrected(Player &player, MapManager &mapManager, unsigned int fov)
{
    const double renderDistance = 128;
    const double fovRadian = (double)fov * Math::DEGREE_TO_RADIAN;
    const double angleStep = fovRadian / m_numberOfRays;
    const double inverseLinearRayDistributionFactor = 2.0 / (m_numberOfRays * tan(0.5 * fovRadian));
    const double heigth = 1 * 0.5;

    // Compute angle array
    const int halfNumberOfRays = m_numberOfRays >> 1;
    for (int i = 0; i < m_numberOfRays; i++)
        m_raysAngle[i] = player.getAngle() + atan(inverseLinearRayDistributionFactor * (i - halfNumberOfRays)); 
    
    double currentAngle;
    bool isNextRayDistanceFound = false;
    
    double rayPositionX; 
    double rayPositionY;
    double rayDirectionX;  
    double rayDirectionY;

    int nextEdgeXaxis;
    int nextEdgeYaxis;

    double currentPointToNextXaxisEdge_X;
    double currentPointToNextXaxisEdge_Y;
    double currentPointToNextYaxisEdge_Y;
    double currentPointToNextYaxisEdge_X;

    char blockHitIndex;

    double playerToRayX;
    double playerToRayY;

    double rayDistanceUncorrected;

    WallSide sideHit;

    for (unsigned int i = 0; i < m_numberOfRays; i++)
    {
        // Get current angle
        currentAngle = m_raysAngle[i];
        
        // Find ray distance
        rayPositionX = player.getX();
        rayPositionY = player.getY();   
        rayDirectionX = cos(currentAngle);
        rayDirectionY = -sin(currentAngle);

        // Calculate next edges (for X & Y axis)
        nextEdgeXaxis = (rayDirectionX > 0) ? 1 + (int)rayPositionX : (int)rayPositionX;
        nextEdgeYaxis = (rayDirectionY > 0) ? 1 + (int)rayPositionY : (int)rayPositionY;

        for (int j = 0; j < renderDistance && !isNextRayDistanceFound; j++)
        {
            // Find the closest edge
            // 1st. Calculate point to axis vector
            currentPointToNextXaxisEdge_X = nextEdgeXaxis - rayPositionX;
            currentPointToNextXaxisEdge_Y = currentPointToNextXaxisEdge_X * rayDirectionY / rayDirectionX;
            currentPointToNextYaxisEdge_Y = nextEdgeYaxis - rayPositionY;
            currentPointToNextYaxisEdge_X = currentPointToNextYaxisEdge_Y * rayDirectionX / rayDirectionY;

            // 2nd. Compare length
            if (currentPointToNextXaxisEdge_X * currentPointToNextXaxisEdge_X + currentPointToNextXaxisEdge_Y * currentPointToNextXaxisEdge_Y < currentPointToNextYaxisEdge_X * currentPointToNextYaxisEdge_X + currentPointToNextYaxisEdge_Y * currentPointToNextYaxisEdge_Y)
            {
                // X edge is closer
                rayPositionX += currentPointToNextXaxisEdge_X;
                rayPositionY += currentPointToNextXaxisEdge_Y;

                if (rayDirectionX > 0)
                {
                    nextEdgeXaxis++;
                    sideHit = WallSide::west;
                }
                else
                {
                    nextEdgeXaxis--;
                    sideHit = WallSide::east;
                }
            }
            else
            {
                // Y edge is closer
                rayPositionX += currentPointToNextYaxisEdge_X;
                rayPositionY += currentPointToNextYaxisEdge_Y;

                if (rayDirectionY > 0)
                {
                    nextEdgeYaxis++;
                    sideHit = WallSide::south;
                }
                else
                {
                    nextEdgeYaxis--;
                    sideHit = WallSide::north;
                }
            }

            rayPositionX += 1e-6 * rayDirectionX;
            rayPositionY += 1e-6 * rayDirectionY;

            // Check if the next block is a wall
            blockHitIndex = mapManager.getMapElement((unsigned int)rayPositionX, (unsigned int)rayPositionY); 
            if (blockHitIndex != 0)
            {
                isNextRayDistanceFound = true;
                playerToRayX = rayPositionX - player.getX();
                playerToRayY = rayPositionY - player.getY();

                m_raysX[i] = rayPositionX;
                m_raysY[i] = rayPositionY;

                rayDistanceUncorrected = sqrt(playerToRayX * playerToRayX + playerToRayY * playerToRayY); 
                m_raysDistance[i] = rayDistanceUncorrected * cos(currentAngle - player.getAngle());
                m_wallHeight[i] = 2 * heigth / m_raysDistance[i];            

                if (blockHitIndex == 1)
                {
                    // White block
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 255;
                }
                else if (blockHitIndex == 2)
                {
                    // Red block
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 0;
                }
                else if (blockHitIndex == 3)
                {
                    // Red block
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 0;
                }
                else if (blockHitIndex == 4)
                {
                    // Red block
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 255;
                }
                else if (blockHitIndex == 5)
                {
                    // Texture block
                    m_raysIsTextured[i] = true;
                    
                    // Calculation of texture X index
                    if (sideHit == WallSide::south)
                        m_raysTextureXIndex[i] = (rayPositionX - (int)(rayPositionX)) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::north)
                        m_raysTextureXIndex[i] = (1 - (rayPositionX - (int)(rayPositionX))) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::east)
                        m_raysTextureXIndex[i] = (rayPositionY - (int)(rayPositionY)) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::west)
                        m_raysTextureXIndex[i] = (1 - (rayPositionY - (int)(rayPositionY))) * TEXTURE_SIZE;

                    // Calculation of Y Step 
                    m_raysTextureYStep[i] = m_wallHeight[i] / TEXTURE_SIZE;
                }
                else 
                {
                    // DEFAULT BLACK BLOCK
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 0;
                }

                m_raysLightFactor[i] = Math::limitToInterval<double>(1 - (rayDistanceUncorrected * 0.05), 0, 1);

                m_raysColorR[i] *= m_raysLightFactor[i];
                m_raysColorG[i] *= m_raysLightFactor[i];
                m_raysColorB[i] *= m_raysLightFactor[i];
            }
        }

        if (!isNextRayDistanceFound)
        {
            m_raysDistance[i] = std::numeric_limits<double>::infinity();
            m_raysX[i] = std::numeric_limits<double>::infinity();
            m_raysY[i] = std::numeric_limits<double>::infinity();   

            m_wallHeight[i] = 0;            
        }

        // Setup next Ray
        currentAngle += angleStep;
        isNextRayDistanceFound = false;
    }
}

void Raycaster::calculateRaysDistance_OMP(Player &player, MapManager &mapManager, unsigned int fov)
{
    const double renderDistance = 128;
    const double fovRadian = (double)fov * Math::DEGREE_TO_RADIAN;
    const double inverseLinearRayDistributionFactor = 2.0 / (m_numberOfRays * tan(0.5 * fovRadian));
    const double heigth = 1 * 0.5;

    // Compute angle array
    const int halfNumberOfRays = m_numberOfRays >> 1;
#pragma omp parallel for
    for (int i = 0; i < m_numberOfRays; i++)
        m_raysAngle[i] = player.getAngle() + atan(inverseLinearRayDistributionFactor * (i - halfNumberOfRays)); 
    
    double currentAngle;
    bool isNextRayDistanceFound = false;
    
    double rayPositionX; 
    double rayPositionY;
    double rayDirectionX;  
    double rayDirectionY;

    int nextEdgeXaxis;
    int nextEdgeYaxis;

    double currentPointToNextXaxisEdge_X;
    double currentPointToNextXaxisEdge_Y;
    double currentPointToNextYaxisEdge_Y;
    double currentPointToNextYaxisEdge_X;

    char blockHitIndex;

    double playerToRayX;
    double playerToRayY;

    double rayDistanceUncorrected;

    WallSide sideHit;

#pragma omp parallel for firstprivate(isNextRayDistanceFound) private(currentAngle, rayPositionX, rayPositionY, rayDirectionX, rayDirectionY, nextEdgeXaxis, nextEdgeYaxis, currentPointToNextXaxisEdge_X, currentPointToNextXaxisEdge_Y, currentPointToNextYaxisEdge_Y, currentPointToNextYaxisEdge_X, blockHitIndex, playerToRayX, playerToRayY, rayDistanceUncorrected)
    for (unsigned int i = 0; i < m_numberOfRays; i++)
    {
        // Get current angle
        currentAngle = m_raysAngle[i];

        // Find ray distance
        rayPositionX = player.getX();
        rayPositionY = player.getY();   
        rayDirectionX = cos(currentAngle);
        rayDirectionY = -sin(currentAngle);

        // Calculate next edges (for X & Y axis)
        nextEdgeXaxis = (rayDirectionX > 0) ? 1 + (int)rayPositionX : (int)rayPositionX;
        nextEdgeYaxis = (rayDirectionY > 0) ? 1 + (int)rayPositionY : (int)rayPositionY;

        for (int j = 0; j < renderDistance && !isNextRayDistanceFound; j++)
        {
            // Find the closest edge
            // 1st. Calculate point to axis vector
            currentPointToNextXaxisEdge_X = nextEdgeXaxis - rayPositionX;
            currentPointToNextXaxisEdge_Y = currentPointToNextXaxisEdge_X * rayDirectionY / rayDirectionX;
            currentPointToNextYaxisEdge_Y = nextEdgeYaxis - rayPositionY;
            currentPointToNextYaxisEdge_X = currentPointToNextYaxisEdge_Y * rayDirectionX / rayDirectionY;

            // 2nd. Compare length
            if (currentPointToNextXaxisEdge_X * currentPointToNextXaxisEdge_X + currentPointToNextXaxisEdge_Y * currentPointToNextXaxisEdge_Y < currentPointToNextYaxisEdge_X * currentPointToNextYaxisEdge_X + currentPointToNextYaxisEdge_Y * currentPointToNextYaxisEdge_Y)
            {
                // X edge is closer
                rayPositionX += currentPointToNextXaxisEdge_X;
                rayPositionY += currentPointToNextXaxisEdge_Y;

                if (rayDirectionX > 0)
                {
                    nextEdgeXaxis++;
                    sideHit = WallSide::west;
                }
                else
                {
                    nextEdgeXaxis--;
                    sideHit = WallSide::east;
                }
            }
            else
            {
                // Y edge is closer
                rayPositionX += currentPointToNextYaxisEdge_X;
                rayPositionY += currentPointToNextYaxisEdge_Y;

                if (rayDirectionY > 0)
                {
                    nextEdgeYaxis++;
                    sideHit = WallSide::south;
                }
                else
                {
                    nextEdgeYaxis--;
                    sideHit = WallSide::north;
                }
            }

            rayPositionX += 1e-6 * rayDirectionX;
            rayPositionY += 1e-6 * rayDirectionY;

            // Check if the next block is a wall
            blockHitIndex = mapManager.getMapElement((unsigned int)rayPositionX, (unsigned int)rayPositionY); 
            if (blockHitIndex != 0)
            {
                isNextRayDistanceFound = true;
                playerToRayX = rayPositionX - player.getX();
                playerToRayY = rayPositionY - player.getY();

                m_raysX[i] = rayPositionX;
                m_raysY[i] = rayPositionY;

                rayDistanceUncorrected = sqrt(playerToRayX * playerToRayX + playerToRayY * playerToRayY); 
                m_raysDistance[i] = rayDistanceUncorrected * cos(currentAngle - player.getAngle());
                m_wallHeight[i] = 2 * heigth / m_raysDistance[i];            

                m_raysIsTextured[i] = false;
                if (blockHitIndex == 1)
                {
                    // White block
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 255;
                }
                else if (blockHitIndex == 2)
                {
                    // Red block
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 0;
                }
                else if (blockHitIndex == 3)
                {
                    // Red block
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 0;
                }
                else if (blockHitIndex == 4)
                {
                    // Red block
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 255;
                }
                else if (blockHitIndex == 5)
                {
                    // Texture block
                    m_raysIsTextured[i] = true;
                    
                    // Calculation of texture X index
                    if (sideHit == WallSide::south)
                        m_raysTextureXIndex[i] = (rayPositionX - (int)(rayPositionX)) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::north)
                        m_raysTextureXIndex[i] = (1 - (rayPositionX - (int)(rayPositionX))) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::east)
                        m_raysTextureXIndex[i] = (rayPositionY - (int)(rayPositionY)) * TEXTURE_SIZE;
                    else if (sideHit == WallSide::west)
                        m_raysTextureXIndex[i] = (1 - (rayPositionY - (int)(rayPositionY))) * TEXTURE_SIZE;

                    // Calculation of Y Step 
                    m_raysTextureYStep[i] = m_wallHeight[i] / TEXTURE_SIZE;

                    // Lightning
                    m_raysColorR[i] = 255;
                    m_raysColorG[i] = 255;
                    m_raysColorB[i] = 255;
                }
                else 
                {
                    // DEFAULT BLACK BLOCK
                    m_raysColorR[i] = 0;
                    m_raysColorG[i] = 0;
                    m_raysColorB[i] = 0;
                }

                m_raysLightFactor[i] = Math::limitToInterval<double>(1 - (rayDistanceUncorrected * 0.05), 0, 1);

                m_raysColorR[i] *= m_raysLightFactor[i];
                m_raysColorG[i] *= m_raysLightFactor[i];
                m_raysColorB[i] *= m_raysLightFactor[i];
            }
        }

        if (!isNextRayDistanceFound)
        {
            m_raysDistance[i] = std::numeric_limits<double>::infinity();
            m_raysX[i] = std::numeric_limits<double>::infinity();
            m_raysY[i] = std::numeric_limits<double>::infinity(); 

            m_wallHeight[i] = 0;            
        }

        // Setup next Ray
        isNextRayDistanceFound = false;
    }
}

void Raycaster::SDL_renderRaycast2DMap(SDL_Renderer *renderer, MapManager &mapManager, Player &player, const unsigned int screenWidth, const unsigned int screenHeigth)
{
    int x1 = player.getX() * screenWidth / mapManager.getWidth();
    int y1 = player.getY() * screenHeigth / mapManager.getHeight();

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);

    for (unsigned int i = 0; i < m_numberOfRays; i++)
    {
        int x2;
        int y2;
        if (std::isinf(m_raysDistance[i]))
        {
            x2 = x1;
            y2 = x1;
        }
        else
        {
            x2 = m_raysX[i] * screenWidth / mapManager.getWidth();
            y2 = m_raysY[i] * screenHeigth / mapManager.getHeight();
        }

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void Raycaster::SDL_renderRaycast2DMiniMap(SDL_Renderer *renderer, MapManager &mapManager, Player &player, const unsigned int screenWidth, const unsigned int screenHeigth, const unsigned int scaleFactor)
{
    int rayScreenSize = std::min<unsigned int>(screenWidth, screenHeigth) / std::max<unsigned int>(mapManager.getWidth(), mapManager.getHeight()) / scaleFactor;
    int x1 = player.getX() * rayScreenSize;
    int y1 = player.getY() * rayScreenSize;


    for (unsigned int i = 0; i < m_numberOfRays; i++)
    {
        int x2;
        int y2;
        if (std::isinf(m_raysDistance[i]))
        {
            x2 = x1;
            y2 = x1;
        }
        else
        {
            x2 = m_raysX[i] * rayScreenSize;
            y2 = m_raysY[i] * rayScreenSize;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255 * m_raysLightFactor[i], 64 * m_raysLightFactor[i]);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void Raycaster::SDL_renderRaycastBackground(SDL_Renderer *renderer, const double currentVelocity, const double time, const unsigned int screenWidth, const unsigned int screenHeigth)
{
    // TODO: optimise using precalculated color array
    const double maxBrightness = 45;
    double brightness = maxBrightness; 
    int j = screenHeigth - 1;
    m_movingOffset = MOVING_OFFSET_MAGNITUDE * currentVelocity * cos(4 * M_PI * time) / screenHeigth;

    for (unsigned int i = 0; i < screenHeigth/2; i++)
    {
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);
        SDL_RenderDrawLine(renderer, 0, i + m_movingOffset, screenWidth - 1, i + m_movingOffset);
        SDL_RenderDrawLine(renderer, 0, j + m_movingOffset, screenWidth - 1, j + m_movingOffset);
        brightness -= maxBrightness * 2 / screenHeigth;  
        j--;
    }
}

void Raycaster::SDL_renderRaycast(SDL_Renderer *renderer, const double currentVelocity, const double time, const unsigned int screenWidth, const unsigned int screenHeigth)
{
    double xStep = screenWidth / m_numberOfRays;
    int x = (m_numberOfRays - 1) * xStep;
    
    for(unsigned int i = 0; i < m_numberOfRays; i++)
    {
        SDL_Rect rectangle;
        if (m_raysIsTextured[i])
        {
            unsigned char r, g, b, a;
            double yStep = m_raysTextureYStep[i] * screenHeigth;
            double y = (screenHeigth - m_wallHeight[i] * screenHeigth) / 2;
            double nextY;
            SDL_Color color;
            // #pragma omp parallel for private(color, )
            for (unsigned int j = 0; j < TEXTURE_SIZE; j++)
            {
                nextY = y + yStep;
                if ((nextY < 0) || ( screenHeigth <= y ))
                {
                    y = nextY;
                    continue;
                }
                color = m_texture.at(m_raysTextureXIndex[i] + j * TEXTURE_SIZE);
                r = color.r * (double)m_raysColorR[i] / 255;
                g = color.g * (double)m_raysColorG[i] / 255;
                b = color.b * (double)m_raysColorB[i] / 255;
                a = color.a;
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                rectangle = { x, (int)(y + m_movingOffset), (int)xStep, (int)yStep + 1 };
                SDL_RenderFillRect(renderer, &rectangle);
                y = nextY;
            }
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, m_raysColorR[i], m_raysColorG[i], m_raysColorB[i], 255);
            int h = m_wallHeight[i] * screenHeigth;
            int y = (screenHeigth - m_wallHeight[i] * screenHeigth) / 2;
            rectangle = { x, (int)(y + m_movingOffset), (int)xStep, h };
            SDL_RenderFillRect(renderer, &rectangle);
        }
        x -= xStep;
    }
}