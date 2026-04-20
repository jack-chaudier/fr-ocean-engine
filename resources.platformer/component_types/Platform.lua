-- Platform — static ground, grass-topped.

Platform = {
    platform_width = 2.0,
}

function Platform:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    if not self.rigidbody then return end
    if self.rigidbody.width and self.rigidbody.width > 0 then
        self.platform_width = self.rigidbody.width
    end
end

function Platform:OnUpdate()
    if not self.rigidbody then return end
    local pos = self.rigidbody:GetPosition()
    Image.DrawEx("platform", pos.x, pos.y, 0,
        self.platform_width, 0.5, 0.5, 0.5, 255, 255, 255, 255, 0)
end
