-- Flag — level goal. Debounced so it can't fire twice.

local NATURAL_WU = 64 * 0.01
local FLAG_W = 0.6

Flag = {
    next_scene = "level2",
    reached = false,
    bob = 0,
}

function Flag:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
    self.reached = false
    self.bob = 0
end

function Flag:OnUpdate()
    if not self.rb then return end
    local dt  = Time.GetDeltaTime()
    local pos = self.rb:GetPosition()
    self.bob = self.bob + dt * 4
    local wave = math.sin(self.bob) * 0.04

    local s = FLAG_W / NATURAL_WU
    Image.DrawEx("flag", pos.x, pos.y + wave, 0,
        s, s * 1.2, 0.5, 0.5, 255, 255, 255, 255, 1)
end

function Flag:Reach()
    if self.reached then return end
    self.reached = true
    local pos = self.rb:GetPosition()

    local c = ParticleConfig()
    c.lifetime_min = 0.4
    c.lifetime_max = 0.9
    c.speed_min    = 2.0
    c.speed_max    = 5.0
    c.spread_angle = 360
    c.gravity      = 3
    c.start_size   = 12
    c.end_size     = 2
    c:SetStartColor(255, 230, 100, 255)
    c:SetEndColor(255, 120, 60, 0)
    Particles.Emit(pos.x, pos.y, 40, c)

    Event.Emit("level_cleared", { next = self.next_scene })
    Scene.LoadWithTransition(self.next_scene, "fade", 0.5)
end
