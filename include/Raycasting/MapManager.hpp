#pragma once

#include "SDL.h"

class MapManager
{
    public: 
    MapManager();
    MapManager(const unsigned int width, const unsigned int height);
    ~MapManager();

    int SDL_renderMap(SDL_Renderer *renderer, const unsigned int screenWidth, const unsigned int screenHeight);
    int SDL_renderMiniMap(SDL_Renderer *renderer, const unsigned int screenWidth, const unsigned int screenHeight, unsigned char scaleFactor);

    inline unsigned int coordinateToIndex(unsigned int x, unsigned int y) { return x + (y * m_height); }
    inline unsigned int getWidth() { return m_width; }
    inline unsigned int getHeight() { return m_height; }
    inline char getMapElement(unsigned int x, unsigned int y) { return m_mapArray[coordinateToIndex(x, y)]; }
    
    

    private:
    char *m_mapArray;
    unsigned int m_width;
    unsigned int m_height;

    const unsigned int DEFAULT_SIZE = 32;
};