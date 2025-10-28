# FR-Ocean Engine Architecture

## Table of Contents
- [Overview](#overview)
- [Core Architecture](#core-architecture)
- [Subsystem Details](#subsystem-details)
- [Data Flow](#data-flow)
- [Component Lifecycle](#component-lifecycle)
- [Rendering Pipeline](#rendering-pipeline)
- [Physics System](#physics-system)
- [Scripting System](#scripting-system)
- [Design Patterns](#design-patterns)

## Overview

FR-Ocean Engine is a component-based 2D game engine built with C++17. It features a hybrid architecture where the core engine systems are written in C++ for performance, while game logic is scripted in Lua for flexibility and rapid iteration.

### Key Design Principles

1. **Component-Based Architecture**: Actors are containers for components that define behavior
2. **Separation of Concerns**: Clear boundaries between C++ engine systems and Lua game logic
3. **Deferred Rendering**: Draw calls are queued and batched for optimal performance
4. **Static Singleton Pattern**: Global access to engine systems via static methods
5. **Data-Driven Design**: Configuration and scenes defined in JSON

### Technology Stack

- **Language**: C++17
- **Graphics**: SDL2, SDL2_image
- **Audio**: SDL2_mixer
- **Text**: SDL2_ttf (TrueType fonts)
- **Physics**: Box2D (2D rigid body dynamics)
- **Scripting**: Lua 5.4 with LuaBridge
- **Math**: GLM (OpenGL Mathematics)
- **JSON**: RapidJSON
- **Build System**: CMake 3.16+

## Core Architecture

### System Overview

```
┌──────────────────────────────────────────────────────────────┐
│                         Engine                               │
│  ┌────────────┐  ┌────────────┐  ┌──────────────────────┐   │
│  │  GameLoop  │─▶│   Update   │─▶│       Render         │   │
│  └────────────┘  └────────────┘  └──────────────────────┘   │
└──────────────────────────────────────────────────────────────┘
          │                │                    │
          ▼                ▼                    ▼
    ┌─────────┐      ┌──────────┐        ┌──────────┐
    │  Input  │      │ SceneDB  │        │ Renderer │
    └─────────┘      └──────────┘        └──────────┘
                          │                    │
                          ▼                    ▼
                  ┌───────────────┐     ┌────────────┐
                  │  ComponentDB  │     │  ImageDB   │
                  │      +        │     │  TextDB    │
                  │  Actor System │     └────────────┘
                  └───────────────┘
                          │
                ┌─────────┴─────────┐
                ▼                   ▼
        ┌──────────────┐    ┌──────────────┐
        │ RigidbodyWorld│    │   AudioDB    │
        └──────────────┘    └──────────────┘
```

### Main Components

| Component | Responsibility | Lifecycle |
|-----------|---------------|-----------|
| **Engine** | Main game loop orchestration | Singleton, lives for entire application |
| **Renderer** | SDL2 window and rendering context | Created by Engine, destroyed on exit |
| **Input** | Keyboard, mouse, and event processing | Static singleton, initialized once |
| **SceneDB** | Actor and scene management | Instance per scene, manages actor lifecycle |
| **ComponentDB** | Lua state and component type management | Static singleton, one Lua state globally |
| **ImageDB** | Sprite rendering and texture caching | Static singleton, deferred rendering |
| **TextDB** | Font rendering and text drawing | Static singleton, deferred rendering |
| **AudioDB** | Sound effect and music playback | Static singleton, channel-based mixing |
| **RigidbodyWorld** | Box2D physics world simulation | One per scene, stepped every frame |

## Subsystem Details

### Engine (Main Loop)

The Engine class is the central orchestrator that manages initialization and the main game loop.

**Initialization Order**:
1. Parse `game.config` and `rendering.config` (JSON)
2. Initialize TextDB (SDL_ttf)
3. Initialize AudioDB (SDL_mixer)
4. Initialize Input system
5. Initialize ComponentDB (Lua state + API bindings)
6. Create Renderer (SDL2 window/renderer)
7. Load initial scene from configuration

**Game Loop**:
```cpp
while (!quit) {
    Input::BeginFrame();

    // Process OS events
    while (SDL_PollEvent(&event)) {
        Input::ProcessEvent(event);
        if (event.type == SDL_QUIT) quit = true;
    }

    Engine::Update();  // Game logic + physics
    Engine::Render();  // Deferred rendering

    Helper::SDL_RenderPresent_EX();  // Swap buffers
}
```

### Scene Management

**SceneDB** manages all actors in the current scene and handles scene transitions.

**Scene Loading**:
1. Parse `.scene` JSON file from `resources/scenes/`
2. For each actor definition:
   - Create Actor instance with unique ID
   - Load actor template (JSON with component definitions)
   - Instantiate components via ComponentDB
   - Add actor to scene

**Actor Lifecycle**:
- **Creation**: `SceneDB::InstantiateActor(template_name)`
- **Update Phases**:
  - `ProcessOnStart()`: Call OnStart() for new components (once)
  - `ProcessUpdate()`: Call OnUpdate() for all components (every frame)
  - `ProcessLateUpdate()`: Call OnLateUpdate() after physics (every frame)
- **Destruction**: `SceneDB::DestroyActor(actor)` (deferred until end of frame)

**Scene Persistence**:
- By default, all actors are destroyed when changing scenes
- `SceneDB::DontDestroy(actor)` marks an actor to persist across scenes

### Component System

**ComponentDB** bridges C++ engine systems with Lua game logic.

**Component Type Loading**:
```lua
-- Example: PlayerController.lua in resources/component_types/
PlayerController = {
    speed = 5.0,
    jump_force = 10.0,
}

function PlayerController:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
end

function PlayerController:OnUpdate()
    if Input.GetKey("space") then
        self.rb:AddForce({x = 0, y = -self.jump_force})
    end
end
```

**Component Instantiation**:
1. ComponentDB loads all `.lua` files from `resources/component_types/`
2. Each file creates a Lua table (prototype) cached in `componentTypeCache`
3. CreateComponent() creates a new instance table with prototype inheritance
4. Lua metatable `__index` implements prototype chain

**C++ API Bindings** (via LuaBridge):
- Actor manipulation
- Input queries
- Image/Text/Audio rendering
- Physics control
- Scene management
- Camera control
- Debug logging

### Rendering System

**ImageDB** implements deferred rendering for sprites:

**Render Pipeline**:
```
Update Phase:
  └─▶ Components call Image.Draw() / Image.DrawUI()
      └─▶ ImageDB::QueueImageDraw() / QueueImageDrawUI()
          └─▶ ImageDrawRequest added to queue

Render Phase:
  └─▶ ImageDB::RenderAndClearAllImages()
      ├─▶ Sort queue by (sorting_order, order_index)
      ├─▶ For each request:
      │   ├─▶ Apply camera transform (if not UI)
      │   ├─▶ SDL_RenderCopyEx() with rotation/scale/color
      │   └─▶ Render to screen
      └─▶ Clear queue
```

**Features**:
- Texture caching (load once, reuse)
- Z-order sorting (sorting_order parameter)
- Camera system (world-space vs UI-space)
- Transform support (position, rotation, scale, pivot)
- Color modulation (tint, alpha)

### Physics System

**Box2D Integration** via Rigidbody component:

**World Setup**:
```cpp
RigidbodyWorld world(gravity_x, gravity_y);
world.Step(dt);  // Called every frame in Engine::Update()
```

**Body Types**:
- **Dynamic**: Affected by forces, has mass (players, projectiles)
- **Kinematic**: Moves via velocity, ignores forces (moving platforms)
- **Static**: Immovable, infinite mass (walls, floors)

**Collision Detection**:
- **Collider**: Physical collision shape (box or circle)
- **Trigger**: Sensor shape (overlap detection without collision response)
- **Callbacks**: OnCollisionEnter/Stay/Exit, OnTriggerEnter/Stay/Exit

**Raycasting**:
```lua
-- Single raycast
hit = Physics.Raycast(origin, direction, distance)
if hit.actor ~= nil then
    print("Hit: " .. hit.actor:GetName())
end

-- Multi-raycast (all hits)
hits = Physics.RaycastAll(origin, direction, distance)
for i, hit in ipairs(hits) do
    print("Hit " .. i .. ": " .. hit.actor:GetName())
end
```

## Data Flow

### Frame Update Flow

```
BeginFrame()
    └─▶ Input::BeginFrame()
        └─▶ Clear transient input state

SDL Event Loop
    └─▶ Input::ProcessEvent()
        └─▶ Update keyboard/mouse state

Engine::Update()
    └─▶ SceneDB::UpdateScene()
        ├─▶ ProcessOnStart()
        │   └─▶ Call OnStart() for new components
        ├─▶ ProcessUpdate()
        │   └─▶ Call OnUpdate() for all components
        │       └─▶ Components queue draw/audio requests
        ├─▶ RigidbodyWorld::Step()
        │   └─▶ Box2D physics simulation
        │   └─▶ Collision callbacks
        ├─▶ ProcessLateUpdate()
        │   └─▶ Call OnLateUpdate() for all components
        └─▶ ActorsPendingDestruction()
            └─▶ Destroy marked actors

Engine::Render()
    ├─▶ Renderer::clear()
    ├─▶ ImageDB::RenderAndClearAllImages()
    │   └─▶ Draw all sprites (sorted by z-order)
    ├─▶ TextDB::RenderQueuedTexts()
    │   └─▶ Draw all text
    └─▶ ImageDB::RenderAndClearAllPixels()
        └─▶ Draw debug pixels

Renderer::present()
    └─▶ SDL_RenderPresent() - Swap buffers

Input::LateUpdate()
    └─▶ Transition input states (JUST_DOWN → DOWN, etc.)
```

## Component Lifecycle

### Lifecycle Methods (Lua)

```lua
Component = {}

-- Called once on the first frame after creation
function Component:OnStart()
    -- Initialization code
end

-- Called every frame during update phase
function Component:OnUpdate()
    -- Game logic
end

-- Called every frame after physics simulation
function Component:OnLateUpdate()
    -- Post-physics logic (e.g., camera follow)
end

-- Called when actor or component is destroyed
function Component:OnDestroy()
    -- Cleanup code
end

-- Collision callbacks (requires Rigidbody component)
function Component:OnCollisionEnter(other)
    -- Physical collision started
end

function Component:OnCollisionStay(other)
    -- Physical collision continuing
end

function Component:OnCollisionExit(other)
    -- Physical collision ended
end

-- Trigger callbacks (requires Rigidbody with trigger)
function Component:OnTriggerEnter(other)
    -- Trigger overlap started
end

function Component:OnTriggerStay(other)
    -- Trigger overlap continuing
end

function Component:OnTriggerExit(other)
    -- Trigger overlap ended
end
```

### State Transitions

```
Component Created
    │
    ├─▶ Added to actor.components
    ├─▶ Added to SceneDB.on_start_cache
    └─▶ Component reference stored

First Frame (OnStart)
    │
    ├─▶ SceneDB::ProcessOnStart()
    ├─▶ Call component:OnStart()
    ├─▶ Move to on_update_cache
    └─▶ Move to on_late_update_cache

Every Frame (OnUpdate/OnLateUpdate)
    │
    ├─▶ SceneDB::ProcessUpdate()
    │   └─▶ Call component:OnUpdate()
    │
    ├─▶ Physics::Step()
    │   └─▶ Collision callbacks
    │
    └─▶ SceneDB::ProcessLateUpdate()
        └─▶ Call component:OnLateUpdate()

Component Destroyed
    │
    ├─▶ Call component:OnDestroy()
    ├─▶ Remove from all caches
    ├─▶ Remove from actor.components
    └─▶ Rigidbody cleanup (if applicable)
```

## Rendering Pipeline

### Deferred Rendering Architecture

The engine uses **deferred rendering** to optimize draw calls and enable sorting:

**Advantages**:
- Batch rendering (fewer state changes)
- Automatic Z-order sorting
- Separation of game logic and rendering
- Easy to add post-processing effects

**Phases**:

1. **Queue Phase** (During Update):
```lua
function Component:OnUpdate()
    Image.Draw("player.png", x, y)           -- Queued, not rendered
    Text.Draw("Score: 100", 10, 10, ...)     -- Queued, not rendered
end
```

2. **Sort Phase** (Before Rendering):
```cpp
std::stable_sort(image_draw_request_queue.begin(),
                 image_draw_request_queue.end(),
                 [](const auto& a, const auto& b) {
                     return a.sorting_order < b.sorting_order;
                 });
```

3. **Render Phase** (During Engine::Render()):
```cpp
for (auto& request : image_draw_request_queue) {
    // Apply camera transform (if not UI)
    // SDL_RenderCopyEx() with all transforms
}
image_draw_request_queue.clear();
```

### Camera System

**World-Space Rendering**:
- Affected by camera position and zoom
- Used for game world objects (sprites, tiles, etc.)

**Screen-Space Rendering** (UI):
- Ignores camera transform
- Used for HUD, menus, debug text

**Camera Controls**:
```lua
-- Set camera position
Camera.SetPosition(player_x, player_y)

-- Set zoom level
Camera.SetZoom(2.0)  -- 2x zoom

-- Get camera info
local pos = Camera.GetPosition()
local zoom = Camera.GetZoomFactor()
```

## Physics System

### Box2D Integration

**World Simulation**:
```cpp
// In Engine::Update()
RigidbodyWorld::Step(fixed_timestep);  // e.g., 1/60 seconds
```

**Collision Layers** (via Box2D fixtures):
- All rigidbodies can collide by default
- Custom filtering possible via Box2D category/mask bits (future enhancement)

**Contact Listener**:
```cpp
class CollisionListener : public b2ContactListener {
    void BeginContact(b2Contact* contact) {
        // Call Lua OnCollisionEnter() or OnTriggerEnter()
    }
    void EndContact(b2Contact* contact) {
        // Call Lua OnCollisionExit() or OnTriggerExit()
    }
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
        // Call Lua OnCollisionStay() or OnTriggerStay()
    }
};
```

### Rigidbody Configuration

**Properties**:
```lua
-- In actor template JSON or Lua:
rigidbody = {
    body_type = "dynamic",      -- "dynamic" | "kinematic" | "static"
    collider_type = "box",      -- "box" | "circle"
    width = 1.0,
    height = 1.0,
    friction = 0.3,
    bounciness = 0.3,
    density = 1.0,
    gravity_scale = 1.0,
    has_collider = true,
    has_trigger = true          -- Separate trigger shape
}
```

## Scripting System

### Lua Integration

**Lua State**:
- One global Lua state shared by all components
- Initialized in ComponentDB::Init()
- Closed in ComponentDB destructor

**API Binding** (via LuaBridge):
```cpp
luabridge::getGlobalNamespace(L)
    .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("AddComponent", &Actor::AddComponent)
    .endClass()

    .beginClass<Input>("Input")
        .addStaticFunction("GetKey", &Input::GetKey)
        .addStaticFunction("GetMousePosition", &Input::GetMousePosition)
    .endClass();
```

**Component Inheritance**:
```lua
-- Prototype pattern via Lua metatables
local instance = {}
setmetatable(instance, {__index = ComponentPrototype})
```

### Available Lua APIs

**Actor**:
- `actor:GetName()` → string
- `actor:GetID()` → number
- `actor:GetComponent(type)` → component or nil
- `actor:GetComponents(type)` → table of components
- `actor:AddComponent(type)` → component
- `actor:RemoveComponent(component)` → void

**Input**:
- `Input.GetKey(key)` → boolean
- `Input.GetKeyDown(key)` → boolean
- `Input.GetKeyUp(key)` → boolean
- `Input.GetMousePosition()` → {x, y}
- `Input.GetMouseButton(button)` → boolean
- `Input.GetMouseScrollDelta()` → number

**Image**:
- `Image.Draw(name, x, y)` → void
- `Image.DrawEx(name, x, y, rotation, scale_x, scale_y, pivot_x, pivot_y, r, g, b, a, sorting_order)` → void
- `Image.DrawUI(name, x, y)` → void
- `Image.DrawPixel(x, y, r, g, b, a)` → void

**Text**:
- `Text.Draw(content, x, y, font_name, font_size, r, g, b, a)` → void

**Audio**:
- `Audio.PlayChannel(channel, clip_name, loop)` → void
- `Audio.HaltChannel(channel)` → void
- `Audio.SetVolume(channel, volume)` → void

**Scene**:
- `Scene.Load(scene_name)` → void
- `Scene.GetCurrent()` → string
- `Scene.DontDestroy(actor)` → void
- `Scene.Find(name)` → actor or nil
- `Scene.FindAll(name)` → table of actors
- `Scene.Instantiate(template_name)` → actor
- `Scene.Destroy(actor)` → void

**Camera**:
- `Camera.SetPosition(x, y)` → void
- `Camera.GetPosition()` → {x, y}
- `Camera.SetZoom(zoom)` → void
- `Camera.GetZoom()` → number

**Physics**:
- `Physics.Raycast(origin, direction, distance)` → hit_result
- `Physics.RaycastAll(origin, direction, distance)` → table of hit_results

**Debug**:
- `Debug.Log(message)` → void
- `Debug.LogError(message)` → void

**Application**:
- `Application.Quit()` → void
- `Application.Sleep(milliseconds)` → void
- `Application.GetFrame()` → number
- `Application.OpenURL(url)` → void

## Design Patterns

### Patterns Used

1. **Static Singleton**:
   - Used for: Input, ImageDB, TextDB, AudioDB, ComponentDB
   - Rationale: Global access from Lua, single instance per system
   - Example: `Input::GetKey()`, `ImageDB::QueueImageDraw()`

2. **Component Pattern**:
   - Used for: Actor system
   - Rationale: Flexible composition, data-driven design
   - Example: Actors composed of Lua components

3. **Prototype Pattern**:
   - Used for: Component instantiation
   - Rationale: Clone component types, override properties
   - Example: Lua metatable `__index` for inheritance

4. **Deferred Rendering**:
   - Used for: ImageDB, TextDB
   - Rationale: Batch draw calls, enable sorting
   - Example: Queue draw requests, render in batch

5. **Observer Pattern**:
   - Used for: Collision callbacks
   - Rationale: Decouple physics from game logic
   - Example: Box2D ContactListener → Lua callbacks

6. **Command Pattern**:
   - Used for: Deferred actor destruction
   - Rationale: Avoid invalidating iterators during updates
   - Example: Mark actors for destruction, destroy at end of frame

### Memory Management

**C++ Side**:
- `std::unique_ptr` for actor ownership (SceneDB owns actors)
- `std::shared_ptr` for Lua component references (shared with Lua GC)
- RAII for SDL resources (destructors clean up)

**Lua Side**:
- Lua garbage collector manages component tables
- Weak references where appropriate (future enhancement)

**Resource Loading**:
- Lazy loading: textures/fonts/audio loaded on first use
- Caching: resources cached and reused
- No explicit unloading (SDL resources freed on engine shutdown)

## Performance Considerations

### Optimization Strategies

1. **Deferred Rendering**: Batch and sort draw calls
2. **Texture Caching**: Load once, reuse
3. **Component Caching**: Cache OnUpdate/OnLateUpdate components
4. **Stable Sorting**: Preserve submission order for same z-order
5. **Reserve Vectors**: Pre-allocate capacity (e.g., 1000 actors)
6. **Lua Function Caching**: Cache function references, avoid string lookups

### Performance Metrics

**Typical Performance** (release build, 1080p):
- 1000+ actors with components: 60 FPS
- 10000+ draw calls per frame: 60 FPS
- Box2D physics (100 bodies): 60 FPS

**Profiling Hotspots**:
- Lua function calls (use caching)
- SDL_RenderCopyEx() (use batching)
- Box2D Step() (reduce body count or timestep)
- String allocations (use string_view or const char*)

## Future Enhancements

### Planned Features

1. **Particle System**: GPU-accelerated particles
2. **Tilemap Support**: Efficient rendering for grid-based games
3. **Animation System**: Sprite sheet animation with keyframes
4. **Collision Layers**: Customize collision filtering
5. **Entity Query System**: Find actors by tag/layer
6. **Profiler**: Built-in performance profiling
7. **Level Editor**: Visual scene editor
8. **Networking**: Client-server multiplayer

### Architectural Improvements

1. **ECS Migration**: Consider Entity-Component-System for better cache locality
2. **Multi-threading**: Separate render and update threads
3. **Lua Optimization**: LuaJIT for JIT compilation
4. **Resource Manager**: Explicit load/unload with reference counting
5. **Scene Streaming**: Load/unload scenes asynchronously

---

**Last Updated**: 2025-10-28
**Engine Version**: 1.0
**Author**: Jack Gaffney
