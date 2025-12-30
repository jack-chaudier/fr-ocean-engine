-- Platform.lua
-- Static platform rendering

Platform = {
    platform_width = 2.0
}

function Platform:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
end

function Platform:OnUpdate()
    local pos = self.rigidbody:GetPosition()
    -- Draw platform (brown rectangle)
    Image.DrawEx("platform", pos.x, pos.y, 0, self.platform_width, 0.5, 0.5, 0.5, 139, 90, 43, 255, 0)
end
