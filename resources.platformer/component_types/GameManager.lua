-- GameManager — score, HUD, onboarding. Persists across scenes.

GameManager = {
    level_name = "Level 1",
    score  = 0,
    coins  = 0,
    deaths = 0,
    initialized = false,

    -- Title card
    title_elapsed = 0,  -- seconds since scene load; title shows for ~2.5s

    -- Score pulse on coin collection
    score_pulse = 0,
}

function GameManager:OnStart()
    self.title_elapsed = 0
    self.score_pulse = 0

    if not self.initialized then
        self.score  = 0
        self.coins  = 0
        self.deaths = 0
        self.initialized = true
        Scene.DontDestroy(self.actor)

        Event.Subscribe("coin_collected", function(data)
            self.score = self.score + (data.value or 0)
            self.coins = self.coins + 1
            self.score_pulse = 1.0
        end)
        Event.Subscribe("enemy_killed", function(data)
            self.score = self.score + (data.points or 0)
            self.score_pulse = 1.0
        end)
        Event.Subscribe("player_died", function()
            self.deaths = self.deaths + 1
        end)
        Event.Subscribe("level_cleared", function()
            self.coins = 0
        end)
    end
end

function GameManager:OnUpdate()
    local dt = Time.GetUnscaledDeltaTime()
    self.title_elapsed = self.title_elapsed + dt
    if self.score_pulse > 0 then
        self.score_pulse = math.max(0, self.score_pulse - dt * 3.5)
    end

    -- ---- Top HUD bar -----------------------------------------------------
    Image.DrawUIEx("hud_bar", 8, 8, 255, 255, 255, 230, 100)

    -- score (pulses on pickup via scale-like extra size when pulse > 0)
    local score_size = 26 + math.floor(self.score_pulse * 8)
    Text.Draw("SCORE  " .. self.score, 20, 18,
        "OpenSans-Regular", score_size, 255, 255, 255, 255)

    -- coin icon + count
    Image.DrawUIEx("coin", 260, 40, 255, 215, 40, 255, 101)
    Text.Draw("x " .. self.coins, 292, 24,
        "OpenSans-Regular", 22, 255, 215, 40, 255)

    -- level name (right side)
    Text.Draw(self.level_name, 620, 24,
        "OpenSans-Regular", 22, 220, 220, 230, 255)

    -- deaths counter (subtle, right)
    if self.deaths > 0 then
        Text.Draw("deaths " .. self.deaths, 620, 50,
            "OpenSans-Regular", 14, 200, 120, 120, 255)
    end

    -- ---- Title card (fades in, holds, fades out) -------------------------
    local t = self.title_elapsed
    if t < 2.8 then
        local alpha
        if t < 0.35 then
            alpha = t / 0.35
        elseif t < 2.2 then
            alpha = 1.0
        else
            alpha = math.max(0, 1 - (t - 2.2) / 0.6)
        end
        local a = math.floor(alpha * 255)
        -- Backing shadow
        Text.Draw(self.level_name, 302, 222,
            "OpenSans-Regular", 64, 0, 0, 0, math.floor(alpha * 160))
        Text.Draw(self.level_name, 300, 220,
            "OpenSans-Regular", 64, 255, 245, 230, a)

        if self.level_name == "Level 1" then
            Text.Draw("A/D or arrows to move    Space to jump    reach the flag",
                170, 310, "OpenSans-Regular", 18, 220, 220, 230, a)
        end
    end

    -- ---- Footer hint -----------------------------------------------------
    Text.Draw("MOVE  A D        JUMP  SPACE        PAUSE  P        RESTART  R        QUIT  ESC",
        55, 574, "OpenSans-Regular", 14, 200, 210, 230, 200)

    -- ---- Controls --------------------------------------------------------
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
