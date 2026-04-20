-- GameManager — score, HUD, title card.
-- Persists across scene transitions via Scene.DontDestroy so the score
-- carries through level 1 → 2 → victory. TitleScreen destroys us on new
-- game so a fresh one spawns on level1.
--
-- Note: EventSystem is wiped on every scene load, so we re-subscribe in
-- OnUpdate whenever the scene name changes.

local LEVEL_NAMES = {
    level1 = "Level 1",
    level2 = "Level 2",
}

GameManager = {
    score  = 0,
    coins  = 0,
    deaths = 0,

    title_elapsed = 99.0,
    last_scene    = "",
    score_pulse   = 0,
}

function GameManager:Subscribe()
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
end

function GameManager:OnStart()
    -- If a GameManager is already persistent (we came here via scene load
    -- rather than first boot), this new one is a duplicate from the scene
    -- JSON — destroy self and let the persistent one keep its state.
    local existing = Actor.FindAll("GameManager")
    if #existing > 1 then
        Actor.Destroy(self.actor)
        return
    end

    self.score  = 0
    self.coins  = 0
    self.deaths = 0
    self.score_pulse = 0
    Scene.DontDestroy(self.actor)
end

function GameManager:OnUpdate()
    local dt = Time.GetUnscaledDeltaTime()
    local current = Scene.GetCurrent()

    -- Scene change → reset title card timer and re-subscribe to events
    -- (EventSystem.Clear() runs on every scene transition).
    if current ~= self.last_scene then
        self.last_scene    = current
        self.title_elapsed = 0
        self:Subscribe()
    end
    self.title_elapsed = self.title_elapsed + dt

    if self.score_pulse > 0 then
        self.score_pulse = math.max(0, self.score_pulse - dt * 3.5)
    end

    local level_name = LEVEL_NAMES[current]
    if not level_name then return end  -- title / victory draw their own UI

    -- Top HUD bar.
    Image.DrawUIEx("hud_bar", 8, 8, 255, 255, 255, 230, 100)

    local score_size = 26 + math.floor(self.score_pulse * 8)
    Text.Draw("SCORE  " .. self.score, 20, 18,
        "OpenSans-Regular", score_size, 255, 255, 255, 255)

    Image.DrawUIEx("coin", 260, 40, 255, 215, 40, 255, 101)
    Text.Draw("x " .. self.coins, 292, 24,
        "OpenSans-Regular", 22, 255, 215, 40, 255)

    Text.Draw(level_name, 620, 24,
        "OpenSans-Regular", 22, 220, 220, 230, 255)
    if self.deaths > 0 then
        Text.Draw("deaths " .. self.deaths, 620, 50,
            "OpenSans-Regular", 14, 200, 120, 120, 255)
    end

    -- Title card.
    local t = self.title_elapsed
    if t < 2.8 then
        local alpha
        if t < 0.35    then alpha = t / 0.35
        elseif t < 2.2 then alpha = 1.0
        else                alpha = math.max(0, 1 - (t - 2.2) / 0.6)
        end
        local a = math.floor(alpha * 255)
        Text.Draw(level_name, 302, 222,
            "OpenSans-Regular", 64, 0, 0, 0, math.floor(alpha * 160))
        Text.Draw(level_name, 300, 220,
            "OpenSans-Regular", 64, 255, 245, 230, a)

        if current == "level1" then
            Text.Draw("A / D  to move       SPACE  to jump       reach the flag",
                150, 310, "OpenSans-Regular", 18, 220, 220, 230, a)
        elseif current == "level2" then
            Text.Draw("mind the spikes    ride the moving platform",
                210, 310, "OpenSans-Regular", 18, 220, 220, 230, a)
        end
    end

    -- Footer controls strip.
    Text.Draw("MOVE  A D        JUMP  SPACE        PAUSE  P        RESTART  R        QUIT  ESC",
        55, 574, "OpenSans-Regular", 14, 200, 210, 230, 200)

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
