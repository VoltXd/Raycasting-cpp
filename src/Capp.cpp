#include <iostream>
#include <string>

#include "Capp.hpp"
#include "SDL.h"
#include "SDL_ttf.h"
#include "Toolbox.hpp"

// TODO: config.cfg
// TODO: ImGui
// TODO: RenderLines, RenderRects

Capp::Capp()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_screenWidth = 1280;
    m_screenHeight = 720;
    m_windowFlags = SDL_WINDOW_SHOWN;
    m_accelForward = 0;
    m_accelSide = 0;
    m_mouseMoved = false;
    m_isSprinting = false;
    m_fov = 90;
    
    m_FPStextColor = { 255, 255, 255 };

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
    
    // Destroy components
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    TTF_CloseFont(m_font);
    TTF_Quit();
    SDL_Quit();

    return true;
}

bool Capp::initialise()
{
    // Initialise SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
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
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (m_renderer == nullptr)
        return false;

    // Initialise blend mode
    if (SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND) != 0)
        return false;

    // Initialise relative mouse mode
    if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0)
        return false;

    // Initialise font
    if (TTF_Init() != 0)
        return false;

    // Open font
    m_font = TTF_OpenFont("imports/fonts/retro_computer_personal_use.ttf", 64);
    if (m_font == nullptr)
        m_font = TTF_OpenFont("../imports/fonts/retro_computer_personal_use.ttf", 64);
    
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
                    m_accelForward = 1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_S)
                    m_accelForward = -1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_D)
                    m_accelSide = 1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_A)
                    m_accelSide = -1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_LEFT)
                    m_angularSpeed = 1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                    m_angularSpeed = -1;
                else if (events.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
                    m_isSprinting = true;
                else if (events.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    m_isRunning = false;
                break;

            case SDL_KEYUP:
                // Stop moving
                if (events.key.keysym.scancode == SDL_SCANCODE_W)
                {
                    if (m_accelForward > 0)
                        m_accelForward = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_S)
                {
                    if (m_accelForward < 0)
                        m_accelForward = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    if (m_accelSide > 0)
                        m_accelSide = 0;
                }
                else if (events.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    if (m_accelSide < 0)
                        m_accelSide = 0;
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
                else if (events.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
                    m_isSprinting = false;
                break;

            case SDL_MOUSEMOTION:
                m_mouseMoved = true;
                m_angularSpeed = -events.motion.xrel;
                break;

            case SDL_MOUSEWHEEL:
                m_fov = Math::limitToInterval<unsigned int>(m_fov + events.wheel.y, 60, 120);
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
    
    // FPS
    m_FPSstring = std::string("FPS: ");
    m_FPSstring.append(std::to_string((int)(1e6 / elapsedTime)));
    m_FPStextSurface = TTF_RenderText_Solid(m_font, m_FPSstring.c_str(), m_FPStextColor);
    m_FPStextTexture = SDL_CreateTextureFromSurface(m_renderer, m_FPStextSurface);
    int textureWidth = 0;
    int textureHeight = 0;
    SDL_QueryTexture(m_FPStextTexture, nullptr, nullptr, &textureWidth, &textureHeight);
    m_FPStextTextureRect = { 0, (int)m_screenHeight - textureHeight, textureWidth, textureHeight };
    
    // Player actions
    m_player.movePlayer(m_mapManager, m_accelForward, m_accelSide, m_isSprinting, 1e-6 * elapsedTime);
    m_player.rotatePlayer(m_angularSpeed, 1e-6 * elapsedTime);

    // Player vision
    m_raycaster.calculateRaysDistance_OMP(m_player, m_mapManager, m_fov);

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
    m_raycaster.SDL_renderRaycastBackground(m_renderer, m_player.getVelocity(), std::chrono::duration_cast<std::chrono::microseconds>(m_previousTimePoint.time_since_epoch()).count() * 1e-6, m_screenWidth, m_screenHeight);
    m_raycaster.SDL_renderRaycast(m_renderer, m_player.getVelocity(), std::chrono::duration_cast<std::chrono::microseconds>(m_previousTimePoint.time_since_epoch()).count() * 1e-6, m_screenWidth, m_screenHeight);

    // Render Minimap
    m_mapManager.SDL_renderMiniMap(m_renderer, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);
    m_raycaster.SDL_renderRaycast2DMiniMap(m_renderer, m_mapManager, m_player, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);
    m_player.SDL_renderPlayerMiniMap(m_renderer, m_mapManager, m_screenWidth, m_screenHeight, MINIMAP_SCALE_FACTOR);

    // Render FPS
    SDL_RenderCopy(m_renderer, m_FPStextTexture, nullptr, &m_FPStextTextureRect);

    // Deallocate FPS's texture & surface
    SDL_FreeSurface(m_FPStextSurface);
    SDL_DestroyTexture(m_FPStextTexture);

    // Render
    SDL_RenderPresent(m_renderer);
}