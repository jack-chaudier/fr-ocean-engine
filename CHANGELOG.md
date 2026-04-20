# Changelog

All notable changes to FR-Ocean Engine will be documented in this file. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/).

## [1.1.0] — 2026-04-20

A cleanup, refactor, and polish release. The engine's public behavior is unchanged; the source tree is tighter, the sample story is clearer, and the test harness actually runs.

### Added
- `--self-check [N]` CLI flag that runs N frames (default 60) and exits 0. Feeds the CI smoke test.
- Two CTest targets (`smoke_platformer`, `smoke_demo`) fail on any `[FATAL]` or `[ERROR]` log line during the 60-frame run.
- **`resources.demo/`** — a single-scene "kitchen sink" that exercises particles, tweens, timers, events, physics, input, and text in ~80 lines of Lua.
- Platformer sample polish: variable jump height, coyote time, jump buffering, stomp/coin/death/land particles, camera shake on stomp + death, new `Spike` and `Flag` components, two-level progression with fade transitions, persistent score via `Scene.DontDestroy`, a sky + hills background sprite.
- New SVG architecture and frame-flow diagrams under `docs/` for the README.

### Fixed
- **Shutdown-order crash**: `ComponentDB::Shutdown()` used to call `lua_close` before the cached `LuaRef` shared pointers were released, crashing on clean exit. Caches are now cleared before the state is closed, and every subsystem that pins Lua references is flushed from `Engine::~Engine()`.
- `Debug.LogError` now routes to `LOG_ERROR` instead of `LOG_INFO`.
- `DebugDraw::Init()` no longer leaks its singleton via `new`.

### Changed
- Hero sample game is now `resources.platformer/` (was `resources.example`). Space Shooter and Puzzle Match samples were retired — the platformer plus the feature demo cover the same engine surface with less maintenance.
- `SceneDB::ProcessSceneUpdate` / `ProcessSceneLateUpdate` are now one `ProcessLifecycleCache` helper parameterised on method name.
- `ComponentDB::overrideRigidbodyfValue` is now a small member-pointer table instead of a 22-branch if/else ladder.
- `ComponentDB::CPPLog` routes through `LOG_INFO` instead of `std::cout`.
- `SceneDB::clearLuaRefs` and `Engine::~Engine()` now release every LuaRef-holding subsystem before `lua_close`.
- Every engine header uses `#pragma once` (was a mix of `#ifndef` / `#pragma once`).
- `ApplicationAPI.h` / `EngineUtils.h` renamed to `.hpp` for consistency.
- Engine version bumped to 1.1.0.

### Removed
- Stale IDE project files (`game_engine.sln`, `game_engine.vcxproj*`, `game_engine.xcodeproj/`). Use CMake generators.
- Top-level `Makefile` (use `cmake --build` or `scripts/run_game.py`).
- `README.md.backup`.
- `docs/` static-site viewer (replaced with inline SVG diagrams; markdown renders on GitHub).
- Unused `ComponentDB::CDB` map and `SceneDB::scene_path` field.
- `resources.example/` (Space Shooter) and `resources.puzzle/` sample games.

## [1.0.1] — 2025-12-29

### Added
- `Image.DrawRect()` function for filled rectangle rendering.
- `Rigidbody.RecreateFixtures()` for runtime fixture updates.
- Two new example games: Platformer and Puzzle Match.

### Fixed
- `--resources` flag now propagates to all subsystems.
- Scene position overrides now update Box2D body positions.
- Width/height overrides now recreate Box2D fixtures.
- Trigger-collider interactions fixed (mask = 0xFFFF).
- Collision callbacks skip C++ userdata components.
- Actor destruction is deferred to after the physics step.

## [1.0.0] — 2025-10-28

Initial release. C++17 engine with SDL2 rendering, Box2D physics, SDL2_mixer audio, Lua 5.4 scripting via LuaBridge, scene/actor/component system, camera, fonts, input, collision callbacks, raycasting, and cross-platform CMake build.

---

## Roadmap

Ideas, not commitments. Ordered by interest.

- Tilemap rendering and collision.
- Sprite animation state machines (the `Animation` API exists but has no frame atlas tooling yet).
- Asset hot-reload (watch `resources/` and re-read on change).
- Networking — probably a thin UDP layer, not real multiplayer.
- ECS-ish storage if/when the `inline static` caches in `SceneDB` become a real bottleneck.
