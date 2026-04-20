-- Showcase — kitchen-sink scene exercising every engine subsystem.

Showcase = {
    burst_cooldown = 0,
}

local function burst_config(r, g, b)
    local c = ParticleConfig()
    c.lifetime_min = 0.35
    c.lifetime_max = 0.9
    c.speed_min    = 2
    c.speed_max    = 6
    c.spread_angle = 360
    c.gravity      = 6
    c.start_size   = 14
    c.end_size     = 2
    c.image_name   = "spark"
    c:SetStartColor(r, g, b, 255)
    c:SetEndColor(r * 0.6, g * 0.6, b * 0.6, 0)
    return c
end

function Showcase:OnStart()
    self.burst_cooldown = 0

    -- Repeating heartbeat emission on the left (first beat after 0.3s, then every 0.8s).
    Timer.Every(0.3, 0.8, function()
        Particles.Emit(-3.5, 1.5, 12, burst_config(120, 220, 255))
    end)
end

function Showcase:OnUpdate()
    local t = Time.GetTotalTime()

    -- Bouncing "mover" dot traced via cheap sine — no physics required.
    local mx = 3.5 + math.sin(t * 2.2) * 0.6
    local my = 1.5 + math.cos(t * 3.1) * 0.4
    Image.DrawEx("mover", mx, my, 0, 0.5, 0.5, 0.5, 0.5, 255, 255, 255, 255, 3)

    -- HUD
    Text.Draw("FR-Ocean Feature Demo", 20, 14,  "OpenSans-Regular", 28, 240, 240, 245, 255)
    Text.Draw("Particles   Tween   Timer   Text   Events   Physics",
        20, 48, "OpenSans-Regular", 14, 160, 170, 190, 255)

    Text.Draw("SPACE  burst",      20, 500, "OpenSans-Regular", 14, 200, 200, 220, 255)
    Text.Draw("MOUSE  spawn spark",200, 500, "OpenSans-Regular", 14, 200, 200, 220, 255)
    Text.Draw("ESC    quit",       420, 500, "OpenSans-Regular", 14, 200, 200, 220, 255)

    if Input.GetKeyDown("space") then
        Particles.Emit(0, 1, 60, burst_config(255, 220, 90))
        Camera.Shake(2.0, 0.18)
        Event.Emit("user_burst", {})
    end

    if Input.GetMouseButtonDown(1) then
        local mp = Input.GetMousePosition()
        -- convert screen → world (assume camera at 0, zoom=1, 100 px/unit, 960x540)
        local wx = (mp.x - 480) / 100.0
        local wy = (mp.y - 270) / 100.0
        Particles.Emit(wx, wy, 24, burst_config(180, 255, 180))
    end

    if Input.GetKeyDown("escape") then
        Application.Quit()
    end

    -- Active particle count readout
    Text.Draw("particles: " .. Particles.GetActiveCount(),
        780, 14, "OpenSans-Regular", 14, 120, 200, 120, 255)
end
