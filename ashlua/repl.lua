local Lexer = require("lexer")
local Parser = require("parser")
local Transpiler = require("transpiler")

local lexer = Lexer:new()
local parser = Parser:new()
local transpiler = Transpiler:new()

-- lexer:lex("ab54_3")

for i, v in pairs(lexer) do
    if type(v) == "function" then print(i, v, debug.getinfo(v)) end
end

local cmd = ""
while cmd ~= "exit" do
    io.write(">>> ")
    io.flush()
    cmd = io.read()
    if cmd ~= "exit" then
        local tokens = lexer:lex(cmd)
        print("Token list:")
        lexer:info()
        print("AST:")
        parser:parse(tokens)
        parser:info()
        print("Lua code:")
        local lua_code = transpiler:transpile(parser.root, true)
        print(lua_code)
        print("Execute:")
        local code, err = load(lua_code)
        if code then
            local ok, res = pcall(code)
            if ok then
                print(res)
            else
                print("Execution error:", res)
            end
        else
            print("Compilation error:", err)
        end

    end
end
