//
//  Engine.hpp
//  FR-Ocean Engine
//
//  Main game engine orchestrator responsible for initialization, game loop management,
//  and coordinating all engine subsystems (rendering, input, physics, audio, scripting).
//
//  Created by Jack Gaffney on 1/22/25.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <string>
#include <optional>
#include "Renderer.hpp"
#include "EngineUtils.h"
#include "SceneDB.hpp"
#include "Actor.hpp"
#include "rapidjson/document.h"
#include "Helper.h"
#include "ConfigManager.hpp"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ApplicationAPI.h"

/**
 * @class Engine
 * @brief Core game engine class that manages the main game loop and all subsystems.
 *
 * The Engine class serves as the central orchestrator for the FR-Ocean Engine.
 * It initializes all subsystems (rendering, input, audio, physics, scripting) and
 * manages the main game loop including update and render cycles.
 *
 * Architecture:
 * - Singleton pattern for global access to engine state
 * - Fixed timestep physics with variable rendering
 * - Deferred rendering system with batched draw calls
 * - Lua scripting integration via LuaBridge
 *
 * Lifecycle:
 * 1. Constructor: Initialize all subsystems (TextDB, AudioDB, Input, ComponentDB, Renderer)
 * 2. GameLoop(): Main loop handling events, updates, and rendering
 * 3. Update(): Process game logic and component updates via SceneDB
 * 4. Render(): Execute deferred rendering for images, text, and UI
 * 5. Destructor: Cleanup resources and shutdown subsystems
 *
 * @note The Engine uses static methods for the game loop to allow global access
 *       from Lua scripts and other engine components.
 */
class Engine {
public:
    /**
     * @brief Constructs the Engine and initializes all subsystems.
     *
     * Initialization order:
     * 1. Parse game.config and rendering.config (JSON)
     * 2. Initialize TextDB (SDL_ttf)
     * 3. Initialize AudioDB (SDL_mixer)
     * 4. Initialize Input system
     * 5. Initialize ComponentDB (Lua state and API bindings)
     * 6. Create Renderer with window and SDL2 context
     * 7. Load initial scene from configuration
     *
     * @throws std::runtime_error if any subsystem fails to initialize
     */
    Engine();

    /**
     * @brief Destructs the Engine and cleans up all resources.
     *
     * Cleanup order:
     * 1. Shutdown Renderer (SDL window/renderer)
     * 2. Close Lua state (ComponentDB)
     * 3. Cleanup TextDB (fonts and textures)
     * 4. Cleanup AudioDB (sound effects and music)
     */
    ~Engine();


private:
    /// Scene database managing all actors and game state
    inline static SceneDB scene;

    /// Background clear color (RGB) for rendering
    inline static glm::ivec3 cleanColor;

public:
    /**
     * @brief Main game loop that runs until application quit is requested.
     *
     * Loop structure (every frame):
     * 1. Input::BeginFrame() - Reset input states
     * 2. SDL_PollEvent() loop - Process OS events (keyboard, mouse, window)
     * 3. Update() - Execute game logic and component updates
     * 4. Render() - Draw all queued images, text, and UI elements
     * 5. Helper::SDL_RenderPresent_EX() - Swap buffers and present frame
     *
     * @note The loop uses SDL event polling for platform-agnostic event handling
     * @see Update() for game logic processing
     * @see Render() for rendering pipeline details
     */
    static void GameLoop();

    /**
     * @brief Processes game logic updates for the current frame.
     *
     * Update pipeline:
     * 1. SceneDB::UpdateScene() - Execute component lifecycle methods:
     *    - ProcessOnStart() for newly created components
     *    - ProcessUpdate() for OnUpdate() callbacks
     *    - ProcessLateUpdate() for OnLateUpdate() callbacks (post-physics)
     * 2. Handle actor destruction and scene transitions
     * 3. Step physics simulation (Box2D world)
     *
     * @note Update runs at a variable framerate; physics uses fixed timestep internally
     */
    static void Update();

    /**
     * @brief Executes the rendering pipeline for the current frame.
     *
     * Render pipeline:
     * 1. Renderer::clear() - Clear screen with background color
     * 2. ImageDB::RenderAndClearAllImages() - Draw all queued sprites (sorted by z-order)
     * 3. TextDB::RenderQueuedTexts() - Draw all queued text elements
     * 4. ImageDB::RenderAndClearAllPixels() - Draw debug pixels (last, on top)
     * 5. Clear all draw queues for next frame
     *
     * @note All rendering is deferred - components queue draw requests during Update(),
     *       which are batched and sorted before actual rendering
     */
    static void Render();

};

#endif /* Engine_hpp */
