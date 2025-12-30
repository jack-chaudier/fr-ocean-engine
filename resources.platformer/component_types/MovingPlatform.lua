-- MovingPlatform.lua
-- Platform that moves back and forth

MovingPlatform = {
    move_speed = 2.0,
    move_distance = 3.0,
    rigidbody = nil,
    start_x = 0,
    direction = 1
}

function MovingPlatform:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    local pos = self.rigidbody:GetPosition()
    self.start_x = pos.x
    self.direction = 1
end

function MovingPlatform:OnUpdate()
    local pos = self.rigidbody:GetPosition()

    -- Calculate movement
    local offset = pos.x - self.start_x
    if offset > self.move_distance then
        self.direction = -1
    elseif offset < -self.move_distance then
        self.direction = 1
    end
    self.rigidbody:SetVelocity(Vector2(self.move_speed * self.direction, 0))

    -- Draw platform
    Image.DrawEx("platform", pos.x, pos.y, 0, 2, 0.5, 0.5, 0.5, 100, 150, 200, 255, 0)
end
