count = 1
play = true
record = nil
while play do
    print("What do you want to do ?")
    print("1. Play")
    print("2. Quit")
    r : int = io.readint()
    if r == 1 then
        print("Game number ${count}")
        goal = random.int(1, 100)
        guess = nil
        try = 0
        while guess != goal do
            try += 1
            print("Your guess ${try}: ")
            guess = io.readint()
            if guess > goal then
                print("Lower!")
            elsif guess < goal then
                print("Higher!")
            end
        loop
        print("You won in ${try}!")
        if record is nil or try < record then
            print("New record!")
        end
    elsif r == 2 then
        play = false
    else
        print("Please enter 1 or 2")
    end
    if NotANumberException then
        print("Please enter 1 or 2")
    end
loop
