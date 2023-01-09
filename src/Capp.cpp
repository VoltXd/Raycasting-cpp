#include <iostream>

#include "Capp.hpp"
#include "SDL.h"

Capp::Capp()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_screenWidth = 1280;
    m_screenHeight = 720;

    m_vForward = 0;
    m_vSide = 0;

    m_previousTimePoint = std::chrono::high_resolution_clock::now();

    m_player.initialisePlayer(m_mapManager);
    m_raycaster.initialiseRaycaster(m_screenWidth);
}

bool Capp::run()
{
    // Initialise App
    m_isRunning = initialise();
    if (!m_isRunning)
        return false;
    
    // Main loop
    while (m_isRunning)
    {
        input();
        update();
        render();
    }
    
    // Destroy components
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();

    return true;
}

bool Capp::initialise()
{
    // Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        return false;

    // Initialise Window
    m_window = SDL_CreateWindow("Raycasting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN);
    if (m_window == nullptr)
        return false;

    // Initialise Renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr)
        return false;

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        
    return true;
}

void Capp::input()
{
    SDL_Event events;
    while(SDL_PollEvent(&events))
    {
        switch(events.type)
        {
            case SDL_QUIT:
                m_isRunning = false;
                break;

            case SDL_KEYDOWN:
                // Move
                if (events.key.keysym.scancode == SDL_SCANCODE_W)
                    m_vForward = 1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_S)
                    m_vForward = -1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_D)
                    m_vSide = 1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_A)
                    m_vSide = -1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_LEFT)
                    m_angularSpeed = 1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                    m_angularSpeed = -1;
                
                break;

            case SDL_KEYUP:
                // Stop moving
                if (events.key.keysym.scancode == SDL_SCANCODE_W)
                {
                    if (m_vForward > 0)
                        m_vForward = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_S)
                {
                    if (m_vForward < 0)
                        m_vForward = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    if (m_vSide > 0)
                        m_vSide = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    if (m_vSide < 0)
                        m_vSide = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    if (m_angularSpeed > 0)
                        m_angularSpeed = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    if (m_angularSpeed < 0)
                        m_angularSpeed = 0;   
                }
                break;
        }
    }
}

void Capp::update()
{
    // Calculate elapsed time
    auto currentTimePoint = std::chrono::high_resolution_clock::now();
    unsigned long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint - m_previousTimePoint).count();
    m_previousTimePoint = currentTimePoint;
    
    // Print FPS
    std::cout << "FPS: " << 1e6 / elapsedTime << '\r';
    
    // Player actions
    m_player.movePlayer(m_mapManager, m_vForward, m_vSide, 1e-6 * elapsedTime);
    m_player.rotatePlayer(m_angularSpeed, 1e-6 * elapsedTime);

    // Player vision
    m_raycaster.calculateRaysDistance(m_player, m_mapManager);
}

void Capp::render()
{
    // Clear renderer
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    // Render map (2D)
    // m_mapManager.SDL_renderMap(m_renderer, m_screenWidth, m_screenHeight);
    // m_player.SDL_renderPlayer(m_renderer, m_mapManager, m_screenWidth, m_screenHeight);
    // m_raycaster.SDL_renderRaycast2DMap(m_renderer, m_mapManager, m_player, m_screenWidth, m_screenHeight);
    
    // Render 2.5D environment
    // Render sky & ground (TODO)

    // Render walls
    m_raycaster.SDL_renderRaycast(m_renderer, m_screenWidth, m_screenHeight);

    // Render Minimap
    m_mapManager.SDL_renderMiniMap(m_renderer, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);
    m_player.SDL_renderPlayerMiniMap(m_renderer, m_mapManager, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);
    m_raycaster.SDL_renderRaycast2DMiniMap(m_renderer, m_mapManager, m_player, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);


    // Render
    SDL_RenderPresent(m_renderer);
}