-- CameraFollow — smooth lerp follow with a slight vertical lead below the
-- player so you can see where you're landing.

CameraFollow = {
    follow_speed = 8.0,
    offset_y     = -0.3,  -- negative = camera slightly above player, so more
                          -- of the level is visible below the player
    zoom         = 0.85,  -- <1 = zoomed out, bigger viewport
    target       = nil,
    current_x    = 0,
    current_y    = 0,
}

function CameraFollow:OnStart()
    Camera.SetZoom(self.zoom)
    self.target = Actor.Find("Player")
    if self.target ~= nil then
        local rb = self.target:GetComponent("Rigidbody")
        if rb ~= nil then
            local pos = rb:GetPosition()
            self.current_x = pos.x
            self.current_y = pos.y + self.offset_y
            Camera.SetPosition(self.current_x, self.current_y)
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

    local dt  = Time.GetDeltaTime()
    local p   = rb:GetPosition()
    local tx  = p.x
    local ty  = p.y + self.offset_y

    self.current_x = self.current_x + (tx - self.current_x) * self.follow_speed * dt
    self.current_y = self.current_y + (ty - self.current_y) * self.follow_speed * dt

    Camera.SetPosition(self.current_x, self.current_y)
end
