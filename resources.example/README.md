# Space Shooter - FR-Ocean Engine Example

A demonstration game for the FR-Ocean Engine showcasing:
- Component-based architecture
- Lua scripting for game logic
- Physics with triggers for collision detection
- Scene management
- Audio and text rendering

## Required Assets

Before running this example, you need to add the following assets:

### Images (resources.example/images/)
Place PNG files with these names:
- `player.png` - Player ship sprite (recommended: 32x32 or 64x64)
- `enemy.png` - Enemy ship sprite
- `bullet.png` - Player bullet sprite (small, e.g., 8x16)
- `enemy_bullet.png` - Enemy bullet sprite

### Fonts (resources.example/fonts/)
Place a TrueType font file:
- `OpenSans-Regular.ttf` - Used for score and UI text

You can download OpenSans from Google Fonts: https://fonts.google.com/specimen/Open+Sans

### Audio (resources.example/audio/) - Optional
- `shoot.wav` or `shoot.ogg` - Player shooting sound
- `explosion.wav` or `explosion.ogg` - Enemy destruction sound

## Running the Example

1. Build the engine (see main README.md)
2. Copy or symlink `resources.example` to `build/bin/resources`
3. Run the game:
   ```bash
   cd build/bin
   ./game_engine
   ```

Or use the --resources flag:
```bash
./game_engine --resources path/to/resources.example/
```

## Controls

- **WASD** or **Arrow Keys**: Move player
- **Space** or **Left Mouse Button**: Shoot
- **R**: Restart (when game over)
- **ESC**: Quit

## Game Mechanics

- Enemies spawn from the top and move side-to-side while descending
- Enemies periodically shoot bullets downward
- Destroy enemies by shooting them to earn points
- Game ends if player is hit by enemy or enemy bullet
