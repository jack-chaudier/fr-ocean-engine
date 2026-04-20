-- VictoryScreen — shown after beating the last level. Reads stats from the
-- persistent GameManager (if any) and offers restart.

VictoryScreen = {
    elapsed = 0,
    final_score = 0,
    final_coins = 0,
    final_deaths = 0,
}

function VictoryScreen:OnStart()
    self.elapsed = 0
    local gm = Actor.Find("GameManager")
    if gm then
        local mgr = gm:GetComponent("GameManager")
        if mgr then
            self.final_score  = mgr.score
            self.final_coins  = mgr.coins
            self.final_deaths = mgr.deaths
        end
    end
end

function VictoryScreen:OnUpdate()
    local dt = Time.GetUnscaledDeltaTime()
    self.elapsed = self.elapsed + dt

    Image.DrawRect(0,   0, 800, 300, 10, 30, 20, 255)
    Image.DrawRect(0, 300, 800, 300,  6, 18, 12, 255)

    local a = math.floor(math.min(1.0, self.elapsed / 0.6) * 255)

    Text.Draw("YOU WIN",
        220, 130, "OpenSans-Regular", 92, 120, 230, 140, a)

    -- Score panel.
    Text.Draw("final score",   260, 270, "OpenSans-Regular", 18, 200, 210, 220, a)
    Text.Draw(tostring(self.final_score),
        260, 292, "OpenSans-Regular", 48, 255, 240, 160, a)

    Text.Draw("coins  " .. self.final_coins,
        480, 285, "OpenSans-Regular", 22, 255, 215, 40, a)
    Text.Draw("deaths " .. self.final_deaths,
        480, 315, "OpenSans-Regular", 22, 230, 120, 120, a)

    if self.elapsed > 0.4 then
        -- Stay visible even at the pulse trough.
        local pulse = math.floor((math.sin(self.elapsed * 3) * 0.5 + 0.5) * 100 + 155)
        Text.Draw("SPACE  play again        ESC  quit",
            172, 470, "OpenSans-Regular", 26, 210, 240, 255, pulse)
    end

    if self.elapsed > 0.8 and (Input.GetKeyDown("space") or Input.GetKeyDown("return")) then
        Scene.LoadWithTransition("title", "fade", 0.5)
    end
    if Input.GetKeyDown("escape") then
        Application.Quit()
    end
end
