local Lexer = require("lexer")

local lexer = Lexer:new()
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
        lexer:lex(cmd)
        lexer:info()
    end
end
