//
//  TextDB.hpp
//  FR-Ocean Engine
//
//  Text rendering system with TrueType font support, dynamic sizing, and
//  deferred rendering pipeline.
//
//  Created by Jack Gaffney on 2/9/25.
//

#ifndef TextDB_hpp
#define TextDB_hpp

#include <string>
#include <vector>
#include <queue>
#include <filesystem>
#include <unordered_map>
#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "Renderer.hpp"
#include "Helper.h"

/**
 * @struct TextDrawRequest
 * @brief Encapsulates parameters for a deferred text draw call.
 *
 * Text draw requests are queued during Update and rendered in batch during Render.
 */
struct TextDrawRequest {
    std::string content;    ///< Text string to render
    int x, y;               ///< Screen position (pixels)
    std::string fontName;   ///< Font file name (e.g., "Arial.ttf")
    int fontSize;           ///< Font size in points
    SDL_Color color;        ///< RGBA text color (0-255)
};

/**
 * @class TextDB
 * @brief Text rendering system with TrueType font support and caching.
 *
 * TextDB manages font loading, caching at multiple sizes, and deferred text rendering.
 * It uses SDL2_ttf for TrueType font rendering with anti-aliasing.
 *
 * Features:
 * - TrueType font loading from resources/fonts/
 * - Font caching at multiple sizes (each size cached separately)
 * - Deferred rendering: queue text during Update, render in batch
 * - Full RGBA color control
 * - UTF-8 text support via SDL_ttf
 *
 * Rendering Pipeline:
 * 1. Components call QueueTextDraw() during Update phase
 * 2. Draw requests accumulate in drawRequests queue
 * 3. Engine::Render() calls RenderQueuedTexts()
 * 4. Each request is rendered as a texture (SDL_RenderCopy)
 * 5. Textures are destroyed and queue is cleared
 *
 * @note TextDB uses static methods for global access from Lua scripts
 */
class TextDB {
public:
    /**
     * @brief Initializes SDL2_ttf library.
     *
     * @throws std::runtime_error if SDL_ttf initialization fails
     */
    static void Init();

    /**
     * @brief Shuts down SDL2_ttf and frees all cached fonts.
     *
     * @note Called automatically by Engine destructor
     */
    static void Shutdown();

    /**
     * @brief Loads a font file and prepares it for use.
     *
     * @param fontName Font filename (e.g., "Arial.ttf", without path)
     *
     * @note Fonts are loaded from resources/fonts/ directory
     */
    static void LoadFont(const std::string& fontName);

    /**
     * @brief Gets or loads a font at a specific size.
     *
     * @param fontName Font filename
     * @param fontSize Font size in points
     * @return Pointer to TTF_Font, or nullptr if loading fails
     *
     * @note Each font size is cached separately for performance
     */
    static TTF_Font* GetFont(const std::string& fontName, int fontSize);

    /**
     * @brief Queues a text draw request for deferred rendering.
     *
     * @param content Text string to render (UTF-8 supported)
     * @param x Screen X position (pixels from left)
     * @param y Screen Y position (pixels from top)
     * @param fontName Font filename
     * @param fontSize Font size in points
     * @param r Red color component (0-255)
     * @param g Green color component (0-255)
     * @param b Blue color component (0-255)
     * @param a Alpha transparency (0-255)
     *
     * @note Text is rendered to screen space (ignores camera transform)
     */
    static void QueueTextDraw(const std::string& content, float x, float y,
                      const std::string& fontName, float fontSize,
                      float r, float g, float b, float a);

    /**
     * @brief Renders all queued text draws, then clears the queue.
     *
     * Rendering process:
     * 1. For each TextDrawRequest in queue:
     *    - Render text to SDL_Texture using TTF_RenderText_Blended()
     *    - Draw texture to screen at specified position
     *    - Destroy texture
     * 2. Clear queue
     *
     * @note Called once per frame by Engine::Render()
     */
    static void RenderQueuedTexts();

private:
    /// Font cache: font name -> font size -> TTF_Font*
    inline static std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

    /// Temporary texture storage for rendered text glyphs
    inline static std::vector<SDL_Texture*> texts;

    /// Base path for font files
    inline static std::string fontPath = "resources/fonts/";

    /// Deferred text draw request queue
    inline static std::queue<TextDrawRequest> drawRequests;

    /// Default text color (white, fully opaque)
    inline static SDL_Color textColor = {255, 255, 255, 255};

    /// Initialization flag (prevents double-init)
    inline static bool initialized = false;
};

#endif /* TextDB_hpp */
