-- GameManager.lua
-- Manages game state (running, game over, restart)

GameManager = {
    game_over = false
}

function GameManager:OnStart()
    self.game_over = false
    Scene.DontDestroy(self.actor)
end

function GameManager:OnUpdate()
    if self.game_over then
        -- Draw game over text
        Text.Draw("GAME OVER", 280, 250, "OpenSans-Regular", 48, 255, 0, 0, 255)
        Text.Draw("Press R to restart", 300, 320, "OpenSans-Regular", 24, 255, 255, 255, 255)

        if Input.GetKeyDown("r") then
            self.game_over = false
            Scene.Load("main")
        end
    else
        -- Draw controls hint
        Text.Draw("WASD/Arrows: Move | Space: Shoot | ESC: Quit", 200, 570, "OpenSans-Regular", 16, 150, 150, 150, 255)
    end

    if Input.GetKeyDown("escape") then
        Application.Quit()
    end
end

function GameManager:GameOver()
    self.game_over = true
end
