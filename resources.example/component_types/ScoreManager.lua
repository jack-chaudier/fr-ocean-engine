-- ScoreManager.lua
-- Tracks and displays player score

ScoreManager = {
    score = 0
}

function ScoreManager:OnStart()
    self.score = 0
end

function ScoreManager:OnUpdate()
    Text.Draw("Score: " .. self.score, 10, 10, "OpenSans-Regular", 24, 255, 255, 255, 255)
end

function ScoreManager:AddScore(points)
    self.score = self.score + points
end

function ScoreManager:GetScore()
    return self.score
end
