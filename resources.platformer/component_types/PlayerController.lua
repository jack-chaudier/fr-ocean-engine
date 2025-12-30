-- PlayerController.lua
-- Handles player movement, jumping, and collision

PlayerController = {
    move_speed = 5.0,
    jump_force = 8.0,
    max_fall_speed = 12.0,
    rigidbody = nil,
    is_grounded = false,
    facing_right = true
}

function PlayerController:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    self.is_grounded = false
    self.facing_right = true
end

function PlayerController:OnUpdate()
    local dt = Time.GetDeltaTime()
    local vel = self.rigidbody:GetVelocity()
    local pos = self.rigidbody:GetPosition()

    -- Ground check via raycast (start inside player, cast past feet)
    -- Player half-height is 0.3, so start at 0.25 (inside) and cast 0.15 (ends at 0.4, past feet)
    local ray_origin = Vector2(pos.x, pos.y + 0.25)
    local ray_dir = Vector2(0, 1)
    local hit = Physics.Raycast(ray_origin, ray_dir, 0.15)
    self.is_grounded = (hit ~= nil and hit.actor ~= nil and not hit.is_trigger)

    -- Horizontal movement
    local move_x = 0
    if Input.GetKey("left") or Input.GetKey("a") then
        move_x = -self.move_speed
        self.facing_right = false
    elseif Input.GetKey("right") or Input.GetKey("d") then
        move_x = self.move_speed
        self.facing_right = true
    end

    -- Clamp fall speed
    local clamped_vel_y = vel.y
    if clamped_vel_y > self.max_fall_speed then
        clamped_vel_y = self.max_fall_speed
    end

    self.rigidbody:SetVelocity(Vector2(move_x, clamped_vel_y))

    -- Jumping (set velocity directly for consistent jump height)
    if (Input.GetKeyDown("space") or Input.GetKeyDown("up") or Input.GetKeyDown("w")) and self.is_grounded then
        self.rigidbody:SetVelocity(Vector2(move_x, -self.jump_force))
        self.is_grounded = false
    end

    -- Draw player
    local scale_x = 1
    if not self.facing_right then
        scale_x = -1
    end
    Image.DrawEx("player", pos.x, pos.y, 0, scale_x * 0.5, 0.5, 0.5, 0.5, 255, 255, 255, 255, 5)

    -- Death by falling
    if pos.y > 10 then
        self:Die()
    end
end

function PlayerController:Die()
    Event.Emit("player_died", {})
    Scene.Load(Scene.GetCurrent())
end

function PlayerController:OnTriggerEnter(collision)
    local other = collision.other
    if other:GetName() == "Enemy" then
        local my_pos = self.rigidbody:GetPosition()
        local enemy_rb = other:GetComponent("Rigidbody")
        if enemy_rb ~= nil then
            local enemy_pos = enemy_rb:GetPosition()
            if my_pos.y < enemy_pos.y - 0.2 then
                -- Stomp enemy
                local enemy_ctrl = other:GetComponent("EnemyController")
                if enemy_ctrl ~= nil then
                    enemy_ctrl:Die()
                end
                -- Bounce
                self.rigidbody:SetVelocity(Vector2(0, -6))
            else
                -- Hit by enemy
                self:Die()
            end
        end
    elseif other:GetName() == "Coin" then
        local coin = other:GetComponent("Coin")
        if coin ~= nil then
            coin:Collect()
        end
    end
end
