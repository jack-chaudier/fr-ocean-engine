-- Coin — bobbing collectible with particle pop on collect.

local NATURAL_WU = 64 * 0.01
local COIN_W = 0.45  -- drawn world-space diameter

Coin = { value = 10, collected = false, bob = 0, spin = 0 }

function Coin:OnStart()
    self.collected = false
    self.bob  = math.random() * 6.28
    self.spin = 0
    self.rb   = self.actor:GetComponent("Rigidbody")
end

function Coin:OnUpdate()
    if self.collected or not self.rb then return end
    local dt  = Time.GetDeltaTime()
    local pos = self.rb:GetPosition()

    self.bob  = self.bob  + dt * 3.2
    self.spin = self.spin + dt * 3.0
    local bob_y    = math.sin(self.bob) * 0.12
    local spin_sq  = math.abs(math.cos(self.spin)) * 0.8 + 0.2   -- 0.2..1.0

    local base = COIN_W / NATURAL_WU
    Image.DrawEx("coin", pos.x, pos.y + bob_y, 0,
        base * spin_sq, base, 0.5, 0.5, 255, 215, 40, 255, 2)
end

function Coin:Collect()
    if self.collected then return end
    self.collected = true

    local pos = self.rb:GetPosition()
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
