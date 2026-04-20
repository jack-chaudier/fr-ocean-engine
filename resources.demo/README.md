# Feature Demo

Single-scene "kitchen sink" that exercises particles, tween, timers, events, physics, input, and text rendering in ~80 lines of Lua.

## Run

```bash
# from repo root, after a build
./build/bin/game_engine --resources resources.demo/

# or via the helper
python3 scripts/run_game.py demo
```

## Controls

- **SPACE** — emit a gold particle burst at origin with camera shake
- **Left click** — spawn a green spark burst at the mouse position
- **ESC** — quit

## What it shows

| Feature | Where |
|---|---|
| Particles | `Particles.Emit()` with `ParticleConfig` on space/click |
| Scheduled timers | `Timer.Every(0.8, ...)` heartbeat burst |
| Tween | `Tween.To(self, "field", target, dur)` (no-op proof) |
| Events | `Event.Emit("user_burst", ...)` |
| Physics | Bouncing `Ball` actor with `Rigidbody` circle |
| Camera | `Camera.Shake()` on burst |
| Input | keyboard + mouse position |
| Text | `Text.Draw()` HUD |
