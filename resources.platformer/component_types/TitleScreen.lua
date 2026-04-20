-- TitleScreen — big title, pulsing prompt, SPACE to start.

TitleScreen = {
    elapsed = 0,
    float   = 0,
}

function TitleScreen:OnStart()
    self.elapsed = 0
    self.float   = 0

    -- Wipe any persistent GameManager from a previous playthrough so score
    -- starts fresh on each new-game.
    local gm = Actor.Find("GameManager")
    if gm then Actor.Destroy(gm) end
end

function TitleScreen:OnUpdate()
    local dt = Time.GetUnscaledDeltaTime()
    self.elapsed = self.elapsed + dt
    self.float   = self.float   + dt * 1.6

    -- Full-screen gradient via two rectangles.
    Image.DrawRect(0,   0, 800, 300, 14, 22, 40, 255)
    Image.DrawRect(0, 300, 800, 300,  8, 14, 24, 255)

    -- Decorative band.
    Image.DrawRect(0, 275, 800,  50, 38, 120, 200, 90)

    -- Title (fade-in over first 0.6s).
    local a = math.floor(math.min(1.0, self.elapsed / 0.6) * 255)
    local drift = math.sin(self.float) * 4

    -- Shadow
    Text.Draw("FR-OCEAN",   182, 152 + drift, "OpenSans-Regular", 84, 0, 0, 0, math.floor(a * 0.5))
    Text.Draw("FR-OCEAN",   180, 150 + drift, "OpenSans-Regular", 84, 110, 200, 255, a)
    Text.Draw("PLATFORMER", 192, 238 + drift, "OpenSans-Regular", 36, 235, 235, 245, a)

    -- Pulsing prompt.
    local prompt_a = math.floor((math.sin(self.float * 2.4) * 0.5 + 0.5) * 180 + 75)
    Text.Draw("press  SPACE  to  start", 196, 400,
        "OpenSans-Regular", 30, 255, 215, 120, prompt_a)

    Text.Draw("Esc quits",
        345, 460, "OpenSans-Regular", 16, 170, 180, 200, 220)
    Text.Draw("Built on the FR-Ocean Engine",
        245, 560, "OpenSans-Regular", 14, 110, 130, 160, 200)

    if Input.GetKeyDown("space") or Input.GetKeyDown("return") then
        Scene.LoadWithTransition("level1", "fade", 0.5)
    end
    if Input.GetKeyDown("escape") then
        Application.Quit()
    end
end
