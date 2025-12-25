-- EnemyController.lua
-- Controls enemy movement and shooting

EnemyController = {
    speed = 2.0,
    health = 1,
    fire_rate = 2.0,
    fire_cooldown = 0,
    rigidbody = nil,
    direction = 1
}

function EnemyController:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    self.fire_cooldown = math.random() * self.fire_rate
    self.direction = 1
    self.rigidbody:SetVelocity(Vector2(self.speed * self.direction, 0.5))
end

function EnemyController:OnUpdate()
    local pos = self.rigidbody:GetPosition()

    -- Bounce off walls
    if pos.x > 3.5 then
        self.direction = -1
        self.rigidbody:SetVelocity(Vector2(self.speed * self.direction, 0.5))
    elseif pos.x < -3.5 then
        self.direction = 1
        self.rigidbody:SetVelocity(Vector2(self.speed * self.direction, 0.5))
    end

    -- Remove if off screen
    if pos.y > 4 then
        Actor.Destroy(self.actor)
        return
    end

    -- Shooting
    self.fire_cooldown = self.fire_cooldown - (1/60)
    if self.fire_cooldown <= 0 then
        self:Fire()
        self.fire_cooldown = self.fire_rate
    end

    -- Draw enemy sprite
    Image.DrawEx("enemy", pos.x, pos.y, 0, 1, 1, 0.5, 0.5, 255, 255, 255, 255, 0)
end

function EnemyController:Fire()
    local pos = self.rigidbody:GetPosition()
    local bullet = Actor.Instantiate("enemy_bullet")
    local bullet_rb = bullet:GetComponent("Rigidbody")
    bullet_rb:SetPosition(Vector2(pos.x, pos.y + 0.5))
end

function EnemyController:TakeDamage(damage)
    self.health = self.health - damage
    if self.health <= 0 then
        -- Add score
        local game_manager = Actor.Find("GameManager")
        if game_manager ~= nil then
            local sm = game_manager:GetComponent("ScoreManager")
            if sm ~= nil then
                sm:AddScore(100)
            end
        end
        Actor.Destroy(self.actor)
    end
end

function EnemyController:OnTriggerEnter(other)
    if other:GetName() == "Bullet" then
        self:TakeDamage(1)
    end
end
