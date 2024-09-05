local Lexer = require("lexer")
local Parser = require("parser")

local lexer = Lexer:new()
local parser = Parser:new()

--lexer:lex("ab54_3")

for i,v in pairs(lexer) do
    if type(v) == "function" then
        print(i,v,debug.getinfo(v))
    end
end

local cmd = ""
while cmd ~= "exit" do
    io.write(">>> ")
    io.flush()
    cmd = io.read()
    if cmd ~= "exit" then
        local tokens = lexer:lex(cmd)
        lexer:info()
        parser:parse(tokens)
        parser:info()
    end
end
