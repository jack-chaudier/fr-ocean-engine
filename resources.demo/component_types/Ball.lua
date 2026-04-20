-- Ball — a dynamic rigidbody bouncing between invisible walls.

Ball = {}

function Ball:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
    if self.rb then
        self.rb:SetVelocity(Vector2(3, -4))
    end
end

function Ball:OnUpdate()
    if not self.rb then return end
    local p = self.rb:GetPosition()

    -- Soft world bounds (reflect velocity)
    local v = self.rb:GetVelocity()
    if p.x < -4 and v.x < 0 then self.rb:SetVelocity(Vector2(-v.x, v.y)) end
    if p.x >  4 and v.x > 0 then self.rb:SetVelocity(Vector2(-v.x, v.y)) end
    if p.y >  2 and v.y > 0 then self.rb:SetVelocity(Vector2(v.x, -math.abs(v.y) * 0.85)) end

    Image.DrawEx("ball", p.x, p.y, 0, 0.4, 0.4, 0.5, 0.5, 255, 255, 255, 255, 2)
end
