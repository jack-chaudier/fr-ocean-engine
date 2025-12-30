# FR-Ocean Engine

A professional 2D game engine built with C++17 and Lua scripting.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Lua 5.4](https://img.shields.io/badge/Lua-5.4-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Version](https://img.shields.io/badge/Version-1.0.0-orange.svg)

## Overview

FR-Ocean Engine is a professional-grade 2D game engine designed for rapid game development. With a component-based architecture, Lua scripting, and Box2D physics, it provides everything you need to create sophisticated 2D games.

Built with modern C++17 and industry-standard libraries, FR-Ocean Engine combines performance with ease of use, making it ideal for both indie developers and students learning game development.

### Key Features

- **Component-Based**: Flexible actor-component architecture inspired by Unity
- **Lua Scripting**: Write game logic in Lua for rapid iteration
- **Full Physics**: Box2D integration with collision detection and raycasting
- **Deferred Rendering**: Optimized sprite rendering with z-order sorting
- **Cross-Platform**: Write once, build for Windows, macOS, and Linux
- **Well-Documented**: Comprehensive API documentation and examples
- **MIT Licensed**: Free for commercial and personal projects

## Features

### Core Engine
- Component-based architecture with reusable, composable components
- JSON-based scene definitions with hot-swapping support
- Dynamic game objects with runtime component addition/removal
- Modern C++17 with unified codebase for all platforms

### Rendering
- Hardware-accelerated 2D sprite rendering via SDL2
- Deferred rendering pipeline with batched, sorted draw calls
- Camera system with position and zoom control
- Z-order sorting for sprite layering
- Transform support (position, rotation, scale, pivot)
- RGBA color modulation and alpha blending
- TrueType font rendering with SDL2_ttf
- Screen-space UI rendering

### Physics
- Box2D integration for industry-standard 2D physics
- Dynamic, kinematic, and static body types
- Box and circle collision shapes
- Trigger volumes for sensor-based overlap detection
- Collision callbacks (OnCollisionEnter/Stay/Exit)
- Single and multi-hit raycasting

### Audio
- Multi-channel mixing with 16 simultaneous channels via SDL2_mixer
- WAV, OGG, and MP3 format support
- Seamless audio looping for music
- Per-channel volume control

### Scripting
- Lua 5.4 with LuaBridge C++ integration
- Component lifecycle callbacks (OnStart, OnUpdate, OnLateUpdate, OnDestroy)
- Full API access to engine systems from Lua
- Component inheritance via Lua metatables

### Developer Tools
- Data-driven design with JSON configuration
- Modern CMake build system
- Vendored dependencies (no external package managers required)
- Built-in logging system with multiple severity levels
- Debug pixel and rectangle drawing for visualization
- Runtime resources path override via command-line

## Requirements

### Windows
- Visual Studio 2019 or newer
- CMake 3.16+

### macOS
- Xcode 12+ or Command Line Tools
- CMake 3.16+

### Linux
- GCC 9+ or Clang 9+
- CMake 3.16+
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf development packages

## Installation

### Building from Source

**Windows:**
```bash
git clone https://github.com/jackgaff/fr-ocean-engine.git
cd fr-ocean-engine
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

**macOS:**
```bash
git clone https://github.com/jackgaff/fr-ocean-engine.git
cd fr-ocean-engine
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

For Xcode project generation:
```bash
cmake -G Xcode ..
```

**Linux:**
```bash
git clone https://github.com/jackgaff/fr-ocean-engine.git
cd fr-ocean-engine
mkdir build && cd build
cmake ..
make
```

The executable outputs to `build/bin/game_engine` with resources copied to `build/bin/resources/`.

## Command-Line Options

```
./game_engine [options]

Options:
  --resources <path>  Override resources directory (default: resources/)
  --debug             Enable debug logging
  --version           Print version and exit
  --help              Print help message
```

## Usage

### Creating a Lua Component

```lua
-- ConstantMovement.lua
ConstantMovement = {
    x_vel = 0,
    y_vel = 0,
}

function ConstantMovement:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
end

function ConstantMovement:OnUpdate()
    local pos = self.rigidbody:GetPosition()
    pos.x = pos.x + self.x_vel
    pos.y = pos.y + self.y_vel
    self.rigidbody:SetPosition(pos)
end
```

### Project Structure

```
fr-ocean-engine/
├── build/                  # Build directory (generated)
├── game_engine/            # Engine source code
│   ├── Actor.cpp/hpp       # Actor system
│   ├── Engine.cpp/hpp      # Main game loop
│   ├── ComponentDB.cpp/hpp # Lua component system
│   └── ...                 # Other engine sources
├── resources/              # Game resources
│   ├── game.config         # Game configuration
│   ├── rendering.config    # Rendering settings
│   ├── scenes/             # Scene definition files
│   ├── actor_templates/    # Actor templates
│   ├── component_types/    # Lua component scripts
│   ├── images/             # Image files
│   ├── fonts/              # Font files
│   └── audio/              # Audio files
├── resources.example/      # Example Space Shooter game
├── vendor/                 # Third-party libraries
└── CMakeLists.txt          # CMake build configuration
```

## Documentation

- **[API Reference](./API_REFERENCE.md)**: Complete Lua API documentation with examples
- **[Architecture](./ARCHITECTURE.md)**: Engine design, subsystems, and data flow
- **[Code Standards](./CODE_STANDARDS.md)**: Coding conventions and best practices
- **[Contributing Guide](./CONTRIBUTING.md)**: How to contribute to the project
- **[Changelog](./CHANGELOG.md)**: Version history and release notes

## Example Games

The engine includes three example games to demonstrate different capabilities:

### Space Shooter (`resources.example/`)
- Player movement and shooting
- Enemy spawning and AI
- Collision detection with triggers
- Score tracking and game over state

### Platformer (`resources.platformer/`)
- Physics-based player movement with jumping
- Ground detection via raycasting
- Collectible coins with trigger volumes
- Smooth camera following
- Multi-platform level design

```bash
./build/bin/game_engine --resources resources.platformer/
```

### Puzzle Match (`resources.puzzle/`)
- Match-3 puzzle gameplay
- Grid-based piece matching
- Score tracking with progress bar
- Move counter and win/lose conditions
- Mouse-based selection and swapping

```bash
./build/bin/game_engine --resources resources.puzzle/
```

## Troubleshooting

### "error: resources/ missing"
Ensure you have a `resources/` directory containing `game.config` and `rendering.config`.

### "error: font X missing"
Font files must be placed in `resources/fonts/` with `.ttf` extension.

### SDL initialization failed
- **Windows**: Ensure SDL2.dll is in the same directory as the executable
- **macOS**: SDL2 frameworks should be in `build/bin/Frameworks/`
- **Linux**: Install SDL2 development packages: `sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev`

### Lua component not loading
- Check the component file exists in `resources/component_types/`
- Verify the filename matches the type name (case-sensitive)
- Check for Lua syntax errors in console output

## Performance

Typical performance on modern hardware (Release build, 1080p):
- 1000+ actors with components: 60 FPS
- 10000+ sprites per frame: 60 FPS
- 100 physics bodies: 60 FPS with Box2D simulation

## Roadmap

See [CHANGELOG.md](./CHANGELOG.md) for planned features:

- **v1.0** [RELEASED]: Core engine, physics, scripting, rendering
- **v1.1** [Planned]: Particle system, tilemaps, animations
- **v1.2** [Planned]: Level editor, prefabs, asset pipeline
- **v2.0** [Future]: ECS architecture, multi-threading

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

You are free to use FR-Ocean Engine for commercial projects, personal projects, educational purposes, and modification/distribution.

## Contributing

Contributions are welcome. Please see [CONTRIBUTING.md](./CONTRIBUTING.md) for guidelines.

Ways to contribute:
- Report bugs and issues
- Suggest new features
- Improve documentation
- Submit bug fixes
- Add new features
- Create example games

Look for the `good-first-issue` label on GitHub for beginner-friendly tasks.

## Credits

This engine utilizes the following open-source libraries:

- [SDL2](https://www.libsdl.org/) - Simple DirectMedia Layer
- [Box2D](https://box2d.org/) - 2D physics engine
- [Lua](https://www.lua.org/) - Scripting language
- [LuaBridge](https://github.com/vinniefalco/LuaBridge) - C++ to Lua binding
- [GLM](https://github.com/g-truc/glm) - Mathematics library
- [RapidJSON](https://rapidjson.org/) - JSON parser/generator

## Community

- **GitHub Issues**: [Report bugs or request features](https://github.com/jackgaff/fr-ocean-engine/issues)
- **Discussions**: [Ask questions and share ideas](https://github.com/jackgaff/fr-ocean-engine/discussions)
- **Email**: jackgaff@umich.edu

---

FR-Ocean Engine - Built by Jack Gaffney

[Star on GitHub](https://github.com/jackgaff/fr-ocean-engine) |
[Fork](https://github.com/jackgaff/fr-ocean-engine/fork) |
[Contribute](./CONTRIBUTING.md)
