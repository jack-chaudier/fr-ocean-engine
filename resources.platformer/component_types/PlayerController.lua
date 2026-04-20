-- PlayerController — movement, jumping, juicy feedback.

local NATURAL_WU = 64 * 0.01  -- Sprite is 64 px = 0.64 world units at scale 1.

PlayerController = {
    move_speed     = 5.5,
    jump_force     = 9.0,
    max_fall_speed = 14.0,
    coyote_time    = 0.10,
    jump_buffer    = 0.10,

    rb = nil,
    facing_right     = true,
    is_grounded      = false,
    time_ungrounded  = 1.0,
    jump_buffered_at = -1.0,
    dead = false,
    hit_flash = 0,
    death_t   = 0,
}

local function burst(r1, g1, b1, r2, g2, b2)
    local c = ParticleConfig()
    c.lifetime_min = 0.25
    c.lifetime_max = 0.55
    c.speed_min    = 1.5
    c.speed_max    = 4.5
    c.spread_angle = 120
    c.direction    = 270
    c.gravity      = 10
    c.start_size   = 10
    c.end_size     = 2
    c:SetStartColor(r1, g1, b1, 255)
    c:SetEndColor(r2, g2, b2, 0)
    return c
end

function PlayerController:OnStart()
    self.rb              = self.actor:GetComponent("Rigidbody")
    self.facing_right    = true
    self.is_grounded     = false
    self.time_ungrounded = 1.0
    self.jump_buffered_at= -1.0
    self.dead            = false
    self.hit_flash       = 0
    self.death_t         = 0
end

function PlayerController:DrawDeath()
    if not self.rb then return end
    local udt = Time.GetUnscaledDeltaTime()
    self.death_t = self.death_t + udt

    local pos  = self.rb:GetPosition()
    local age  = self.death_t
    local rot  = age * 720              -- spin 2 rev/sec
    local sx   = (self.facing_right and 1 or -1) * 0.6 / NATURAL_WU
    local sy   = 0.8 / NATURAL_WU
    local alpha = math.max(0, math.floor(255 - age * 400))

    Image.DrawEx("player", pos.x, pos.y, rot, sx, sy, 0.5, 0.5,
        255, 110, 110, alpha, 5)

    -- Red full-screen flash, fades over ~0.5s of wall time.
    local flash = math.max(0, math.floor(180 - age * 360))
    if flash > 0 then
        Image.DrawRect(0, 0, 800, 600, 210, 40, 40, flash)
    end

    -- Big "OUCH" text briefly.
    if age < 0.6 then
        local oa = math.floor(math.max(0, 255 - age * 340))
        Text.Draw("OUCH!", 322, 240, "OpenSans-Regular", 80, 255, 220, 220, oa)
    end
end

function PlayerController:OnUpdate()
    if not self.rb then return end
    if self.dead then
        self:DrawDeath()
        return
    end

    local dt  = Time.GetDeltaTime()
    local vel = self.rb:GetVelocity()
    local pos = self.rb:GetPosition()

    if self.hit_flash > 0 then
        self.hit_flash = math.max(0, self.hit_flash - dt * 3.5)
    end

    -- Grounded check
    local hit = Physics.Raycast(Vector2(pos.x, pos.y + 0.25), Vector2(0, 1), 0.18)
    local was_grounded = self.is_grounded
    self.is_grounded = (hit ~= nil and hit.actor ~= nil and not hit.is_trigger)
    if self.is_grounded then
        self.time_ungrounded = 0.0
        if not was_grounded then
            Particles.Emit(pos.x, pos.y + 0.3, 6, burst(200, 200, 200, 150, 150, 150))
        end
    else
        self.time_ungrounded = self.time_ungrounded + dt
    end

    -- Horizontal input
    local move_x = 0
    if Input.GetKey("left")  or Input.GetKey("a") then
        move_x = -self.move_speed; self.facing_right = false
    elseif Input.GetKey("right") or Input.GetKey("d") then
        move_x =  self.move_speed; self.facing_right = true
    end

    local vy = vel.y
    if vy > self.max_fall_speed then vy = self.max_fall_speed end
    self.rb:SetVelocity(Vector2(move_x, vy))

    -- Jump buffering + coyote time
    if Input.GetKeyDown("space") or Input.GetKeyDown("up") or Input.GetKeyDown("w") then
        self.jump_buffered_at = Time.GetTotalTime()
    end
    local now = Time.GetTotalTime()
    local can_jump = (self.time_ungrounded <= self.coyote_time)
        and (self.jump_buffered_at >= 0 and (now - self.jump_buffered_at) <= self.jump_buffer)

    if can_jump then
        self.rb:SetVelocity(Vector2(move_x, -self.jump_force))
        self.is_grounded = false
        self.jump_buffered_at = -1.0
        self.time_ungrounded  = self.coyote_time + 1.0
        Particles.Emit(pos.x, pos.y + 0.3, 8, burst(240, 240, 255, 180, 180, 220))
    end

    -- Variable jump height: early release caps ascent.
    local holding = Input.GetKey("space") or Input.GetKey("up") or Input.GetKey("w")
    if vy < -2.5 and not holding then
        self.rb:SetVelocity(Vector2(move_x, vy * 0.5))
    end

    -- Draw. Sprite is 64 px; scale so 1 world unit corresponds to scale = 1/NATURAL_WU.
    -- Player collider is 0.4w x 0.6h; visual matches the body silhouette.
    local draw_sx = (self.facing_right and 1 or -1) * 0.6 / NATURAL_WU
    local draw_sy = 0.8 / NATURAL_WU
    -- hit flash: bright red tint for a brief moment
    local r, g, b = 255, 255, 255
    if self.hit_flash > 0 then
        local k = self.hit_flash
        r = 255
        g = math.floor(255 * (1 - k * 0.9))
        b = math.floor(255 * (1 - k * 0.9))
    end
    Image.DrawEx("player", pos.x, pos.y, 0, draw_sx, draw_sy, 0.5, 0.5,
        r, g, b, 255, 5)

    if pos.y > 12 then self:Die() end
end

function PlayerController:Die()
    if self.dead then return end
    self.dead    = true
    self.death_t = 0

    local pos = self.rb:GetPosition()
    Particles.Emit(pos.x, pos.y, 50, burst(255, 120, 120, 120, 40, 40))
    Camera.Shake(8.0, 0.45)
    Time.SetTimeScale(0.25)
    Event.Emit("player_died", {})

    -- Dramatic pop: fling the body up and spin via a raw velocity set,
    -- then let physics take over for the rest of the death animation.
    if self.rb then
        self.rb:SetVelocity(Vector2(0, -6))
        self.rb:SetGravityScale(2.0)
    end

    -- Reload scene after a short slo-mo beat (Scheduler uses scaled dt,
    -- so 0.15s here is ~0.6s of wall time at time-scale 0.25).
    Timer.After(0.15, function()
        Time.SetTimeScale(1.0)
        Scene.LoadWithTransition(Scene.GetCurrent(), "fade", 0.35)
    end)
end

function PlayerController:OnTriggerEnter(collision)
    if self.dead or not collision or not collision.other then return end
    local other = collision.other
    local name  = other:GetName()

    if name == "Enemy" then
        local my_pos    = self.rb:GetPosition()
        local enemy_rb  = other:GetComponent("Rigidbody")
        if enemy_rb == nil then return end
        local enemy_pos = enemy_rb:GetPosition()

        if my_pos.y < enemy_pos.y - 0.15 then
            local enemy_ctrl = other:GetComponent("EnemyController")
            if enemy_ctrl then enemy_ctrl:Die() end
            self.rb:SetVelocity(Vector2(0, -6.5))
            Camera.Shake(3.0, 0.12)
        else
            self.hit_flash = 1.0
            self:Die()
        end
    elseif name == "Coin" then
        local c = other:GetComponent("Coin")
        if c then c:Collect() end
    elseif name == "Spike" then
        self.hit_flash = 1.0
        self:Die()
    elseif name == "Flag" then
        local f = other:GetComponent("Flag")
        if f then f:Reach() end
    end
end
