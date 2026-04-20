-- Background — fixed full-screen backdrop that ignores camera offset.

Background = {}

function Background:OnUpdate()
    Image.DrawUI("background", 0, 0, 1, 1, 255, 255, 255, 255, -10)
end
