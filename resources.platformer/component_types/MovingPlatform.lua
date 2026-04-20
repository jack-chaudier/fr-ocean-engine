-- MovingPlatform — kinematic horizontal patrol.

MovingPlatform = {
    move_speed     = 2.0,
    move_distance  = 3.0,
    platform_width = 2.0,
    rigidbody = nil,
    start_x   = 0,
    direction = 1,
}

function MovingPlatform:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    if not self.rigidbody then return end
    self.start_x   = self.rigidbody:GetPosition().x
    self.direction = 1
    if self.rigidbody.width and self.rigidbody.width > 0 then
        self.platform_width = self.rigidbody.width
    end
end

function MovingPlatform:OnUpdate()
    if not self.rigidbody then return end
    local pos = self.rigidbody:GetPosition()

    local offset = pos.x - self.start_x
    if offset > self.move_distance then
        self.direction = -1
    elseif offset < -self.move_distance then
        self.direction = 1
    end
    self.rigidbody:SetVelocity(Vector2(self.move_speed * self.direction, 0))

    Image.DrawEx("platform_moving", pos.x, pos.y, 0,
        self.platform_width, 0.5, 0.5, 0.5, 255, 255, 255, 255, 0)
end
