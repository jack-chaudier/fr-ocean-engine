-- CameraFollow.lua
-- Smooth camera following

CameraFollow = {
    follow_speed = 5.0,
    offset_y = -1.0,
    target = nil,
    current_x = 0,
    current_y = 0
}

function CameraFollow:OnStart()
    self.target = Actor.Find("Player")
    if self.target ~= nil then
        local rb = self.target:GetComponent("Rigidbody")
        if rb ~= nil then
            local pos = rb:GetPosition()
            self.current_x = pos.x
            self.current_y = pos.y + self.offset_y
        end
    end
end

function CameraFollow:OnLateUpdate()
    if self.target == nil then
        self.target = Actor.Find("Player")
        if self.target == nil then return end
    end

    local rb = self.target:GetComponent("Rigidbody")
    if rb == nil then return end

    local dt = Time.GetDeltaTime()
    local target_pos = rb:GetPosition()

    -- Smooth follow
    local target_x = target_pos.x
    local target_y = target_pos.y + self.offset_y

    self.current_x = self.current_x + (target_x - self.current_x) * self.follow_speed * dt
    self.current_y = self.current_y + (target_y - self.current_y) * self.follow_speed * dt

    Camera.SetPosition(self.current_x, self.current_y)
end
