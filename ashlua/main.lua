local Lexer = require("lexer")

local WIDTH = 800
local HEIGHT = 600
local MENU_HEIGHT = 30
local MENU_START_WRITING_HEIGHT = 5
local CHAR_WIDTH = 11
local CONSOLE_FONT = love.graphics.newFont("consolas.ttf", 20)

local menu = {}
local on = {}

function love.load()
    love.window.setTitle("Ash")
    local success = love.window.setMode(WIDTH, HEIGHT, {borderless = true})
    if not success then
        error("Impossible to launch")
        love.event.quit()
    end
    local lexer = Lexer.new()
    lexer:lex("ab5")
    love.graphics.setFont(CONSOLE_FONT)
    love.createMenuOption("File")
    love.createMenuOption("Edit")
    love.createMenuOption("About")
end

function love.createMenuOption(text)
    table.insert(menu, text)
    on[text] = false
end

function love.drawMenuOption()
    local menu_options = 0
    local menu_letters = 0
    for _, text in ipairs(menu) do
        local old = menu_letters
        menu_options = menu_options + 1
        menu_letters = menu_letters + string.len(text)
        local start = (menu_options-1) * 20 + (old * CHAR_WIDTH)
        local length = 10 + ( string.len(text) * CHAR_WIDTH) + 10
        if on[text] then
            love.graphics.setColor(1, 0, 1)
            love.graphics.rectangle("fill", start, 0, length, MENU_HEIGHT)
        else
            love.graphics.rectangle("line", start, 0, length, MENU_HEIGHT)
        end
        love.graphics.setColor(1, 1, 1)
        love.graphics.print(text, start + 10, MENU_START_WRITING_HEIGHT)
        --print(start, length)
    end
end

function love.isMouseOverMenuOption()
    local x = love.mouse.getX( )
    local y = love.mouse.getY( )
    local menu_options = 0
    local menu_letters = 0
    for _, text in ipairs(menu) do
        local old = menu_letters
        menu_options = menu_options + 1
        menu_letters = menu_letters + string.len(text)
        local start = (menu_options-1) * 20 + (old * CHAR_WIDTH)
        local length = 10 + ( string.len(text) * CHAR_WIDTH) + 10
        if x >= start and x < start + length and y >= 0 and y < MENU_HEIGHT then
            on[text] = true
        else
            on[text] = false
        end
    end
end

function love.draw()
    love.graphics.print('Hello World!', 400, 300)
    love.graphics.setLineWidth(1)
    love.graphics.setLineStyle("rough")
    love.drawMenuOption()
    --love.graphics.print("File", 10, 5)
    --love.graphics.print("Edit", 80, 5) -- 15 * 4 lettres + 10 espacement + 10 start
    --love.graphics.print("About", 150, 5) -- 15 * 4 lettres + 10 espacement + 80 start
    love.graphics.line(0, MENU_HEIGHT, WIDTH, MENU_HEIGHT)
end

function love.update(_) -- dt
    if love.keyboard.isDown("escape") then
        love.event.quit()
    end
    love.isMouseOverMenuOption()
end
