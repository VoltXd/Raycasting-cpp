#include <iostream>

#include "MapManager.hpp"
#include "SDL.h"

MapManager::MapManager()
{
    // Default constructor, used for debuging only
    
    // Setup map array
    m_width = DEFAULT_SIZE;
    m_height = DEFAULT_SIZE;
    m_mapArray = new char[m_width * m_height] { 0 };

    // Fill array
    for(unsigned int i = 0; i < DEFAULT_SIZE; i++)
    {
        m_mapArray[coordinateToIndex(i, 0)] = 1;
        m_mapArray[coordinateToIndex(0, i)] = 2;
        m_mapArray[coordinateToIndex(i, DEFAULT_SIZE-1)] = 3;
        m_mapArray[coordinateToIndex(DEFAULT_SIZE-1, i)] = 4;
    }
    
    m_mapArray[coordinateToIndex(4, 3)] = 3;
    m_mapArray[coordinateToIndex(4, 5)] = 3;
    m_mapArray[coordinateToIndex(4, 7)] = 3;

    m_mapArray[coordinateToIndex(20, 10)] = 2;
    m_mapArray[coordinateToIndex(20, 11)] = 2;
    m_mapArray[coordinateToIndex(20, 12)] = 2;
    m_mapArray[coordinateToIndex(21, 12)] = 2;
    m_mapArray[coordinateToIndex(22, 12)] = 2;
    m_mapArray[coordinateToIndex(22, 11)] = 2;
    m_mapArray[coordinateToIndex(22, 10)] = 2;

}

MapManager::MapManager(const unsigned int width, const unsigned int height)
{

    // Setup map array
    m_width = width;
    m_height = height;
    m_mapArray = new char[m_width * m_height] { 0 };

    // Fill array
    for(unsigned int i = 0; i < m_width; i++)
    {
        m_mapArray[coordinateToIndex(i, 0)] = 1;
        m_mapArray[coordinateToIndex(i, m_height-1)] = 1;
    }

    for (unsigned int i = 0; i < m_height; i++)
    {
        m_mapArray[coordinateToIndex(0, i)] = 1;
        m_mapArray[coordinateToIndex(m_width-1, i)] = 1;
    }
}

MapManager::~MapManager()
{
    delete[] m_mapArray;
}


int MapManager::SDL_renderMap(SDL_Renderer *renderer, const unsigned int screenWidth, const unsigned int screenHeight)
{
    SDL_Rect tile = { 0, 0, (int)(screenWidth / m_width + 1), (int)(screenHeight / m_height + 1) };
    for (unsigned int i = 0; i < m_width; i++)
    {
        for (unsigned int j = 0; j < m_height; j++)
        {
            if (m_mapArray[coordinateToIndex(i, j)] == 0)
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else if (m_mapArray[coordinateToIndex(i, j)] == 1)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else if (m_mapArray[coordinateToIndex(i, j)] == 2)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            else if (m_mapArray[coordinateToIndex(i, j)] == 3)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else if (m_mapArray[coordinateToIndex(i, j)] == 4)
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            

            tile.x = (int)(i * screenWidth / m_width);
            tile.y = (int)(j * screenHeight / m_height);

            SDL_RenderFillRect(renderer, &tile);
        }
    }

    return 0;
}

int MapManager::SDL_renderMiniMap(SDL_Renderer *renderer, const unsigned int screenWidth, const unsigned int screenHeight, unsigned char scaleFactor)
{
    int miniMapSize = std::min<unsigned int>(screenWidth, screenHeight) / std::max<unsigned int>(m_width, m_height) / scaleFactor; 
    SDL_Rect tile = { 0, 0, miniMapSize , miniMapSize };
    for (unsigned int i = 0; i < m_width; i++)
    {
        for (unsigned int j = 0; j < m_height; j++)
        {
            if (m_mapArray[coordinateToIndex(i, j)] == 0)
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
            else if (m_mapArray[coordinateToIndex(i, j)] == 1)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
            else if (m_mapArray[coordinateToIndex(i, j)] == 2)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
            else if (m_mapArray[coordinateToIndex(i, j)] == 3)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
            else if (m_mapArray[coordinateToIndex(i, j)] == 4)
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);

            SDL_RenderFillRect(renderer, &tile);
            tile.y += miniMapSize;
        }
        tile.x += miniMapSize;
        tile.y = 0;
    }

    return 0;
}