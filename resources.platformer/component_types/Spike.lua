-- Spike — hazard. Sprite matches the trigger volume.

local NATURAL_WU = 64 * 0.01

Spike = {}

function Spike:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
end

function Spike:OnUpdate()
    if not self.rb then return end
    local pos = self.rb:GetPosition()
    local sx  = (self.rb.trigger_width  or 1.0) / NATURAL_WU
    local sy  = (self.rb.trigger_height or 0.5) / NATURAL_WU
    Image.DrawEx("spike", pos.x, pos.y, 0, sx, sy, 0.5, 0.5,
        255, 255, 255, 255, 1)
end
