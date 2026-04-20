-- MovingPlatform — kinematic horizontal patrol, sprite matches collider.

local NATURAL_WU = 64 * 0.01

MovingPlatform = {
    move_speed    = 2.0,
    move_distance = 3.0,
    rb = nil,
    start_x   = 0,
    direction = 1,
}

function MovingPlatform:OnStart()
    self.rb = self.actor:GetComponent("Rigidbody")
    if not self.rb then return end
    self.start_x   = self.rb:GetPosition().x
    self.direction = 1
end

function MovingPlatform:OnUpdate()
    if not self.rb then return end
    local pos = self.rb:GetPosition()

    local offset = pos.x - self.start_x
    if offset >  self.move_distance then self.direction = -1
    elseif offset < -self.move_distance then self.direction =  1 end
    self.rb:SetVelocity(Vector2(self.move_speed * self.direction, 0))

    local sx = (self.rb.width  or 2.0) / NATURAL_WU
    local sy = (self.rb.height or 0.5) / NATURAL_WU
    Image.DrawEx("platform_moving", pos.x, pos.y, 0, sx, sy, 0.5, 0.5,
        255, 255, 255, 255, 0)
end
