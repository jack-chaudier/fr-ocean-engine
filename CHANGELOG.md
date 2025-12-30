# Changelog

All notable changes to FR-Ocean Engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Particle system with GPU acceleration
- Tilemap rendering and collision
- Sprite animation system
- Level editor
- Unit test framework
- Profiling tools
- Documentation website

## [1.0.1] - 2025-12-29

### Added
- `Image.DrawRect()` function for filled rectangle rendering
- `Rigidbody.RecreateFixtures()` for runtime fixture updates
- Two new example games: Platformer and Puzzle Match

### Fixed
- **Resources path override**: `--resources` flag now properly propagates to all subsystems
- **Scene component overrides**: Position overrides now correctly update Box2D body positions
- **Fixture dimension overrides**: Width/height overrides now recreate Box2D fixtures
- **Trigger collision detection**: Changed filter to allow trigger-collider interactions (mask=0xFFFF)
- **Collision callbacks**: Now properly skip C++ userdata components (Rigidbody)
- **Actor destruction**: Deferred to after physics step to prevent Box2D crashes
- **Component enabled checks**: Fixed crash when accessing `enabled` on Rigidbody userdata

### Changed
- Collision filter masks updated to 0xFFFF for universal detection
- Actor destruction now defers component cleanup to `ActorsPendingDestruction()`

## [1.0.0] - 2025-10-28

### Initial Release

#### Added

**Core Engine**
- Main game loop with fixed timestep support
- Component-based actor system
- Scene management with JSON-based scene files
- Cross-platform support (Windows, macOS, Linux)

**Rendering**
- SDL2-based 2D rendering pipeline
- Deferred rendering with z-order sorting
- Sprite rendering with transforms (position, rotation, scale, pivot)
- Color modulation and alpha blending
- Camera system (position, zoom)
- World-space and screen-space (UI) rendering modes
- Pixel drawing for debugging
- TrueType font rendering with SDL2_ttf

**Input**
- Keyboard input with frame-accurate state tracking
- Mouse input (position, buttons, scroll wheel)
- Input states: down, just pressed, just released
- String-based key names for easy scripting

**Physics**
- Box2D integration for 2D rigid body dynamics
- Three body types: dynamic, kinematic, static
- Collision shapes: box, circle
- Trigger volumes (sensors) for overlap detection
- Physics properties: friction, bounciness, density, gravity scale
- Collision callbacks: OnCollisionEnter/Stay/Exit
- Trigger callbacks: OnTriggerEnter/Stay/Exit
- Raycasting (single and multi-hit)

**Audio**
- SDL2_mixer integration for sound and music
- Multi-channel audio mixing (16 channels)
- Looping audio support
- Per-channel volume control
- Support for WAV, OGG, MP3 formats

**Scripting**
- Lua 5.4 scripting for game logic
- LuaBridge for C++-Lua bindings
- Component lifecycle: OnStart, OnUpdate, OnLateUpdate, OnDestroy
- Full C++ API exposed to Lua:
  - Actor manipulation
  - Input queries
  - Rendering (sprites, text, UI)
  - Physics control
  - Audio playback
  - Scene management
  - Camera control
  - Debug logging

**Build System**
- CMake 3.16+ build configuration
- Cross-platform build support
- Vendored dependencies (no external package manager required)
- Visual Studio project files (Windows)
- Xcode project files (macOS)
- Makefile (Linux/macOS alternative)

**Documentation**
- Comprehensive README with quickstart guide
- API Reference with complete Lua API documentation
- Architecture documentation explaining engine design
- Contributing guidelines
- Code standards and conventions
- Example component and scene files

#### Technical Details

**Language and Standards**
- C++17 standard
- Modern C++ features (smart pointers, range-based loops, etc.)
- RAII for resource management

**Dependencies**
- SDL2 (rendering, window management)
- SDL2_image (texture loading)
- SDL2_mixer (audio playback)
- SDL2_ttf (font rendering)
- Box2D (physics simulation)
- Lua 5.4 (scripting)
- LuaBridge (Lua-C++ binding)
- GLM (math library)
- RapidJSON (JSON parsing)

**Performance**
- Deferred rendering with batched draw calls
- Texture caching for optimal reuse
- Component function caching to avoid string lookups
- Vector reserve for known sizes
- Stable sorting for consistent z-ordering

**File Formats**
- JSON for configuration (game.config, rendering.config)
- JSON for scenes (.scene files)
- JSON for actor templates
- Lua for component types
- Standard image formats (PNG, JPG, BMP)
- Standard audio formats (WAV, OGG, MP3)
- TrueType fonts (TTF)

### Known Issues

- No particle system yet
- No tilemap support
- No sprite animation system
- Limited collision filtering options
- No visual level editor
- No networking support

### Platform Support

**Windows**
- Windows 10/11
- Visual Studio 2019+
- MSVC compiler

**macOS**
- macOS 12+
- Xcode 12+
- Clang compiler

**Linux**
- Ubuntu 20.04+, Fedora 35+, Arch Linux
- GCC 9+, Clang 9+
- X11 or Wayland

---

## Version History

### Version Numbering

FR-Ocean Engine follows [Semantic Versioning](https://semver.org/):

- **MAJOR**: Incompatible API changes
- **MINOR**: New features (backwards compatible)
- **PATCH**: Bug fixes (backwards compatible)

### Release Process

1. Update CHANGELOG.md with release notes
2. Update version in CMakeLists.txt
3. Tag release in Git: `git tag v1.0.0`
4. Build release binaries for all platforms
5. Create GitHub release with binaries and notes

---

## [Unreleased] - Future Roadmap

### Version 1.1.0 (Planned Q1 2026)

**Planned Features**:
- Particle system with configurable emitters
- Tilemap rendering and collision detection
- Sprite animation with keyframes and state machines
- Improved documentation and tutorials
- Performance profiling tools

### Version 1.2.0 (Planned Q2 2026)

**Planned Features**:
- Visual level editor (standalone tool)
- Scene hierarchies and prefab system
- Asset pipeline with hot-reloading
- Advanced collision filtering (layers, masks)
- Localization support

### Version 2.0.0 (Future)

**Major Changes** (Breaking API):
- Consider Entity-Component-System (ECS) architecture
- Multi-threading support (separate render thread)
- Vulkan/Metal rendering backend option
- LuaJIT for improved scripting performance
- Built-in networking for multiplayer

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to this changelog and the project.

---

**Maintainer**: Jack Gaffney
**Repository**: https://github.com/jackgaff/fr-ocean-engine
**License**: MIT
