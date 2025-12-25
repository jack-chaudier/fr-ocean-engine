# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Build (from project root)
mkdir build && cd build
cmake ..
cmake --build . --config Release

# macOS with Xcode
cmake -G Xcode ..

# Windows with Visual Studio
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

The executable outputs to `build/bin/game_engine` with resources copied to `build/bin/resources/`.

## Command-Line Options

```bash
./game_engine --resources path/to/resources/  # Override resources directory
./game_engine --debug                          # Enable debug logging
./game_engine --version                        # Print version
./game_engine --help                           # Print help
```

## Architecture Overview

FR-Ocean Engine is a component-based 2D game engine with C++17 core systems and Lua scripting for game logic.

### Core Systems (Static Singletons)

- **Engine** (`Engine.cpp`) - Main game loop orchestrator
- **SceneDB** (`SceneDB.cpp`) - Actor lifecycle and scene management
- **ComponentDB** (`ComponentDB.cpp`) - Lua state, component type loading, C++ API bindings via LuaBridge
- **ImageDB** (`ImageDB.cpp`) - Deferred sprite rendering with z-order sorting
- **TextDB** (`TextDB.cpp`) - TrueType font rendering
- **AudioDB** (`AudioDB.cpp`) - SDL2_mixer audio playback
- **Input** (`Input.cpp`) - Keyboard/mouse state management
- **RigidbodyWorld** (`RigidbodyWorld.cpp`) - Box2D physics world simulation
- **Logger** (`Logger.cpp`) - Thread-safe logging with multiple severity levels
- **ConfigManager** (`ConfigManager.cpp`) - Game and rendering configuration

### Error Handling

The engine uses an exception hierarchy defined in `EngineException.hpp`:
- `EngineException` - Base exception class
- `ConfigurationException` - Config file errors
- `ResourceNotFoundException` - Missing assets (images, fonts, audio, scenes)
- `ScriptException` - Lua script errors
- `RenderException` - SDL/rendering errors
- `AudioException` - Audio system errors

Use `LOG_INFO`, `LOG_WARNING`, `LOG_ERROR`, `LOG_FATAL` macros for logging.

### Frame Update Flow

```
Input::BeginFrame() -> SDL Event Loop -> SceneDB::UpdateScene() -> Physics Step -> Engine::Render()
```

SceneDB update phases: `ProcessOnStart()` -> `ProcessUpdate()` -> physics -> `ProcessLateUpdate()` -> destroy pending actors

### Component System

Lua components in `resources/component_types/` define game behavior. ComponentDB loads these as prototypes and instantiates via metatable inheritance. Components have lifecycle methods: `OnStart`, `OnUpdate`, `OnLateUpdate`, `OnDestroy`, plus collision callbacks.

### Rendering Pipeline

Deferred rendering: components queue draw requests during update, ImageDB sorts by `sorting_order` and renders in batch. World-space draws respect camera transform; UI draws (`DrawUI`) ignore camera.

### Data Files

- `resources/game.config` - Game configuration (initial scene, window title)
- `resources/rendering.config` - Window size, clear color
- `resources/scenes/*.scene` - JSON scene definitions
- `resources/actor_templates/*.template` - Actor component configurations
- `resources/component_types/*.lua` - Lua component scripts

## Code Standards

- **C++17** with modern features (smart pointers, optional, structured bindings)
- **PascalCase** for classes, public methods, Lua component tables
- **snake_case** for variables, private helpers
- **4 spaces** indentation, K&R braces
- Attach `*` and `&` to type: `SDL_Texture* texture`
- Use `std::unique_ptr`/`std::shared_ptr` over raw `new`/`delete`
- Use exception hierarchy for fatal errors, log errors for recoverable issues

## Dependencies (Vendored)

All dependencies in `vendor/`: SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, Box2D, Lua 5.4, LuaBridge, GLM, RapidJSON

## Example Game

See `resources.example/` for a complete Space Shooter game demonstrating:
- Player controller with movement and shooting
- Enemy spawning and AI
- Collision detection with triggers
- Score tracking and game state management
