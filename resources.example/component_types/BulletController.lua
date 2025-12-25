-- BulletController.lua
-- Controls bullet movement and collision

BulletController = {
    speed = 10.0,
    damage = 1,
    is_player_bullet = true,
    rigidbody = nil
}

function BulletController:OnStart()
    self.rigidbody = self.actor:GetComponent("Rigidbody")
    local direction = -1
    if not self.is_player_bullet then
        direction = 1
    end
    self.rigidbody:SetVelocity(Vector2(0, self.speed * direction))
end

function BulletController:OnUpdate()
    local pos = self.rigidbody:GetPosition()

    -- Remove if off screen
    if pos.y < -4 or pos.y > 4 then
        Actor.Destroy(self.actor)
        return
    end

    -- Draw bullet sprite
    local image_name = "bullet"
    if not self.is_player_bullet then
        image_name = "enemy_bullet"
    end
    Image.DrawEx(image_name, pos.x, pos.y, 0, 1, 1, 0.5, 0.5, 255, 255, 255, 255, 1)
end

function BulletController:OnTriggerEnter(other)
    if self.is_player_bullet and other:GetName() == "Enemy" then
        Actor.Destroy(self.actor)
    elseif not self.is_player_bullet and other:GetName() == "Player" then
        Actor.Destroy(self.actor)
    end
end
