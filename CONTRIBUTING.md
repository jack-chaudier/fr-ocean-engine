# Contributing to FR-Ocean Engine

Thanks for your interest in contributing. This doc is short on purpose.

## Setup

### Prerequisites

- CMake ≥ 3.16
- A C++17 compiler: Clang, GCC 9+, or MSVC (VS 2019+)
- Python 3 + `uv` (optional, for regenerating sample sprites)
- Linux only: `libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev`

### Build

```bash
cmake --preset release
cmake --build --preset release
./build/bin/game_engine --resources resources.platformer/
```

### Test

```bash
cd build && ctest --output-on-failure
```

Two headless smoke targets (`smoke_platformer`, `smoke_demo`) boot each sample for 60 frames and fail on `[ERROR]` / `[FATAL]` logs. Run after any engine change.

## Code standards

Full rules in [CODE_STANDARDS.md](CODE_STANDARDS.md). Summary:

- C++17, `#pragma once`, 4-space indent, K&R braces.
- `PascalCase` classes / public methods / Lua component tables.
- `snake_case` variables / private helpers / Lua fields / config keys.
- `UPPER_SNAKE_CASE` for compile-time constants.
- Attach `*` and `&` to the type: `SDL_Texture* texture`, `const std::string& s`.
- Prefer `std::unique_ptr` / `std::shared_ptr` over raw `new`/`delete`. The one documented exception is `ComponentDB::CreateComponent` handing a raw `Rigidbody*` to LuaBridge for GC ownership.
- Log via `LOG_INFO / WARNING / ERROR / FATAL`. Do not use `std::cout` in engine code.
- Throw `EngineException` subtypes for fatal startup / config / resource errors.
- Don't comment what the code already says. Leave a comment only when the *why* is non-obvious.
- Format with `clang-format` (repo `.clang-format`) on large edits.

## Pull request checklist

1. Branch from `main`: `git checkout -b feat/short-name` or `fix/short-name`.
2. Build cleanly (`cmake --build --preset release`) and run `ctest`.
3. Keep the diff focused. One logical change per commit.
4. Update docs when relevant:
   - New Lua API → `ComponentDB::Init()` bindings + [API_REFERENCE.md](API_REFERENCE.md). Exercise it in `resources.demo/component_types/Showcase.lua` if cheap, so smoke coverage grows with the API.
   - User-visible behavior change → `README.md` + `CHANGELOG.md`.
   - New subsystem or flow change → `ARCHITECTURE.md`, and regenerate the SVG under `docs/` if appropriate.
5. Commit messages: one-line summary in imperative mood, optional body with the *why*. Example: `fix: clear EventSystem before lua_close to avoid shutdown crash`.
6. Open the PR. Include a reproduction or test steps in the description when relevant.

## Reporting bugs

Open an issue with:

- Platform + compiler + commit SHA.
- Exact steps to reproduce (sample game + controls).
- Expected vs actual behavior.
- Relevant log output (`--debug` for DEBUG-level).
- Minimal Lua/scene repro if the bug is script-facing.

## License

By contributing, you agree that your contributions will be licensed under the MIT License — see [LICENSE](LICENSE).
