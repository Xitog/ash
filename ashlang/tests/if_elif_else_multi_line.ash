-- Test 1

a = 7

if a == 5 then
    writeln("never 1.1")
elif a == 6 then
    writeln("never 1.2")
else
    writeln("a == 7 (on else after elif)")
end

-- Test 2

a = 8

if a == 5 then
    writeln("never 2.1")
elif a == 6 then
    writeln("never 2.2")
elif a == 8 then
    writeln("a == 8 (on elif)")
else
    writeln("never 2.3")
end
