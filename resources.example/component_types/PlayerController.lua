-- PlayerController.lua
-- Controls player movement and shooting

PlayerController = {
    speed = 5.0,
    fire_rate = 0.2,
    fire_cooldown = 0,
    rigidbody = nil
}

function PlayerController:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    self.fire_cooldown = 0
end

function PlayerController:OnUpdate()
    -- Movement
    local vel_x = 0
    local vel_y = 0

    if Input.GetKey("left") or Input.GetKey("a") then
        vel_x = -self.speed
    elseif Input.GetKey("right") or Input.GetKey("d") then
        vel_x = self.speed
    end

    if Input.GetKey("up") or Input.GetKey("w") then
        vel_y = -self.speed
    elseif Input.GetKey("down") or Input.GetKey("s") then
        vel_y = self.speed
    end

    self.rigidbody:SetVelocity(Vector2(vel_x, vel_y))

    -- Clamp position to screen bounds
    local pos = self.rigidbody:GetPosition()
    local clamped_x = math.max(-3.5, math.min(3.5, pos.x))
    local clamped_y = math.max(-2.5, math.min(2.5, pos.y))
    if pos.x ~= clamped_x or pos.y ~= clamped_y then
        self.rigidbody:SetPosition(Vector2(clamped_x, clamped_y))
    end

    -- Shooting
    self.fire_cooldown = self.fire_cooldown - (1/60)
    if (Input.GetKey("space") or Input.GetMouseButton(1)) and self.fire_cooldown <= 0 then
        self:Fire()
        self.fire_cooldown = self.fire_rate
    end

    -- Draw player sprite
    Image.DrawEx("player", pos.x, pos.y, 0, 1, 1, 0.5, 0.5, 255, 255, 255, 255, 0)
end

function PlayerController:Fire()
    local pos = self.rigidbody:GetPosition()
    local bullet = Actor.Instantiate("bullet")
    local bullet_rb = bullet:GetComponent("Rigidbody")
    bullet_rb:SetPosition(Vector2(pos.x, pos.y - 0.5))
end

function PlayerController:OnTriggerEnter(other)
    if other:GetName() == "EnemyBullet" or other:GetName() == "Enemy" then
        -- Player hit - game over
        local game_manager = Actor.Find("GameManager")
        if game_manager ~= nil then
            local gm = game_manager:GetComponent("GameManager")
            if gm ~= nil then
                gm:GameOver()
            end
        end
        Actor.Destroy(self.actor)
    end
end
