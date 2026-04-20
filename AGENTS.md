# AGENTS.md

Guidance for any coding agent (Codex, Claude Code, etc.) working in this repo. Extends `CLAUDE.md`.

## Repo layout

- `game_engine/` — C++17 engine core
- `resources.platformer/` — hero sample game (two-level platformer)
- `resources.demo/` — minimal feature showcase
- `vendor/` — third-party libs (do not edit without explicit request)
- `scripts/run_game.py` — build + run helper
- `docs/` — SVG diagrams referenced by the README

## Build / run

```bash
cmake --preset release
cmake --build --preset release
./build/bin/game_engine --resources resources.platformer/
```

Helper: `python3 scripts/run_game.py {platformer|demo}`.

## Tests

```bash
cd build && ctest --output-on-failure
```

Two headless smoke targets. Always run after engine changes.

## Conventions

- C++17, `#pragma once`, 4 spaces, PascalCase public / snake_case internal.
- Run `clang-format` (repo config) on large touches.
- Log via `LOG_INFO/WARNING/ERROR/FATAL`.
- Throw `EngineException` subtypes for fatal startup/config/resource errors.
- Use `ConfigManager::GetResourcesPath()` for paths.
- Collision callbacks receive `collision = { other, point, normal, relative_velocity, is_trigger }`.

## Change checklist

- New Lua API: update `ComponentDB` bindings + `API_REFERENCE.md` + (ideally) exercise in `resources.demo/`.
- Engine behavior change: update `README.md` and `CHANGELOG.md`.
- New subsystem: update `ARCHITECTURE.md` + the `docs/architecture.svg` diagram.
- Assets: `.gitignore` drops generated images (regenerated via `create_assets.py`).

## Workflow

1. Identify subsystem(s) touched.
2. Plan minimal change set + doc updates.
3. Implement.
4. `cmake --build --preset release`.
5. `cd build && ctest --output-on-failure`.
6. Summarize changes in the final response.
