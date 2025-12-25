-- EnemySpawner.lua
-- Spawns enemies at regular intervals

EnemySpawner = {
    spawn_rate = 2.0,
    spawn_cooldown = 0,
    max_enemies = 8
}

function EnemySpawner:OnStart()
    self.spawn_cooldown = 1.0
end

function EnemySpawner:OnUpdate()
    self.spawn_cooldown = self.spawn_cooldown - (1/60)

    if self.spawn_cooldown <= 0 then
        local enemies = Actor.FindAll("Enemy")
        if #enemies < self.max_enemies then
            self:SpawnEnemy()
        end
        self.spawn_cooldown = self.spawn_rate
    end
end

function EnemySpawner:SpawnEnemy()
    local enemy = Actor.Instantiate("enemy")
    local rb = enemy:GetComponent("Rigidbody")
    local x = (math.random() * 6) - 3
    rb:SetPosition(Vector2(x, -3.5))
end
