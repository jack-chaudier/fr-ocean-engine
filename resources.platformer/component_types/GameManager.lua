-- GameManager.lua
-- Manages game state, score, and UI

GameManager = {
    level_name = "Level 1",
    next_level = "level1",
    score = 0,
    coins = 0
}

function GameManager:OnStart()
    self.score = 0
    self.coins = 0

    -- Subscribe to events
    Event.Subscribe("coin_collected", function(data)
        self.score = self.score + data.value
        self.coins = self.coins + 1
    end)

    Event.Subscribe("enemy_killed", function(data)
        self.score = self.score + data.points
    end)

    Event.Subscribe("player_died", function(data)
        self.score = 0
        self.coins = 0
    end)
end

function GameManager:OnUpdate()
    -- Draw UI
    Text.Draw("Score: " .. self.score, 10, 10, "OpenSans-Regular", 24, 255, 255, 255, 255)
    Text.Draw("Coins: " .. self.coins, 10, 40, "OpenSans-Regular", 20, 255, 215, 0, 255)
    Text.Draw(self.level_name, 650, 10, "OpenSans-Regular", 20, 200, 200, 200, 255)

    -- Controls hint
    Text.Draw("WASD/Arrows: Move | Space: Jump | R: Restart", 150, 570, "OpenSans-Regular", 14, 150, 150, 150, 255)

    -- Restart
    if Input.GetKeyDown("r") then
        Scene.Load(Scene.GetCurrent())
    end

    -- Quit
    if Input.GetKeyDown("escape") then
        Application.Quit()
    end

    -- Pause (time scale demo)
    if Input.GetKeyDown("p") then
        if Time.GetTimeScale() > 0 then
            Time.SetTimeScale(0)
        else
            Time.SetTimeScale(1)
        end
    end
end
