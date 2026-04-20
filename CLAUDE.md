# CLAUDE.md

Guidance for Claude Code when working in this repo.

## Build

```bash
cmake --preset release
cmake --build --preset release
./build/bin/game_engine --resources resources.platformer/
```

Or use the helper:

```bash
python3 scripts/run_game.py platformer   # hero sample
python3 scripts/run_game.py demo         # minimal feature showcase
```

## Command-line flags

```
--resources <path>    Override resources dir (default: resources/)
--debug               Enable DEBUG-level logs
--self-check [N]      Boot engine, run N frames (default 60), exit 0. Used by CI.
--version             Print version and exit
--help                Print usage
```

## Tests

CTest has two smoke targets that boot each sample headless and fail on any `[FATAL]` or `[ERROR]` line in the log.

```bash
cd build && ctest --output-on-failure
```

Always run CTest after engine changes. If you add a new Lua API, write a short Lua component in `resources.demo/` that exercises it so the smoke test covers it.

## Architecture

Core systems live in `game_engine/` as static-class "DBs" plus an `Engine` orchestrator.

- **Engine** (`Engine.cpp`) — owns the game loop.
- **SceneDB** — actor lifecycle, scene load, `ProcessLifecycleCache` helper that runs `OnUpdate` / `OnLateUpdate` from a keyed cache.
- **ComponentDB** — single Lua state, hosts every LuaBridge binding in `Init()`, and is responsible for clean shutdown order (caches before `lua_close`).
- **ImageDB** / **TextDB** / **AudioDB** — deferred-queue rendering / SDL2_ttf / SDL2_mixer.
- **RigidbodyWorld** — Box2D world step. `Rigidbody` is the per-actor wrapper.
- **ParticleSystem** / **Tween** / **Scheduler** (`Timer`) / **EventSystem** / **AnimationDB** — all expose a `Clear()` that `Engine::~Engine()` calls before `ComponentDB::Shutdown()`.
- **Renderer** / **Input** / **Logger** / **ConfigManager** / **SceneTransition** / **DebugDraw** / **CollisionLayers** / **PhysicsQuery** — straightforward.

### Frame flow

```
Input::BeginFrame → SDL events → Time::Update →
  load pending scene (if any) → Scheduler/Tween/Animation/Particles/SceneTransition/Camera update →
  SceneDB::UpdateScene (OnStart → OnUpdate → RB init → physics → OnLateUpdate → destroy queued)
  → Render (clear → images → particles → text → pixels → debug → transition → present)
```

### Error handling

`EngineException` hierarchy in `EngineException.hpp` (Configuration / ResourceNotFound / Script / Render / Audio). Use `LOG_INFO / WARNING / ERROR / FATAL` for anything recoverable.

**Shutdown order matters.** Anything that caches `std::shared_ptr<luabridge::LuaRef>` (SceneDB caches, ComponentDB `componentTypeCache`, EventSystem, Scheduler, Tween, AnimationDB, ParticleSystem) must be cleared *before* `lua_close(L)`. `Engine::~Engine()` already does this; respect the order if you add new LuaRef-holding subsystems.

## Code standards

- C++17, `#pragma once`, 4-space indent, K&R braces.
- PascalCase classes / public methods; snake_case variables / private helpers.
- `std::unique_ptr` / `std::shared_ptr` over raw `new`/`delete` — *except* where LuaBridge takes ownership of a raw pointer (see `ComponentDB::CreateComponent` for `Rigidbody`).
- Keep engine headers `.hpp`. Vendor `.h` (Helper.h) is fine.
- One subsystem per file pair.

## Data file layout

```
resources.<game>/
  game.config          JSON: game_title, initial_scene
  rendering.config     JSON: resolution, clear color
  scenes/*.scene       JSON: actor list with components
  actor_templates/*.template  JSON: default component mix
  component_types/*.lua       Lua: component definitions
  images/ audio/ fonts/       assets
  create_assets.py     Python (Pillow) regen script
```

Hero game: `resources.platformer/`. Feature demo: `resources.demo/`.

The symlink `resources/` → `resources.platformer/` is used by the CMake bundled build.

## Dependencies (vendored in `vendor/`)

SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, Box2D, Lua 5.4, LuaBridge, GLM, RapidJSON.

## When to update docs

| Change | Update |
|---|---|
| New Lua binding | `ComponentDB::Init()` + `API_REFERENCE.md` |
| New subsystem | `ARCHITECTURE.md` + README architecture SVG |
| User-visible behavior change | `README.md` + `CHANGELOG.md` |
| New sample game | `scripts/run_game.py` `GAMES` map + CMake test target |

## Adding a new Lua binding — checklist

1. Add the C++ function.
2. Bind it in `ComponentDB::Init()` under the right namespace.
3. Document in `API_REFERENCE.md`.
4. Exercise it in `resources.demo/component_types/Showcase.lua` if cheap.
5. `cmake --build --preset release && cd build && ctest`.

## Common pitfalls

- Do not close the Lua state without clearing every LuaRef cache first.
- When an actor is destroyed mid-physics-step, the deferral in `ActorsPendingDestruction` is what keeps Box2D alive. Don't remove it.
- New LuaRef-holding subsystem? Add a `Clear()` and call it from `Engine::~Engine()` above `ComponentDB::Shutdown()`.
- The `componentTypeCache` is keyed by component *type* string; two scenes referencing the same type share the prototype.
- `MovingPlatform` / `Coin` use `kinematic` rigidbodies so the physics engine does not integrate gravity but still fires triggers — keep that in mind when scripting collision-heavy actors.
