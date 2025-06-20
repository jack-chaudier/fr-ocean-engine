//
//  Renderer.cpp
//  game_engine
//
//  Created by Jack Gaffney on 2/7/25.
//

#include "Renderer.hpp"
#include "Helper.h"
#include <iostream>

Renderer::Renderer(const std::string& title, glm::ivec3& clearColor, const glm::ivec2& resolution)
    : title(title), clearColor(clearColor), resolution(resolution) {
    window = Helper::SDL_CreateWindow(title.c_str(), 100, 100, resolution.x, resolution.y, SDL_WINDOW_SHOWN);

    renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    // Initialize camera dimensions with the window resolution
    camera_dimensions = resolution;
}

void Renderer::clear(glm::ivec3 color) {
    SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 0);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    Helper::SDL_RenderPresent(renderer);
}

Renderer::~Renderer() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}
