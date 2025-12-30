-- EnemyController.lua
-- Simple patrolling enemy

EnemyController = {
    patrol_speed = 1.5,
    patrol_distance = 2.0,
    rigidbody = nil,
    start_x = 0,
    direction = 1,
    dead = false
}

function EnemyController:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    local pos = self.rigidbody:GetPosition()
    self.start_x = pos.x
    self.direction = 1
    self.dead = false
end

function EnemyController:OnUpdate()
    if self.dead then return end

    local pos = self.rigidbody:GetPosition()

    -- Patrol movement
    local offset = pos.x - self.start_x
    if offset > self.patrol_distance then
        self.direction = -1
    elseif offset < -self.patrol_distance then
        self.direction = 1
    end
    self.rigidbody:SetVelocity(Vector2(self.patrol_speed * self.direction, 0))

    -- Draw enemy (red)
    local scale_x = 1
    if self.direction < 0 then
        scale_x = -1
    end
    Image.DrawEx("enemy", pos.x, pos.y, 0, scale_x * 0.4, 0.4, 0.5, 0.5, 255, 100, 100, 255, 3)
end

function EnemyController:Die()
    self.dead = true
    Event.Emit("enemy_killed", { points = 50 })
    Actor.Destroy(self.actor)
end
