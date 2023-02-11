#include <iostream>

#include "Capp.hpp"
#include "SDL.h"

// TODO: config.cfg

Capp::Capp()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_screenWidth = 1280;
    m_screenHeight = 720;
    m_windowFlags = SDL_WINDOW_SHOWN;
    m_vForward = 0;
    m_vSide = 0;
    m_mouseMoved = false;

    m_previousTimePoint = std::chrono::high_resolution_clock::now();

    m_player.initialisePlayer(m_mapManager);
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
    std::cout << '\n';
    
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

    // Set fullscreen
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) == 0)
    {
        m_windowFlags = SDL_WINDOW_FULLSCREEN;
        m_screenWidth = displayMode.w;
        m_screenHeight = displayMode.h;
    }

    // Initialise Raycasting
    m_raycaster.initialiseRaycaster(m_screenWidth);

    // Initialise Window
    m_window = SDL_CreateWindow("Raycasting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screenWidth, m_screenHeight, m_windowFlags);
    if (m_window == nullptr)
        return false;

    // Initialise Renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr)
        return false;

    // Initialise blend mode
    if (SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND) != 0)
        return false;

    // Initialise relative mouse mode
    if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0)
        return false;
        
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
                else if (events.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    m_isRunning = false;
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

            case SDL_MOUSEMOTION:
                m_mouseMoved = true;
                m_angularSpeed = -events.motion.xrel;
                break;

            default:
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
    m_raycaster.calculateRaysDistance_OMP(m_player, m_mapManager);

    // Reset rotation if mouse moved
    if (m_mouseMoved)
    {
        m_mouseMoved = false;
        m_angularSpeed = 0;
    }
}

void Capp::render()
{
    // Clear renderer
    SDL_SetRenderDrawColor(m_renderer, 70, 70, 70, 255);
    SDL_RenderClear(m_renderer);

    // Render map (2D)
    // m_mapManager.SDL_renderMap(m_renderer, m_screenWidth, m_screenHeight);
    // m_player.SDL_renderPlayer(m_renderer, m_mapManager, m_screenWidth, m_screenHeight);
    // m_raycaster.SDL_renderRaycast2DMap(m_renderer, m_mapManager, m_player, m_screenWidth, m_screenHeight);
    
    // Render 2.5D environment
    // Render sky & ground (TODO)

    // Render walls
    m_raycaster.SDL_renderRaycastBackground(m_renderer, m_screenWidth, m_screenHeight);
    m_raycaster.SDL_renderRaycast(m_renderer, m_screenWidth, m_screenHeight);

    // Render Minimap
    m_mapManager.SDL_renderMiniMap(m_renderer, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);
    m_raycaster.SDL_renderRaycast2DMiniMap(m_renderer, m_mapManager, m_player, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);
    m_player.SDL_renderPlayerMiniMap(m_renderer, m_mapManager, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);

    // Render
    SDL_RenderPresent(m_renderer);
}