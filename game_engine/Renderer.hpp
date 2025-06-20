//
//  Renderer.hpp
//  game_engine
//
//  Created by Jack Gaffney on 2/7/25.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include <string>

class Renderer {
public:
    Renderer(const std::string &title, glm::ivec3 &clearColor, const glm::ivec2 &resolution);
    ~Renderer();

    static void clear(glm::ivec3 color);
    static void present();
    static SDL_Renderer* getSDLRenderer() { return renderer; }
    static SDL_Window* getSDLWindow() { return window; }
    
    static float GetCameraZoomFactor() { return zoom_factor; }
    static void SetCameraZoomFactor(float zoom) {
        zoom_factor = zoom;
        if (renderer) SDL_RenderSetScale(renderer, zoom, zoom);
    }
    static void SetCameraPosition(float x, float y) { camera_pos = { x, y }; }
    static glm::vec2 GetCameraPosition() { return camera_pos; }
    static float GetCameraPositionX() { return camera_pos.x; }
    static float GetCameraPositionY() { return camera_pos.y; }
    static glm::ivec2 GetCameraDimensions() { return camera_dimensions; }

    inline static glm::vec2 camera_pos = {0.0f, 0.0f};
    inline static float zoom_factor = 1.0f;
    inline static glm::ivec2 camera_dimensions;

private:
    std::string title;
    glm::ivec3 clearColor;
    glm::ivec2 resolution;
    inline static SDL_Window *window = nullptr;
    inline static SDL_Renderer *renderer = nullptr;
};

#endif /* Renderer_hpp */
