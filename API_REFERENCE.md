# FR-Ocean Engine API Reference

Complete reference for all Lua APIs available in FR-Ocean Engine.

## Table of Contents
- [Actor API](#actor-api)
- [Input API](#input-api)
- [Image API](#image-api)
- [Text API](#text-api)
- [Audio API](#audio-api)
- [Scene API](#scene-api)
- [Camera API](#camera-api)
- [Physics API](#physics-api)
- [Rigidbody API](#rigidbody-api)
- [Application API](#application-api)
- [Debug API](#debug-api)
- [Component Lifecycle](#component-lifecycle)

---

## Actor API

The Actor class represents a game object in the scene. Actors are containers for components.

### actor:GetName()

Gets the actor's name.

**Returns**: `string` - The actor's name

**Example**:
```lua
function Component:OnStart()
    local name = self.actor:GetName()
    Debug.Log("Actor name: " .. name)
end
```

---

### actor:GetID()

Gets the actor's unique ID.

**Returns**: `number` - The actor's globally unique 64-bit ID

**Example**:
```lua
local id = self.actor:GetID()
Debug.Log("Actor ID: " .. id)
```

---

### actor:GetComponent(type)

Gets the first component of the specified type attached to this actor.

**Parameters**:
- `type` (string): The component type name (e.g., "Rigidbody", "PlayerController")

**Returns**: `table` - The component, or `nil` if not found

**Example**:
```lua
function Component:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    if self.rigidbody ~= nil then
        self.rigidbody:SetVelocity({x = 5, y = 0})
    end
end
```

---

### actor:GetComponents(type)

Gets all components of the specified type attached to this actor.

**Parameters**:
- `type` (string): The component type name

**Returns**: `table` - Array of components (1-indexed), or empty table if none found

**Example**:
```lua
local all_scripts = self.actor:GetComponents("CustomScript")
for i, script in ipairs(all_scripts) do
    Debug.Log("Script " .. i .. " found")
end
```

---

### actor:AddComponent(type)

Adds a new component to this actor at runtime.

**Parameters**:
- `type` (string): The component type name (must exist in `resources/component_types/`)

**Returns**: `table` - The newly created component

**Example**:
```lua
function Component:OnStart()
    -- Dynamically add a Rigidbody component
    local rb = self.actor:AddComponent("Rigidbody")
    rb.body_type = "dynamic"
    rb.x = 5
    rb.y = 10
end
```

**Note**: The new component's `OnStart()` will be called on the next frame.

---

### actor:RemoveComponent(component)

Removes a component from this actor.

**Parameters**:
- `component` (table): The component to remove

**Example**:
```lua
function Component:OnUpdate()
    if condition then
        -- Remove this component from the actor
        self.actor:RemoveComponent(self)
    end
end
```

**Note**: Removal is deferred until the end of the frame. The component's `OnDestroy()` will be called before removal.

---

## Input API

The Input API provides access to keyboard, mouse, and scroll input.

### Input.GetKey(key)

Checks if a key is currently held down.

**Parameters**:
- `key` (string): Key name (e.g., "space", "a", "left", "return", "escape")

**Returns**: `boolean` - `true` if key is down, `false` otherwise

**Example**:
```lua
function Component:OnUpdate()
    if Input.GetKey("space") then
        Debug.Log("Space is held down")
    end
end
```

---

### Input.GetKeyDown(key)

Checks if a key was pressed this frame (single-frame event).

**Parameters**:
- `key` (string): Key name

**Returns**: `boolean` - `true` only on the frame the key was pressed

**Example**:
```lua
function Component:OnUpdate()
    if Input.GetKeyDown("space") then
        Debug.Log("Space was just pressed")
        self:Jump()
    end
end
```

---

### Input.GetKeyUp(key)

Checks if a key was released this frame (single-frame event).

**Parameters**:
- `key` (string): Key name

**Returns**: `boolean` - `true` only on the frame the key was released

**Example**:
```lua
function Component:OnUpdate()
    if Input.GetKeyUp("space") then
        Debug.Log("Space was just released")
    end
end
```

---

### Input.GetMousePosition()

Gets the current mouse cursor position in screen space.

**Returns**: `table` - `{x = number, y = number}` in pixels from top-left corner

**Example**:
```lua
local mouse_pos = Input.GetMousePosition()
Debug.Log("Mouse at: " .. mouse_pos.x .. ", " .. mouse_pos.y)
```

---

### Input.GetMouseButton(button)

Checks if a mouse button is currently held down.

**Parameters**:
- `button` (number): Button number (1 = left, 2 = middle, 3 = right)

**Returns**: `boolean` - `true` if button is down, `false` otherwise

**Example**:
```lua
if Input.GetMouseButton(1) then
    Debug.Log("Left mouse button is held")
end
```

---

### Input.GetMouseButtonDown(button)

Checks if a mouse button was pressed this frame.

**Parameters**:
- `button` (number): Button number

**Returns**: `boolean` - `true` only on the frame the button was pressed

---

### Input.GetMouseButtonUp(button)

Checks if a mouse button was released this frame.

**Parameters**:
- `button` (number): Button number

**Returns**: `boolean` - `true` only on the frame the button was released

---

### Input.GetMouseScrollDelta()

Gets the mouse scroll wheel delta for this frame.

**Returns**: `number` - Vertical scroll delta (positive = up, negative = down, 0 = no scroll)

**Example**:
```lua
local scroll = Input.GetMouseScrollDelta()
if scroll > 0 then
    Debug.Log("Scrolled up")
elseif scroll < 0 then
    Debug.Log("Scrolled down")
end
```

---

## Image API

The Image API handles sprite rendering with transforms, colors, and sorting.

### Image.Draw(image_name, x, y)

Draws a sprite at the specified world position.

**Parameters**:
- `image_name` (string): Image filename (e.g., "player.png", without path)
- `x` (number): World X position
- `y` (number): World Y position

**Example**:
```lua
function Component:OnUpdate()
    Image.Draw("player.png", 100, 200)
end
```

**Note**: Images are loaded from `resources/images/` and cached automatically.

---

### Image.DrawEx(image_name, x, y, rotation, scale_x, scale_y, pivot_x, pivot_y, r, g, b, a, sorting_order)

Draws a sprite with full transform and color control.

**Parameters**:
- `image_name` (string): Image filename
- `x`, `y` (number): World position
- `rotation` (number): Rotation in degrees (clockwise)
- `scale_x`, `scale_y` (number): Scale factors (1.0 = normal)
- `pivot_x`, `pivot_y` (number): Pivot point (0.5 = center)
- `r`, `g`, `b`, `a` (number): Color modulation (0-255)
- `sorting_order` (number): Z-depth (lower = behind, higher = in front)

**Example**:
```lua
-- Draw rotated, scaled, tinted sprite
Image.DrawEx("enemy.png",
    100, 200,        -- position
    45,              -- 45° rotation
    1.5, 1.5,        -- 1.5x scale
    0.5, 0.5,        -- center pivot
    255, 100, 100, 255,  -- red tint
    10)              -- sorting order 10
```

---

### Image.DrawUI(image_name, x, y)

Draws a sprite in screen space (ignores camera).

**Parameters**:
- `image_name` (string): Image filename
- `x`, `y` (number): Screen position in pixels

**Example**:
```lua
function Component:OnUpdate()
    -- Draw HUD element (always stays in place on screen)
    Image.DrawUI("health_bar.png", 10, 10)
end
```

---

### Image.DrawUIEx(image_name, x, y, r, g, b, a, sorting_order)

Draws a UI sprite with color and sorting control.

**Parameters**:
- `image_name` (string): Image filename
- `x`, `y` (number): Screen position
- `r`, `g`, `b`, `a` (number): Color modulation (0-255)
- `sorting_order` (number): Z-depth for UI layering

---

### Image.DrawPixel(x, y, r, g, b, a)

Draws a single pixel (useful for debugging).

**Parameters**:
- `x`, `y` (number): Screen position
- `r`, `g`, `b`, `a` (number): RGBA color (0-255)

**Example**:
```lua
-- Draw red debug pixel
Image.DrawPixel(100, 100, 255, 0, 0, 255)
```

---

## Text API

The Text API handles TrueType font rendering.

### Text.Draw(content, x, y, font_name, font_size, r, g, b, a)

Draws text on screen.

**Parameters**:
- `content` (string): Text to display
- `x`, `y` (number): Screen position in pixels
- `font_name` (string): Font filename (e.g., "Arial.ttf", without path)
- `font_size` (number): Font size in points
- `r`, `g`, `b`, `a` (number): Color (0-255)

**Example**:
```lua
function Component:OnUpdate()
    Text.Draw("Score: 1000", 10, 10, "Arial.ttf", 32, 255, 255, 255, 255)
end
```

**Note**: Fonts are loaded from `resources/fonts/` and cached at each size.

---

## Audio API

The Audio API handles sound effects and music playback.

### Audio.PlayChannel(channel, clip_name, loop)

Plays an audio clip on a specific channel.

**Parameters**:
- `channel` (number): Channel number (0-15, or -1 for auto-select)
- `clip_name` (string): Audio filename (e.g., "explosion.wav", without path)
- `loop` (boolean): `true` to loop infinitely, `false` to play once

**Example**:
```lua
-- Play sound effect once
Audio.PlayChannel(0, "explosion.wav", false)

-- Loop background music
Audio.PlayChannel(1, "background_music.ogg", true)
```

**Note**: Audio files are loaded from `resources/audio/` and cached.

---

### Audio.HaltChannel(channel)

Stops playback on a channel.

**Parameters**:
- `channel` (number): Channel number to stop (0-15)

**Example**:
```lua
Audio.HaltChannel(1)  -- Stop channel 1
```

---

### Audio.SetVolume(channel, volume)

Sets the volume for a channel.

**Parameters**:
- `channel` (number): Channel number (0-15)
- `volume` (number): Volume level (0.0 = silent, 1.0 = full volume)

**Example**:
```lua
Audio.SetVolume(0, 0.5)  -- Set channel 0 to 50% volume
```

---

## Scene API

The Scene API manages scene loading and actor lifecycle.

### Scene.Load(scene_name)

Loads a new scene (destroys current scene actors unless marked dont_destroy).

**Parameters**:
- `scene_name` (string): Scene filename (e.g., "level1", without `.scene` extension)

**Example**:
```lua
function Component:OnUpdate()
    if Input.GetKeyDown("return") then
        Scene.Load("main_menu")
    end
end
```

---

### Scene.GetCurrent()

Gets the name of the current scene.

**Returns**: `string` - Current scene name

**Example**:
```lua
local current = Scene.GetCurrent()
Debug.Log("Current scene: " .. current)
```

---

### Scene.DontDestroy(actor)

Marks an actor to persist across scene transitions.

**Parameters**:
- `actor` (table): The actor to preserve

**Example**:
```lua
function Component:OnStart()
    -- Make this actor persist across scenes
    Scene.DontDestroy(self.actor)
end
```

---

### Scene.Find(name)

Finds the first actor with the specified name.

**Parameters**:
- `name` (string): Actor name to search for

**Returns**: `table` - The actor, or `nil` if not found

**Example**:
```lua
local player = Scene.Find("Player")
if player ~= nil then
    local rb = player:GetComponent("Rigidbody")
    rb:SetVelocity({x = 0, y = 0})
end
```

---

### Scene.FindAll(name)

Finds all actors with the specified name.

**Parameters**:
- `name` (string): Actor name to search for

**Returns**: `table` - Array of actors (1-indexed), or empty table if none found

**Example**:
```lua
local enemies = Scene.FindAll("Enemy")
for i, enemy in ipairs(enemies) do
    Debug.Log("Found enemy " .. i)
end
```

---

### Scene.Instantiate(template_name)

Creates a new actor from a template at runtime.

**Parameters**:
- `template_name` (string): Template filename (without path or extension)

**Returns**: `table` - The newly created actor

**Example**:
```lua
function Component:OnUpdate()
    if Input.GetKeyDown("space") then
        -- Spawn a bullet
        local bullet = Scene.Instantiate("bullet_template")
        local rb = bullet:GetComponent("Rigidbody")
        rb:SetPosition({x = self.x, y = self.y})
        rb:SetVelocity({x = 10, y = 0})
    end
end
```

---

### Scene.Destroy(actor)

Destroys an actor (deferred until end of frame).

**Parameters**:
- `actor` (table): The actor to destroy

**Example**:
```lua
function Component:OnTriggerEnter(other)
    Scene.Destroy(other)  -- Destroy the other actor
end
```

---

## Camera API

The Camera API controls the camera position and zoom.

### Camera.SetPosition(x, y)

Sets the camera position in world space.

**Parameters**:
- `x`, `y` (number): Camera world position

**Example**:
```lua
function Component:OnLateUpdate()
    -- Camera follows player
    local player_pos = self.rigidbody:GetPosition()
    Camera.SetPosition(player_pos.x, player_pos.y)
end
```

---

### Camera.GetPosition()

Gets the current camera position.

**Returns**: `table` - `{x = number, y = number}`

---

### Camera.SetZoom(zoom)

Sets the camera zoom level.

**Parameters**:
- `zoom` (number): Zoom factor (1.0 = normal, 2.0 = 2x zoom, 0.5 = 50% zoom)

**Example**:
```lua
Camera.SetZoom(2.0)  -- Zoom in 2x
```

---

### Camera.GetZoom()

Gets the current camera zoom level.

**Returns**: `number` - Current zoom factor

---

## Physics API

The Physics API provides raycasting functionality.

### Physics.Raycast(origin, direction, distance)

Performs a single raycast and returns the first hit.

**Parameters**:
- `origin` (table): `{x = number, y = number}` - Ray start position
- `direction` (table): `{x = number, y = number}` - Ray direction (normalized)
- `distance` (number): Maximum ray distance

**Returns**: `table` - Hit result with:
  - `actor` (table or nil): The hit actor, or nil if no hit
  - `point` (table): `{x, y}` - Hit point in world space
  - `normal` (table): `{x, y}` - Surface normal at hit point
  - `fraction` (number): Fraction along ray (0.0 to 1.0)

**Example**:
```lua
local origin = {x = 0, y = 0}
local direction = {x = 1, y = 0}  -- Raycast to the right
local hit = Physics.Raycast(origin, direction, 10)

if hit.actor ~= nil then
    Debug.Log("Hit: " .. hit.actor:GetName())
    Debug.Log("Distance: " .. hit.fraction * 10)
else
    Debug.Log("No hit")
end
```

---

### Physics.RaycastAll(origin, direction, distance)

Performs a raycast and returns all hits (sorted by distance).

**Parameters**:
- Same as `Raycast()`

**Returns**: `table` - Array of hit results (1-indexed)

**Example**:
```lua
local hits = Physics.RaycastAll({x = 0, y = 0}, {x = 1, y = 0}, 10)

for i, hit in ipairs(hits) do
    Debug.Log("Hit " .. i .. ": " .. hit.actor:GetName())
end
```

---

## Rigidbody API

The Rigidbody component provides physics simulation via Box2D.

### rigidbody:GetPosition()

Gets the rigidbody's current position.

**Returns**: `table` - `{x = number, y = number}`

**Example**:
```lua
local pos = self.rigidbody:GetPosition()
Debug.Log("Position: " .. pos.x .. ", " .. pos.y)
```

---

### rigidbody:SetPosition(position)

Teleports the rigidbody to a new position.

**Parameters**:
- `position` (table): `{x = number, y = number}`

**Example**:
```lua
self.rigidbody:SetPosition({x = 10, y = 20})
```

---

### rigidbody:GetRotation()

Gets the current rotation in degrees.

**Returns**: `number` - Rotation in degrees (clockwise)

---

### rigidbody:SetRotation(degrees)

Sets the rotation directly.

**Parameters**:
- `degrees` (number): Rotation in degrees (clockwise)

**Example**:
```lua
self.rigidbody:SetRotation(45)  -- Face 45° clockwise
```

---

### rigidbody:GetVelocity()

Gets the current linear velocity.

**Returns**: `table` - `{x = number, y = number}` in units per second

---

### rigidbody:SetVelocity(velocity)

Sets the linear velocity directly.

**Parameters**:
- `velocity` (table): `{x = number, y = number}`

**Example**:
```lua
-- Move right at 5 units/second
self.rigidbody:SetVelocity({x = 5, y = 0})
```

---

### rigidbody:AddForce(force)

Applies a force to the rigidbody (affects dynamic bodies only).

**Parameters**:
- `force` (table): `{x = number, y = number}`

**Example**:
```lua
function Component:OnUpdate()
    if Input.GetKey("space") then
        -- Apply upward force (jump)
        self.rigidbody:AddForce({x = 0, y = -10})
    end
end
```

---

### rigidbody:GetAngularVelocity()

Gets the current angular velocity.

**Returns**: `number` - Angular velocity in degrees per second

---

### rigidbody:SetAngularVelocity(degrees_per_second)

Sets the angular velocity (rotation speed).

**Parameters**:
- `degrees_per_second` (number): Rotation speed

**Example**:
```lua
self.rigidbody:SetAngularVelocity(90)  -- Rotate 90°/sec
```

---

### rigidbody:GetGravityScale()

Gets the gravity scale multiplier.

**Returns**: `number` - Gravity scale (1.0 = normal)

---

### rigidbody:SetGravityScale(scale)

Sets the gravity scale multiplier.

**Parameters**:
- `scale` (number): Gravity scale (1.0 = normal, 0.0 = no gravity, -1.0 = inverted)

**Example**:
```lua
self.rigidbody:SetGravityScale(0.0)  -- Disable gravity
```

---

## Application API

The Application API provides system-level functions.

### Application.Quit()

Quits the application.

**Example**:
```lua
if Input.GetKeyDown("escape") then
    Application.Quit()
end
```

---

### Application.Sleep(milliseconds)

Pauses execution for the specified duration.

**Parameters**:
- `milliseconds` (number): Sleep duration in milliseconds

**Example**:
```lua
Application.Sleep(1000)  -- Sleep for 1 second
```

---

### Application.GetFrame()

Gets the current frame number.

**Returns**: `number` - Frame counter (starts at 0)

**Example**:
```lua
local frame = Application.GetFrame()
if frame % 60 == 0 then
    Debug.Log("60 frames have passed")
end
```

---

### Application.OpenURL(url)

Opens a URL in the default browser.

**Parameters**:
- `url` (string): URL to open

**Example**:
```lua
Application.OpenURL("https://github.com/jackgaff/fr-ocean-engine")
```

---

## Debug API

The Debug API provides logging functionality.

### Debug.Log(message)

Logs a message to stdout.

**Parameters**:
- `message` (string): Message to log

**Example**:
```lua
Debug.Log("Player health: " .. self.health)
```

---

### Debug.LogError(message)

Logs an error message to stderr.

**Parameters**:
- `message` (string): Error message

**Example**:
```lua
if self.health < 0 then
    Debug.LogError("Invalid health value!")
end
```

---

## Component Lifecycle

### Lifecycle Methods

Every Lua component can implement these optional methods:

```lua
Component = {}

-- Called once on the first frame after component creation
function Component:OnStart()
    -- Initialize variables
    -- Get component references
    -- Set up initial state
end

-- Called every frame during the update phase
function Component:OnUpdate()
    -- Game logic
    -- Input handling
    -- Queue rendering (Image.Draw, Text.Draw)
end

-- Called every frame after physics simulation
function Component:OnLateUpdate()
    -- Post-physics logic
    -- Camera updates
    -- Animation state
end

-- Called when the actor or component is destroyed
function Component:OnDestroy()
    -- Cleanup
    -- Save state
end
```

### Physics Collision Callbacks

If the actor has a Rigidbody component, these callbacks are available:

```lua
-- Called when a collision starts
function Component:OnCollisionEnter(other_actor)
    Debug.Log("Collision with: " .. other_actor:GetName())
end

-- Called every frame while colliding
function Component:OnCollisionStay(other_actor)
    -- Apply damage over time, etc.
end

-- Called when a collision ends
function Component:OnCollisionExit(other_actor)
    Debug.Log("Stopped colliding with: " .. other_actor:GetName())
end

-- Called when a trigger overlap starts
function Component:OnTriggerEnter(other_actor)
    Debug.Log("Entered trigger: " .. other_actor:GetName())
end

-- Called every frame while overlapping a trigger
function Component:OnTriggerStay(other_actor)
    -- Powerup zone, damage zone, etc.
end

-- Called when a trigger overlap ends
function Component:OnTriggerExit(other_actor)
    Debug.Log("Left trigger: " .. other_actor:GetName())
end
```

### Component Properties

Components can define properties that are overridable from JSON:

```lua
PlayerController = {
    speed = 5.0,
    jump_force = 10.0,
    max_health = 100
}

function PlayerController:OnStart()
    -- self.speed is accessible and can be overridden in actor templates
    Debug.Log("Speed: " .. self.speed)
end
```

**In actor template JSON**:
```json
{
  "name": "Player",
  "components": {
    "PlayerController": {
      "speed": 8.0,
      "jump_force": 15.0
    }
  }
}
```

---

## Complete Example

Here's a complete example of a player controller component:

```lua
PlayerController = {
    speed = 5.0,
    jump_force = 10.0,
    is_grounded = false
}

function PlayerController:OnStart()
    -- Get rigidbody component
    self.rigidbody = self.actor:GetComponent("Rigidbody")

    if self.rigidbody == nil then
        Debug.LogError("PlayerController requires Rigidbody component!")
    end
end

function PlayerController:OnUpdate()
    -- Horizontal movement
    local velocity = self.rigidbody:GetVelocity()

    if Input.GetKey("left") then
        velocity.x = -self.speed
    elseif Input.GetKey("right") then
        velocity.x = self.speed
    else
        velocity.x = 0
    end

    self.rigidbody:SetVelocity(velocity)

    -- Jump
    if Input.GetKeyDown("space") and self.is_grounded then
        self.rigidbody:AddForce({x = 0, y = -self.jump_force})
        self.is_grounded = false
    end

    -- Draw sprite
    local pos = self.rigidbody:GetPosition()
    Image.Draw("player.png", pos.x, pos.y)
end

function PlayerController:OnCollisionEnter(other)
    -- Check if we landed on the ground
    if other:GetName() == "Ground" then
        self.is_grounded = true
    end
end

function PlayerController:OnCollisionExit(other)
    if other:GetName() == "Ground" then
        self.is_grounded = false
    end
end
```

---

**Last Updated**: 2025-10-28
**Engine Version**: 1.0
**Author**: Jack Gaffney
