-- Coin.lua
-- Collectible coin

Coin = {
    value = 10,
    collected = false,
    bob_offset = 0
}

function Coin:OnStart()
    self.collected = false
    self.bob_offset = math.random() * 6.28
    self.rigidbody = self.actor:GetComponent("Rigidbody")
end

function Coin:OnUpdate()
    if self.collected then return end

    local dt = Time.GetDeltaTime()
    local pos = self.rigidbody:GetPosition()

    -- Bobbing animation
    self.bob_offset = self.bob_offset + dt * 3
    local bob_y = math.sin(self.bob_offset) * 0.1

    -- Draw coin (yellow)
    Image.DrawEx("coin", pos.x, pos.y + bob_y, 0, 0.3, 0.3, 0.5, 0.5, 255, 215, 0, 255, 2)
end

function Coin:Collect()
    if self.collected then return end
    self.collected = true
    Event.Emit("coin_collected", { value = self.value })
    Actor.Destroy(self.actor)
end
