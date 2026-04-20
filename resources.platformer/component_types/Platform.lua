-- Platform — static ground. Sprite is sized to exactly match the collider.

-- Sprites are 64 px = 0.64 world units at scale 1, so the scale factor
-- to get W world units is W / 0.64.
local WU_PER_PIXEL = 0.01
local NATURAL_WU   = 64 * WU_PER_PIXEL

Platform = {}

function Platform:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
end

function Platform:OnUpdate()
    if not self.rb then return end
    local pos = self.rb:GetPosition()
    local sx  = (self.rb.width  or 2.0) / NATURAL_WU
    local sy  = (self.rb.height or 0.5) / NATURAL_WU
    Image.DrawEx("platform", pos.x, pos.y, 0, sx, sy, 0.5, 0.5, 255, 255, 255, 255, 0)
end
