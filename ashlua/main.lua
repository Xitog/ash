local Lexer = require("lexer")

function love.load()
    love.window.setTitle("Ash")
    local lexer = Lexer.new()
    lexer:lex("ab5")
end
