-- GameManager — score, HUD, level progression. Persists across scenes.

GameManager = {
    level_name = "Level 1",
    score = 0,
    coins = 0,
    deaths = 0,
    initialized = false,
}

function GameManager:OnStart()
    if not self.initialized then
        self.score  = 0
        self.coins  = 0
        self.deaths = 0
        self.initialized = true
        Scene.DontDestroy(self.actor)

        Event.Subscribe("coin_collected", function(data)
            self.score = self.score + (data.value or 0)
            self.coins = self.coins + 1
        end)
        Event.Subscribe("enemy_killed", function(data)
            self.score = self.score + (data.points or 0)
        end)
        Event.Subscribe("player_died", function()
            self.deaths = self.deaths + 1
        end)
        Event.Subscribe("level_cleared", function(data)
            -- Keep score, reset coins for the new level pickup count.
            self.coins = 0
        end)
    end
end

function GameManager:OnUpdate()
    Text.Draw("Score "  .. self.score, 14, 10, "OpenSans-Regular", 26, 255, 255, 255, 255)
    Text.Draw("Coins "  .. self.coins, 14, 42, "OpenSans-Regular", 20, 255, 215, 40, 255)
    Text.Draw(self.level_name, 640, 10, "OpenSans-Regular", 20, 220, 220, 220, 255)
    Text.Draw("WASD / Arrows   Space=Jump   R=Restart   Esc=Quit",
        90, 574, "OpenSans-Regular", 14, 180, 180, 180, 255)

    if Input.GetKeyDown("r") then
        Scene.LoadWithTransition(Scene.GetCurrent(), "fade", 0.25)
    end
    if Input.GetKeyDown("escape") then
        Application.Quit()
    end
    if Input.GetKeyDown("p") then
        Time.SetTimeScale(Time.GetTimeScale() > 0 and 0 or 1)
    end
end
