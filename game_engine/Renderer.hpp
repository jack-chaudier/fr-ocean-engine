//
//  Renderer.hpp
//  FR-Ocean Engine
//
//  SDL2-based 2D rendering system with camera support (position, zoom) and
//  deferred rendering pipeline for sprites, text, and UI elements.
//
//  Created by Jack Gaffney on 2/7/25.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include <string>

/**
 * @class Renderer
 * @brief SDL2-based 2D rendering system with camera and viewport management.
 *
 * The Renderer class encapsulates SDL2 window and rendering context management,
 * providing a camera system for world-space rendering and utility methods for
 * clearing and presenting frames.
 *
 * Features:
 * - SDL2 hardware-accelerated rendering
 * - 2D camera system with position and zoom
 * - VSync support for smooth frame pacing
 * - Configurable window title, resolution, and clear color
 * - Static accessor pattern for global rendering access
 *
 * Camera System:
 * - camera_pos: World-space position (affects all non-UI rendering)
 * - zoom_factor: Uniform scale factor applied via SDL_RenderSetScale()
 * - camera_dimensions: Viewport size (typically matches window resolution)
 *
 * @note The Renderer uses static members to allow global access from
 *       ImageDB, TextDB, and other rendering subsystems.
 */
class Renderer {
public:
    /**
     * @brief Constructs the Renderer and initializes SDL2 window and rendering context.
     *
     * @param title Window title displayed in the OS window chrome
     * @param clearColor RGB clear color used when clearing the screen (0-255 per channel)
     * @param resolution Window resolution in pixels (width, height)
     *
     * @throws std::runtime_error if SDL2 initialization fails or window/renderer creation fails
     *
     * @note Automatically enables VSync via SDL_RENDERER_PRESENTVSYNC flag
     */
    Renderer(const std::string &title, glm::ivec3 &clearColor, const glm::ivec2 &resolution);

    /**
     * @brief Destructs the Renderer and cleans up SDL2 resources.
     *
     * Cleanup order:
     * 1. Destroy SDL_Renderer
     * 2. Destroy SDL_Window
     */
    ~Renderer();

    /**
     * @brief Clears the screen with the specified color.
     *
     * @param color RGB clear color (0-255 per channel)
     *
     * @note This should be called at the beginning of every frame before rendering
     */
    static void clear(glm::ivec3 color);

    /**
     * @brief Presents the current frame to the screen (swap buffers).
     *
     * @note This should be called at the end of every frame after all rendering
     */
    static void present();

    /**
     * @brief Gets the SDL2 renderer instance for direct SDL rendering calls.
     *
     * @return Pointer to the SDL_Renderer instance
     */
    static SDL_Renderer* getSDLRenderer() { return renderer; }

    /**
     * @brief Gets the SDL2 window instance for window management.
     *
     * @return Pointer to the SDL_Window instance
     */
    static SDL_Window* getSDLWindow() { return window; }

    /**
     * @brief Gets the current camera zoom factor.
     *
     * @return Current zoom factor (1.0 = normal, 2.0 = 2x zoom, 0.5 = 50% zoom)
     */
    static float GetCameraZoomFactor() { return zoom_factor; }

    /**
     * @brief Sets the camera zoom factor and updates SDL render scale.
     *
     * @param zoom New zoom factor (e.g., 2.0 for 2x zoom, 0.5 for 50% zoom)
     *
     * @note Zoom is applied via SDL_RenderSetScale() and affects all rendering
     */
    static void SetCameraZoomFactor(float zoom) {
        zoom_factor = zoom;
        if (renderer) SDL_RenderSetScale(renderer, zoom, zoom);
    }

    /**
     * @brief Sets the camera position in world space.
     *
     * @param x World X coordinate
     * @param y World Y coordinate
     *
     * @note Camera position affects all world-space rendering (not UI elements)
     */
    static void SetCameraPosition(float x, float y) { camera_pos = { x, y }; }

    /**
     * @brief Gets the current camera position in world space.
     *
     * @return 2D vector containing camera position
     */
    static glm::vec2 GetCameraPosition() { return camera_pos; }

    /**
     * @brief Gets the camera X position in world space.
     *
     * @return Camera X coordinate
     */
    static float GetCameraPositionX() { return camera_pos.x; }

    /**
     * @brief Gets the camera Y position in world space.
     *
     * @return Camera Y coordinate
     */
    static float GetCameraPositionY() { return camera_pos.y; }

    /**
     * @brief Gets the camera viewport dimensions.
     *
     * @return 2D vector containing viewport width and height in pixels
     */
    static glm::ivec2 GetCameraDimensions() { return camera_dimensions; }

    /// Current camera position in world space (affects all non-UI rendering)
    inline static glm::vec2 camera_pos = {0.0f, 0.0f};

    /// Current zoom factor (1.0 = normal scale, applied via SDL_RenderSetScale)
    inline static float zoom_factor = 1.0f;

    /// Camera viewport dimensions in pixels (typically matches window resolution)
    inline static glm::ivec2 camera_dimensions;

private:
    std::string title;          ///< Window title
    glm::ivec3 clearColor;      ///< Background clear color (RGB 0-255)
    glm::ivec2 resolution;      ///< Window resolution in pixels

    /// SDL2 window handle
    inline static SDL_Window *window = nullptr;

    /// SDL2 renderer handle (hardware-accelerated, VSync enabled)
    inline static SDL_Renderer *renderer = nullptr;
};

#endif /* Renderer_hpp */
