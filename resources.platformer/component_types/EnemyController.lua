-- EnemyController — patrolling foe, dies via stomp with a poof.

local NATURAL_WU = 64 * 0.01
local ENEMY_W = 0.55   -- drawn world-space width

EnemyController = {
    patrol_speed    = 1.6,
    patrol_distance = 2.0,
    rb = nil,
    start_x   = 0,
    direction = 1,
    dead = false,
    bob  = 0,
}

function EnemyController:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
    if not self.rb then return end
    self.start_x   = self.rb:GetPosition().x
    self.direction = 1
    self.dead = false
    self.bob  = math.random() * 6.28
end

function EnemyController:OnUpdate()
    if self.dead or not self.rb then return end

    local dt  = Time.GetDeltaTime()
    local pos = self.rb:GetPosition()

    local offset = pos.x - self.start_x
    if offset >  self.patrol_distance then self.direction = -1
    elseif offset < -self.patrol_distance then self.direction =  1 end
    self.rb:SetVelocity(Vector2(self.patrol_speed * self.direction, 0))

    self.bob = self.bob + dt * 6
    local bob_y = math.sin(self.bob) * 0.04

    local sx = (self.direction >= 0 and 1 or -1) * ENEMY_W / NATURAL_WU
    local sy = ENEMY_W / NATURAL_WU
    Image.DrawEx("enemy", pos.x, pos.y + bob_y, 0,
        sx, sy, 0.5, 0.5, 255, 255, 255, 255, 3)
end

function EnemyController:Die()
    if self.dead then return end
    self.dead = true
    local pos = self.rb:GetPosition()

    local c = ParticleConfig()
    c.lifetime_min = 0.3
    c.lifetime_max = 0.7
    c.speed_min    = 2.0
    c.speed_max    = 5.5
    c.spread_angle = 360
    c.gravity      = 8
    c.start_size   = 14
    c.end_size     = 2
    c:SetStartColor(220, 70, 70, 255)
    c:SetEndColor(120, 30, 30, 0)
    Particles.Emit(pos.x, pos.y, 22, c)

    Event.Emit("enemy_killed", { points = 50 })
    Actor.Destroy(self.actor)
end
