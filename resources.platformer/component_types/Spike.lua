-- Spike — pure visual hazard. Collision is handled by PlayerController.

Spike = {
    length = 1,
}

function Spike:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    if not self.rigidbody then return end
    if self.rigidbody.trigger_width and self.rigidbody.trigger_width > 0 then
        self.length = self.rigidbody.trigger_width
    end
end

function Spike:OnUpdate()
    if not self.rigidbody then return end
    local pos = self.rigidbody:GetPosition()
    Image.DrawEx("spike", pos.x, pos.y, 0, self.length, 0.5, 0.5, 0.5, 255, 255, 255, 255, 1)
end
