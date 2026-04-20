-- Coin — bobbing collectible with particle pop on collect.

Coin = {
    value = 10,
    collected = false,
    bob_offset = 0,
    spin = 0,
}

function Coin:OnStart()
    self.collected  = false
    self.bob_offset = math.random() * 6.28
    self.spin       = 0
    self.rigidbody  = self.actor:GetComponent("Rigidbody")
end

function Coin:OnUpdate()
    if self.collected or not self.rigidbody then return end

    local dt  = Time.GetDeltaTime()
    local pos = self.rigidbody:GetPosition()

    self.bob_offset = self.bob_offset + dt * 3.2
    self.spin       = self.spin + dt * 3.0
    local bob_y  = math.sin(self.bob_offset) * 0.12
    -- Fake rotation via horizontal squash (cos oscillates -1..1)
    local scale  = math.abs(math.cos(self.spin)) * 0.3 + 0.05

    Image.DrawEx("coin", pos.x, pos.y + bob_y, 0,
        scale, 0.3, 0.5, 0.5, 255, 215, 40, 255, 2)
end

function Coin:Collect()
    if self.collected then return end
    self.collected = true

    local pos = self.rigidbody:GetPosition()
    local c = ParticleConfig()
    c.lifetime_min = 0.3
    c.lifetime_max = 0.7
    c.speed_min    = 2.0
    c.speed_max    = 5.0
    c.spread_angle = 360
    c.gravity      = 6.0
    c.start_size   = 12
    c.end_size     = 2
    c:SetStartColor(255, 230, 80, 255)
    c:SetEndColor(255, 150, 40, 0)
    Particles.Emit(pos.x, pos.y, 18, c)

    Event.Emit("coin_collected", { value = self.value })
    Actor.Destroy(self.actor)
end
