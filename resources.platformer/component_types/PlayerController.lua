-- PlayerController — movement, jumping, juice.

PlayerController = {
    move_speed     = 5.5,
    jump_force     = 9.0,
    max_fall_speed = 14.0,
    coyote_time    = 0.10,  -- jump allowed this many seconds after leaving ground
    jump_buffer    = 0.10,  -- jump pressed this many seconds before landing still registers

    rigidbody    = nil,
    facing_right = true,
    is_grounded  = false,
    time_ungrounded  = 1.0,
    jump_buffered_at = -1.0,
    dead = false,
}

local function new_particle_config(r1, g1, b1, r2, g2, b2, count_speed)
    local c = ParticleConfig()
    c.lifetime_min = 0.25
    c.lifetime_max = 0.55
    c.speed_min    = 1.5
    c.speed_max    = 4.5
    c.spread_angle = 120
    c.direction    = 270  -- up
    c.gravity      = 10
    c.start_size   = 10
    c.end_size     = 2
    c:SetStartColor(r1, g1, b1, 255)
    c:SetEndColor(r2, g2, b2, 0)
    return c
end

function PlayerController:OnStart()
    self.rigidbody        = self.actor:GetComponent("Rigidbody")
    self.facing_right     = true
    self.is_grounded      = false
    self.time_ungrounded  = 1.0
    self.jump_buffered_at = -1.0
    self.dead             = false
end

function PlayerController:OnUpdate()
    if self.dead or not self.rigidbody then return end

    local dt  = Time.GetDeltaTime()
    local vel = self.rigidbody:GetVelocity()
    local pos = self.rigidbody:GetPosition()

    -- Grounded check (short ray past feet)
    local ray_origin = Vector2(pos.x, pos.y + 0.25)
    local hit = Physics.Raycast(ray_origin, Vector2(0, 1), 0.18)
    local was_grounded = self.is_grounded
    self.is_grounded = (hit ~= nil and hit.actor ~= nil and not hit.is_trigger)
    if self.is_grounded then
        self.time_ungrounded = 0.0
        if not was_grounded then
            -- Land dust
            Particles.Emit(pos.x, pos.y + 0.3, 6,
                new_particle_config(200, 200, 200, 150, 150, 150))
        end
    else
        self.time_ungrounded = self.time_ungrounded + dt
    end

    -- Horizontal input
    local move_x = 0
    if Input.GetKey("left") or Input.GetKey("a") then
        move_x = -self.move_speed
        self.facing_right = false
    elseif Input.GetKey("right") or Input.GetKey("d") then
        move_x = self.move_speed
        self.facing_right = true
    end

    -- Clamp fall speed
    local vy = vel.y
    if vy > self.max_fall_speed then vy = self.max_fall_speed end
    self.rigidbody:SetVelocity(Vector2(move_x, vy))

    -- Jump buffering: remember press a brief moment so landing-then-press works
    if Input.GetKeyDown("space") or Input.GetKeyDown("up") or Input.GetKeyDown("w") then
        self.jump_buffered_at = Time.GetTotalTime()
    end

    local now = Time.GetTotalTime()
    local can_jump = (self.time_ungrounded <= self.coyote_time)
        and (self.jump_buffered_at >= 0 and (now - self.jump_buffered_at) <= self.jump_buffer)

    if can_jump then
        self.rigidbody:SetVelocity(Vector2(move_x, -self.jump_force))
        self.is_grounded = false
        self.jump_buffered_at = -1.0
        self.time_ungrounded = self.coyote_time + 1.0
        Particles.Emit(pos.x, pos.y + 0.3, 8,
            new_particle_config(240, 240, 255, 180, 180, 220))
    end

    -- Variable jump height: releasing the jump button while rising caps ascent
    local holding_jump = Input.GetKey("space") or Input.GetKey("up") or Input.GetKey("w")
    if vy < -2.5 and not holding_jump then
        self.rigidbody:SetVelocity(Vector2(move_x, vy * 0.5))
    end

    -- Draw player (flip via negative scale_x)
    local sx = self.facing_right and 0.5 or -0.5
    Image.DrawEx("player", pos.x, pos.y, 0, sx, 0.5, 0.5, 0.5, 255, 255, 255, 255, 5)

    -- Fell off map
    if pos.y > 12 then self:Die() end
end

function PlayerController:Die()
    if self.dead then return end
    self.dead = true

    local pos = self.rigidbody:GetPosition()
    Particles.Emit(pos.x, pos.y, 30,
        new_particle_config(255, 120, 120, 120, 40, 40))
    Camera.Shake(6.0, 0.35)
    Event.Emit("player_died", {})

    -- Small delay before reload so the splash is visible.
    Timer.After(0.5, function()
        Scene.LoadWithTransition(Scene.GetCurrent(), "fade", 0.35)
    end)
end

function PlayerController:OnTriggerEnter(collision)
    if self.dead or not collision or not collision.other then return end
    local other = collision.other
    local name  = other:GetName()

    if name == "Enemy" then
        local my_pos    = self.rigidbody:GetPosition()
        local enemy_rb  = other:GetComponent("Rigidbody")
        if enemy_rb == nil then return end
        local enemy_pos = enemy_rb:GetPosition()

        if my_pos.y < enemy_pos.y - 0.15 then
            local enemy_ctrl = other:GetComponent("EnemyController")
            if enemy_ctrl then enemy_ctrl:Die() end
            self.rigidbody:SetVelocity(Vector2(0, -6.5))
            Camera.Shake(3.0, 0.12)
        else
            self:Die()
        end
    elseif name == "Coin" then
        local c = other:GetComponent("Coin")
        if c then c:Collect() end
    elseif name == "Spike" then
        self:Die()
    elseif name == "Flag" then
        local f = other:GetComponent("Flag")
        if f then f:Reach() end
    end
end
