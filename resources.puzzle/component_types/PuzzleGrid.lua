-- PuzzleGrid.lua
-- Match-3 puzzle game grid management

PuzzleGrid = {
    grid_width = 6,
    grid_height = 6,
    num_piece_types = 4,
    target_score = 300,
    max_moves = 25,

    -- Runtime state
    grid = {},
    cell_size = 64,
    grid_offset_x = 200,
    grid_offset_y = 100,
    selected_x = -1,
    selected_y = -1,
    score = 0,
    moves_left = 0,
    game_over = false,
    game_won = false,
    animating = false
}

-- Piece colors for rendering
local piece_colors = {
    {255, 60, 60},    -- Red
    {60, 200, 60},    -- Green
    {60, 100, 255},   -- Blue
    {255, 220, 60},   -- Yellow
    {200, 60, 255}    -- Purple
}

function PuzzleGrid:OnStart()
    self.moves_left = self.max_moves
    self.score = 0
    self.game_over = false
    self.game_won = false
    self.animating = false

    -- Initialize grid with random pieces (avoiding initial matches)
    self:InitializeGrid()

    -- Subscribe to tween completion for chain reactions
    Event.Subscribe("tween_complete", function(data)
        if data.grid_id == self.actor:GetID() then
            self:OnAnimationComplete()
        end
    end)
end

function PuzzleGrid:InitializeGrid()
    self.grid = {}
    for x = 1, self.grid_width do
        self.grid[x] = {}
        for y = 1, self.grid_height do
            repeat
                self.grid[x][y] = math.random(1, self.num_piece_types)
            until not self:WouldCreateMatch(x, y, self.grid[x][y])
        end
    end
end

function PuzzleGrid:WouldCreateMatch(x, y, piece_type)
    -- Check horizontal match (need 2 same pieces to the left)
    if x >= 3 then
        if self.grid[x-1] and self.grid[x-2] then
            if self.grid[x-1][y] == piece_type and self.grid[x-2][y] == piece_type then
                return true
            end
        end
    end

    -- Check vertical match (need 2 same pieces above)
    if y >= 3 then
        if self.grid[x][y-1] == piece_type and self.grid[x][y-2] == piece_type then
            return true
        end
    end

    return false
end

function PuzzleGrid:OnUpdate()
    -- Draw grid background
    self:DrawGrid()

    -- Handle input if not animating and game not over
    if not self.animating and not self.game_over then
        self:HandleInput()
    end

    -- Draw game over/win overlay
    if self.game_over then
        self:DrawGameOverOverlay()
    end
end

function PuzzleGrid:DrawGrid()
    local cell = self.cell_size
    local ox = self.grid_offset_x
    local oy = self.grid_offset_y

    -- Draw grid cells and pieces
    for x = 1, self.grid_width do
        for y = 1, self.grid_height do
            local px = ox + (x - 1) * cell
            local py = oy + (y - 1) * cell

            -- Cell background
            Image.DrawPixel(px, py, cell - 2, cell - 2, 40, 40, 60, 255)

            -- Piece
            local piece = self.grid[x][y]
            if piece and piece > 0 then
                local color = piece_colors[piece] or {128, 128, 128}
                local inner_margin = 4
                Image.DrawPixel(
                    px + inner_margin,
                    py + inner_margin,
                    cell - 2 - inner_margin * 2,
                    cell - 2 - inner_margin * 2,
                    color[1], color[2], color[3], 255
                )

                -- Highlight for gems (inner shine)
                Image.DrawPixel(
                    px + inner_margin + 8,
                    py + inner_margin + 8,
                    16, 16,
                    math.min(255, color[1] + 60),
                    math.min(255, color[2] + 60),
                    math.min(255, color[3] + 60),
                    180
                )
            end

            -- Selection highlight
            if x == self.selected_x and y == self.selected_y then
                -- Draw selection border
                Image.DrawPixel(px - 2, py - 2, cell + 2, 4, 255, 255, 255, 255)
                Image.DrawPixel(px - 2, py + cell - 4, cell + 2, 4, 255, 255, 255, 255)
                Image.DrawPixel(px - 2, py - 2, 4, cell + 2, 255, 255, 255, 255)
                Image.DrawPixel(px + cell - 4, py - 2, 4, cell + 2, 255, 255, 255, 255)
            end
        end
    end
end

function PuzzleGrid:HandleInput()
    if Input.GetMouseButtonDown(1) then
        local mx = Input.GetMousePosition().x
        local my = Input.GetMousePosition().y

        -- Convert to grid coordinates
        local gx = math.floor((mx - self.grid_offset_x) / self.cell_size) + 1
        local gy = math.floor((my - self.grid_offset_y) / self.cell_size) + 1

        -- Check if within grid bounds
        if gx >= 1 and gx <= self.grid_width and gy >= 1 and gy <= self.grid_height then
            if self.selected_x == -1 then
                -- First selection
                self.selected_x = gx
                self.selected_y = gy
                -- Audio.Play(0, "click", false)  -- Audio disabled
            else
                -- Second selection - check if adjacent
                local dx = math.abs(gx - self.selected_x)
                local dy = math.abs(gy - self.selected_y)

                if (dx == 1 and dy == 0) or (dx == 0 and dy == 1) then
                    -- Adjacent - attempt swap
                    self:TrySwap(self.selected_x, self.selected_y, gx, gy)
                else
                    -- Not adjacent - new selection
                    self.selected_x = gx
                    self.selected_y = gy
                    -- Audio.Play(0, "click", false)  -- Audio disabled
                end
            end
        else
            -- Clicked outside grid - deselect
            self.selected_x = -1
            self.selected_y = -1
        end
    end

    -- Keyboard shortcuts
    if Input.GetKeyDown("r") then
        Scene.Load("puzzle")
    end

    if Input.GetKeyDown("escape") then
        Application.Quit()
    end
end

function PuzzleGrid:TrySwap(x1, y1, x2, y2)
    -- Perform the swap
    local temp = self.grid[x1][y1]
    self.grid[x1][y1] = self.grid[x2][y2]
    self.grid[x2][y2] = temp

    -- Check if this creates a match
    local matches = self:FindAllMatches()

    if #matches > 0 then
        -- Valid move - use a move
        self.moves_left = self.moves_left - 1
        -- Audio.Play(1, "match", false)  -- Audio disabled

        -- Process matches
        self:ProcessMatches(matches)
    else
        -- Invalid move - swap back
        self.grid[x2][y2] = self.grid[x1][y1]
        self.grid[x1][y1] = temp
        -- Audio.Play(2, "invalid", false)  -- Audio disabled
    end

    -- Clear selection
    self.selected_x = -1
    self.selected_y = -1
end

function PuzzleGrid:FindAllMatches()
    local matches = {}
    local matched = {}

    -- Initialize matched grid
    for x = 1, self.grid_width do
        matched[x] = {}
        for y = 1, self.grid_height do
            matched[x][y] = false
        end
    end

    -- Find horizontal matches
    for y = 1, self.grid_height do
        local run_start = 1
        local run_type = self.grid[1][y]

        for x = 2, self.grid_width + 1 do
            local current = self.grid[x] and self.grid[x][y] or -1

            if current ~= run_type or x > self.grid_width then
                -- Check if run is 3+
                local run_length = x - run_start
                if run_length >= 3 and run_type and run_type > 0 then
                    for i = run_start, x - 1 do
                        matched[i][y] = true
                    end
                end
                run_start = x
                run_type = current
            end
        end
    end

    -- Find vertical matches
    for x = 1, self.grid_width do
        local run_start = 1
        local run_type = self.grid[x][1]

        for y = 2, self.grid_height + 1 do
            local current = self.grid[x][y] or -1

            if current ~= run_type or y > self.grid_height then
                local run_length = y - run_start
                if run_length >= 3 and run_type and run_type > 0 then
                    for i = run_start, y - 1 do
                        matched[x][i] = true
                    end
                end
                run_start = y
                run_type = current
            end
        end
    end

    -- Collect matched positions
    for x = 1, self.grid_width do
        for y = 1, self.grid_height do
            if matched[x][y] then
                table.insert(matches, {x = x, y = y})
            end
        end
    end

    return matches
end

function PuzzleGrid:ProcessMatches(matches)
    -- Add score based on matches
    local points = #matches * 10
    self.score = self.score + points

    -- Emit score event
    Event.Emit("score_changed", {score = self.score, added = points})

    -- Remove matched pieces
    for _, pos in ipairs(matches) do
        self.grid[pos.x][pos.y] = 0
    end

    -- Apply gravity
    self:ApplyGravity()

    -- Check for chain reactions
    local new_matches = self:FindAllMatches()
    if #new_matches > 0 then
        -- Chain reaction bonus
        self.score = self.score + #new_matches * 5
        self:ProcessMatches(new_matches)
    else
        -- Check win/lose conditions
        self:CheckGameState()
    end
end

function PuzzleGrid:ApplyGravity()
    for x = 1, self.grid_width do
        -- Compact column downward
        local write_y = self.grid_height

        for read_y = self.grid_height, 1, -1 do
            if self.grid[x][read_y] ~= 0 then
                if write_y ~= read_y then
                    self.grid[x][write_y] = self.grid[x][read_y]
                    self.grid[x][read_y] = 0
                end
                write_y = write_y - 1
            end
        end

        -- Fill empty spaces at top with new pieces
        for y = write_y, 1, -1 do
            self.grid[x][y] = math.random(1, self.num_piece_types)
        end
    end
end

function PuzzleGrid:CheckGameState()
    if self.score >= self.target_score then
        self.game_won = true
        self.game_over = true
        Event.Emit("game_won", {score = self.score})
    elseif self.moves_left <= 0 then
        self.game_over = true
        Event.Emit("game_lost", {score = self.score})
    end
end

function PuzzleGrid:DrawGameOverOverlay()
    -- Semi-transparent overlay
    Image.DrawPixel(0, 0, 800, 600, 0, 0, 0, 180)

    if self.game_won then
        Text.Draw("YOU WIN!", 300, 200, "OpenSans-Regular", 48, 100, 255, 100, 255)
        Text.Draw("Score: " .. self.score, 320, 280, "OpenSans-Regular", 32, 255, 255, 255, 255)
    else
        Text.Draw("GAME OVER", 280, 200, "OpenSans-Regular", 48, 255, 100, 100, 255)
        Text.Draw("Score: " .. self.score .. " / " .. self.target_score, 280, 280, "OpenSans-Regular", 28, 255, 255, 255, 255)
    end

    Text.Draw("Press R to Restart", 300, 380, "OpenSans-Regular", 24, 200, 200, 200, 255)
    Text.Draw("Press ESC to Quit", 310, 420, "OpenSans-Regular", 24, 200, 200, 200, 255)
end

function PuzzleGrid:OnAnimationComplete()
    self.animating = false
end
