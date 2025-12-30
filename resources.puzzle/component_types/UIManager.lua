-- UIManager.lua
-- Handles UI rendering for the puzzle game

UIManager = {
    score = 0,
    target_score = 300,
    moves_left = 25
}

function UIManager:OnStart()
    -- Get reference to PuzzleGrid for data
    local puzzle_grid = self.actor:GetComponent("PuzzleGrid")
    if puzzle_grid then
        self.target_score = puzzle_grid.target_score
        self.moves_left = puzzle_grid.max_moves
    end

    -- Subscribe to score changes
    Event.Subscribe("score_changed", function(data)
        self.score = data.score
    end)
end

function UIManager:OnUpdate()
    -- Get current state from PuzzleGrid
    local puzzle_grid = self.actor:GetComponent("PuzzleGrid")
    if puzzle_grid then
        self.score = puzzle_grid.score
        self.moves_left = puzzle_grid.moves_left
        self.target_score = puzzle_grid.target_score
    end

    -- Draw UI panel on the left
    self:DrawLeftPanel()

    -- Draw instructions at bottom
    self:DrawInstructions()
end

function UIManager:DrawLeftPanel()
    -- Panel background
    Image.DrawRect(10, 100, 170, 200, 40, 40, 60, 220)

    -- Title
    Text.Draw("PUZZLE MATCH", 20, 20, "OpenSans-Regular", 24, 255, 200, 100, 255)

    -- Score section
    Text.Draw("SCORE", 30, 120, "OpenSans-Regular", 16, 180, 180, 180, 255)
    Text.Draw(tostring(self.score), 30, 145, "OpenSans-Regular", 32, 255, 255, 255, 255)

    -- Target section
    Text.Draw("TARGET", 30, 195, "OpenSans-Regular", 16, 180, 180, 180, 255)
    Text.Draw(tostring(self.target_score), 30, 220, "OpenSans-Regular", 28, 100, 255, 100, 255)

    -- Progress bar
    local progress = math.min(1.0, self.score / self.target_score)
    local bar_width = 140
    local bar_height = 12

    -- Bar background
    Image.DrawRect(25, 255, bar_width, bar_height, 60, 60, 80, 255)

    -- Bar fill
    local fill_width = math.floor(bar_width * progress)
    if fill_width > 0 then
        local r = math.floor(255 * (1 - progress) + 100 * progress)
        local g = math.floor(100 * (1 - progress) + 255 * progress)
        Image.DrawRect(25, 255, fill_width, bar_height, r, g, 100, 255)
    end

    -- Moves section
    Text.Draw("MOVES LEFT", 30, 285, "OpenSans-Regular", 16, 180, 180, 180, 255)

    -- Color code moves remaining
    local moves_color_r = 255
    local moves_color_g = 255
    if self.moves_left <= 5 then
        moves_color_g = 100  -- Red when low
    elseif self.moves_left <= 10 then
        moves_color_g = 200  -- Yellow-ish when medium
    end

    Text.Draw(tostring(self.moves_left), 30, 310, "OpenSans-Regular", 36, moves_color_r, moves_color_g, 100, 255)
end

function UIManager:DrawInstructions()
    Text.Draw("Click to select, click adjacent piece to swap | R: Restart | ESC: Quit",
              120, 570, "OpenSans-Regular", 14, 150, 150, 150, 255)
end
