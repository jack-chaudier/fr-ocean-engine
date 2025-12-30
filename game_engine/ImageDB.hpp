//
//  ImageDB.hpp
//  FR-Ocean Engine
//
//  Deferred sprite rendering system with texture caching, sorting, transformations,
//  and pixel drawing capabilities.
//
//  Created by Jack Gaffney on 2/9/25.
//

#ifndef ImageDB_hpp
#define ImageDB_hpp

#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "Helper.h"
#include "glm/glm.hpp"
#include "Renderer.hpp"
#include "ConfigManager.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>

/**
 * @struct ImageDrawRequest
 * @brief Encapsulates all parameters for a deferred sprite draw call.
 *
 * Draw requests are queued during the Update phase and rendered in batch
 * during the Render phase, sorted by sorting_order for z-depth control.
 */
struct ImageDrawRequest {
    std::string image_name;     ///< Texture name (loaded from resources/images/)
    float x;                    ///< X position (world-space or UI-space)
    float y;                    ///< Y position (world-space or UI-space)
    int rotation_degrees;       ///< Rotation in degrees (clockwise)
    float scale_x;              ///< Horizontal scale factor (1.0 = normal)
    float scale_y;              ///< Vertical scale factor (1.0 = normal)
    float pivot_x;              ///< Pivot point X (0.5 = center)
    float pivot_y;              ///< Pivot point Y (0.5 = center)
    int r;                      ///< Red color modulation (0-255)
    int g;                      ///< Green color modulation (0-255)
    int b;                      ///< Blue color modulation (0-255)
    int a;                      ///< Alpha transparency (0-255)
    int sorting_order;          ///< Z-depth sorting (lower = drawn first, behind higher)
    bool is_ui;                 ///< If true, ignores camera transform (screen-space rendering)
    size_t order_index;         ///< Submission order for stable sorting
};

/**
 * @struct PixelDrawRequest
 * @brief Encapsulates parameters for a deferred pixel draw call (debugging).
 *
 * Pixels are drawn last (on top of all images) and are useful for debugging,
 * visualizing positions, or simple particle effects.
 */
struct PixelDrawRequest {
    int x, y;                   ///< Screen-space pixel position
    int r, g, b, a;             ///< RGBA color (0-255)
    size_t order_index;         ///< Submission order for stable sorting
};

/**
 * @class ImageDB
 * @brief Deferred rendering system for sprites with texture caching and sorting.
 *
 * ImageDB manages texture loading, caching, and deferred rendering of sprites.
 * Components queue draw requests via QueueImageDraw*() methods, which are batched,
 * sorted by z-order, and rendered during Engine::Render().
 *
 * Features:
 * - Automatic texture loading and caching from resources/images/
 * - Deferred rendering: queue draws during Update, render in batch
 * - Z-order sorting via sorting_order parameter
 * - Full transform support: position, rotation, scale, pivot
 * - Color modulation and alpha blending
 * - UI rendering mode (ignores camera transform)
 * - Pixel drawing for debugging
 *
 * Rendering Pipeline:
 * 1. Components call QueueImageDraw*() during Update phase
 * 2. Draw requests accumulate in image_draw_request_queue
 * 3. Engine::Render() calls RenderAndClearAllImages()
 * 4. Requests are sorted by (sorting_order, order_index)
 * 5. Each request is rendered with SDL_RenderCopyEx()
 * 6. Queue is cleared for next frame
 *
 * @note ImageDB uses static methods for global access from Lua scripts
 */
class ImageDB {
public:
    /**
     * @brief Destructor (currently unused, reserved for cleanup).
     */
    ~ImageDB();

    /**
     * @brief Loads and caches a texture by name, or retrieves it from cache.
     *
     * @param imageName Image filename (without path, e.g., "player.png")
     * @return Pointer to SDL_Texture, or nullptr if loading fails
     *
     * @note Textures are loaded from resources/images/ and cached for reuse
     */
    static SDL_Texture* GetTexture(const std::string& imageName);

    /**
     * @brief Queues a simple sprite draw (world-space, no transforms).
     *
     * @param imageName Image filename
     * @param x World X position
     * @param y World Y position
     *
     * @note Uses default values: no rotation, scale 1:1, white color, sorting order 0
     */
    static void QueueImageDraw(const std::string& imageName, float x, float y);

    /**
     * @brief Queues a sprite draw with full transform and color control.
     *
     * @param imageName Image filename
     * @param x World X position
     * @param y World Y position
     * @param rotationDegrees Rotation in degrees (clockwise)
     * @param scaleX Horizontal scale (1.0 = normal)
     * @param scaleY Vertical scale (1.0 = normal)
     * @param pivotX Pivot point X (0.5 = center)
     * @param pivotY Pivot point Y (0.5 = center)
     * @param r Red color modulation (0-255)
     * @param g Green color modulation (0-255)
     * @param b Blue color modulation (0-255)
     * @param a Alpha transparency (0-255)
     * @param sortingOrder Z-depth (lower = behind, higher = in front)
     */
    static void QueueImageDrawEx(const std::string& imageName, float x, float y,
                          float rotationDegrees, float scaleX, float scaleY,
                          float pivotX, float pivotY,
                          float r, float g, float b, float a,
                          float sortingOrder);

    /**
     * @brief Queues a UI sprite draw (screen-space, ignores camera).
     *
     * @param imageName Image filename
     * @param x Screen X position (pixels from left)
     * @param y Screen Y position (pixels from top)
     *
     * @note UI draws ignore camera position and zoom
     */
    static void QueueImageDrawUI(const std::string& imageName, float x, float y);

    /**
     * @brief Queues a UI sprite draw with color and sorting control.
     *
     * @param imageName Image filename
     * @param x Screen X position
     * @param y Screen Y position
     * @param r Red color modulation (0-255)
     * @param g Green color modulation (0-255)
     * @param b Blue color modulation (0-255)
     * @param a Alpha transparency (0-255)
     * @param sortingOrder Z-depth for UI layering
     */
    static void QueueImageDrawUIEx(const std::string& imageName, float x, float y,
                            float r, float g, float b, float a,
                            float sortingOrder);

    /**
     * @brief Queues a single pixel draw (debugging/visualization).
     *
     * @param x Screen X position
     * @param y Screen Y position
     * @param r Red (0-255)
     * @param g Green (0-255)
     * @param b Blue (0-255)
     * @param a Alpha (0-255)
     *
     * @note Pixels are drawn last (on top of all images)
     */
    static void QueueDrawPixel(float x, float y, float r, float g, float b, float a);

    /**
     * @brief Renders all queued image draws, then clears the queue.
     *
     * Rendering process:
     * 1. Sort image_draw_request_queue by (sorting_order, order_index)
     * 2. For each request, apply camera transform (if not UI)
     * 3. Render with SDL_RenderCopyEx() (rotation, scale, color mod)
     * 4. Clear queue
     *
     * @note Called once per frame by Engine::Render()
     */
    static void RenderAndClearAllImages();

    /**
     * @brief Renders all queued pixel draws, then clears the queue.
     *
     * @note Pixels are drawn last (on top of everything) and ignore camera
     */
    static void RenderAndClearAllPixels();

    /**
     * @brief Clears all draw queues without rendering.
     *
     * @note Useful for scene transitions or debugging
     */
    static void ClearQueues();

    /**
     * @brief Creates a default 1x1 white texture for particles.
     *
     * @param name Texture name to register in cache
     *
     * @note Used for particle systems or procedural effects
     */
    void CreateDefaultParticleTextureWithName(const std::string& name);

private:
    /// Texture cache: image name -> SDL_Texture*
    inline static std::unordered_map<std::string, SDL_Texture*> textureMap;

    /// Deferred draw request queue for images
    inline static std::vector<ImageDrawRequest> image_draw_request_queue;

    /// Deferred draw request queue for pixels
    inline static std::vector<PixelDrawRequest> pixel_draw_request_queue;

    /// Submission order counter for stable sorting
    inline static size_t request_counter = 0;

    /// Pixel submission order counter
    inline static size_t pixel_request_counter = 0;
};

#endif /* ImageDB_hpp */
