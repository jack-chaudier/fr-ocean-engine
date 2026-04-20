# FR-Ocean Engine — Architecture

A concrete tour of the engine as it exists today. For the runtime API surface, see [API_REFERENCE.md](API_REFERENCE.md).

<p align="center">
  <img src="docs/architecture.svg" alt="Architecture" width="880">
</p>

## Shape

- **C++17 core** owning the loop, rendering, physics, audio, and the Lua state.
- **Lua 5.4 scripts** define game logic as components attached to actors.
- **JSON** declares scenes and actor templates.
- **CMake** builds everything; all third-party libs are vendored.

No DLLs to install on macOS or Windows. On Linux, SDL2 comes from the distro.

## File layout

```
game_engine/
  main.cpp            CLI, SDL init, SdlLifecycle RAII
  Engine.{hpp,cpp}    Game loop: Input → Update → Render
  ConfigManager       game.config / rendering.config parsing
  Logger              leveled, timestamped, thread-safe
  SceneDB             actor lifecycle, caches, scene load
  ComponentDB         Lua state + LuaBridge bindings
  Actor               components map, collision callbacks
  Renderer            SDL2 window + renderer + camera
  ImageDB             deferred sprite queue, texture cache
  TextDB              SDL2_ttf, deferred text queue
  AudioDB             SDL2_mixer channels
  Rigidbody           Box2D body wrapper
  RigidbodyWorld      Box2D world step + contact listener
  CollisionListener   contact callbacks → Lua
  CollisionLayers     named collision filter pairs
  PhysicsQuery        Raycast / RaycastAll
  Input               keyboard + mouse state
  Time                dt, total time, scale, frame count
  EventSystem         pub/sub for Lua callbacks
  Scheduler           Timer.After / Timer.Every
  Tween               float/vec tween on Lua tables
  AnimationDB         sprite-sheet animation (frames + speed)
  ParticleSystem      pooled emitters
  DebugDraw           Box2D b2Draw visualization (F1)
  SceneTransition     fade-in / fade-out between scenes
  Transform           transform helper for components
  EngineException.hpp exception hierarchy
  EngineUtils.hpp     JSON file read helper
  ApplicationAPI.hpp  Quit / Sleep / OpenURL / GetFrame
```

## Game loop

<p align="center">
  <img src="docs/frame-flow.svg" alt="Frame flow" width="880">
</p>

Inside `Engine::GameLoop(int max_frames)`:

```
loop:
  Input::BeginFrame()
  while SDL_PollEvent(&e):
    if SDL_QUIT: quit
    if SDL_KEYDOWN F1: DebugDraw::ToggleEnabled()
    Input::ProcessEvent(e)
  Engine::Update()          // time, scene lifecycle, schedulers
  Engine::Render()          // clear, images, particles, text, debug, present
  Input::LateUpdate()
  if max_frames >= 0 and frames+1 >= max_frames: quit
```

`Engine::Update()`:

1. `Time::Update()`.
2. If a scene load is pending (from `Scene.Load` or `SceneTransition`), clear the schedulers and load it.
3. Tick `Scheduler`, `Tween`, `AnimationDB`, `ParticleSystem`, `SceneTransition`, `Renderer::UpdateCamera`.
4. `SceneDB::UpdateScene()`: `OnStart` for fresh components → deferred Rigidbody init → `OnUpdate` → `OnLateUpdate` → remove-queued components → destroy-queued actors → step Box2D.

`Engine::Render()`:

1. Clear the color buffer.
2. Drain `ImageDB` in `sorting_order` order (world + UI).
3. Render queued particles with camera transform.
4. Drain `TextDB`.
5. Drain pixel queue (for debug / rects).
6. `DebugDraw::Render()` if enabled (Box2D bodies overlay).
7. `SceneTransition::Render()` (fade overlay).
8. `Renderer::present()`.

## Component lifecycle

```
scene load → Actor created → components attached (Lua tables + Rigidbody userdata)
                         ↓
               ProcessSceneOnStart    → every component's OnStart() exactly once
                         ↓ (each frame)
               ProcessLifecycleCache("OnUpdate")
                         ↓
               physics step + collision callbacks (OnCollisionEnter/Stay/Exit, OnTriggerEnter/Stay/Exit)
                         ↓
               ProcessLifecycleCache("OnLateUpdate")
                         ↓
               RemoveActorComponents  (per-actor removal queue)
                         ↓
               ActorsPendingDestruction (defers Box2D-body destruction out of physics step)
```

`ProcessLifecycleCache` is the same helper for both update passes; it auto-disables a component after three throws from `pcall`-style error handling.

## Shutdown ordering

This is load-bearing and easy to break. Anything caching `std::shared_ptr<luabridge::LuaRef>` must be cleared before `lua_close`:

```
Engine::~Engine()
  EventSystem::Clear()     ← LuaRefs in subscriptions
  Scheduler::Clear()       ← LuaRefs in Timer callbacks
  Tween::Clear()           ← LuaRefs in tween targets
  AnimationDB::Clear()
  ParticleSystem::Clear()
  scene.clearLuaRefs()     ← actors, caches, templates
  ComponentDB::Shutdown()  ← componentTypeCache, then lua_close
  AudioDB::Shutdown()
  TextDB::Shutdown()
```

`ComponentDB::Shutdown()` itself clears `componentTypeCache` *before* calling `lua_close(L)`. The CI smoke test catches regressions here: it runs 60 frames and exits, so the whole teardown path must not crash.

## Rendering pipeline

Rendering is deferred. Lua draws during `OnUpdate`/`OnLateUpdate` by calling `Image.Draw*` / `Text.Draw`, which push entries into per-frame queues in `ImageDB` and `TextDB`. `Engine::Render()` then:

- Sorts sprite queues by `sorting_order` (stable).
- Applies camera transform (world-space) or identity (`DrawUI*`).
- Uses one batched `SDL_RenderCopy*` per sprite, with texture color/alpha modulation for tinting.
- Renders particles after images so they float on top of world geometry but below text.

Camera supports position, zoom, lerp-follow, bounds, and timed screen shake; see `Renderer::UpdateCamera`.

## Physics

`RigidbodyWorld` owns a single `b2World` stepped at 60 Hz with 8 velocity / 3 position iterations. Each `Rigidbody` wraps a `b2Body` and is attached to an Actor. Lua sees it as userdata with `GetPosition` / `SetVelocity` / `AddForce` / etc.

Collision and trigger contacts route through `CollisionListener`, which looks up the Lua actors by their Box2D user-data pointers and invokes callbacks with a `collision` table `{ other, point, normal, relative_velocity, is_trigger }`.

`CollisionLayers` lets Lua declare named layers and pairwise masks without touching Box2D categories directly. `PhysicsQuery` exposes `Physics.Raycast` and `Physics.RaycastAll`.

Actors destroyed mid-step (including from collision callbacks) are queued into `actors_to_destroy` and actually removed in `ActorsPendingDestruction` after the step finishes — destroying a `b2Body` inside a contact callback is undefined behavior.

## Scripting

`ComponentDB::Init()` builds one Lua state with LuaBridge and registers every engine API in one pass. Component types under `resources.<game>/component_types/*.lua` are loaded on demand the first time a scene references them, then cached in `componentTypeCache` as the prototype. Instantiating a component = making a fresh Lua table and wiring its metatable `__index` to the prototype, so each instance has its own per-instance state while sharing behavior.

Rigidbody is the one exception — it's a C++ class exposed as LuaBridge userdata. `CreateComponent("Rigidbody", ...)` deep-copies the prototype and hands the raw pointer to LuaBridge, which takes ownership via Lua's GC. This raw `new` is intentional.

## Error handling

Fatal conditions raise from the `EngineException` hierarchy in `EngineException.hpp`:

- `ConfigurationException` — bad `game.config` / `rendering.config`.
- `ResourceNotFoundException` — missing scene / template / image / font / audio.
- `ScriptException` — syntax or load error in a component file.
- `RenderException` — SDL or SDL_image failure.
- `AudioException` — SDL_mixer failure.

`main.cpp` catches these, logs with `LOG_FATAL`, and exits 1. Recoverable per-frame problems use `LOG_WARNING` / `LOG_ERROR` and are swallowed — a single misbehaving Lua component is disabled after three throws rather than taking down the game.

## Extending

- **Add a Lua API**: add the C++ function → bind it in `ComponentDB::Init()` → document in `API_REFERENCE.md` → exercise in `resources.demo/component_types/Showcase.lua` if cheap.
- **Add a subsystem**: model it after `ParticleSystem` — static class with `Init`, `Update(dt)`, `Clear`. Wire `Init` into `Engine::Engine()` and `Clear` into `Engine::~Engine()` before `ComponentDB::Shutdown()`.
- **Add a scene**: drop a `.scene` JSON under `resources.<game>/scenes/`. Reference it from code via `Scene.Load("name")` or `Scene.LoadWithTransition("name", "fade", 0.5)`.
- **Add a component**: drop a `.lua` under `resources.<game>/component_types/`. Reference it by table name in an actor template or scene.
