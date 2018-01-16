def parse(rules)
    matches = []
    # regex to match format 3d6k3 and such.
    # (?<rolls>\d+)(?<type>[de])(?<sides>\d+)(?<sign>.)(?<mod>\d+)

    pat = /(?<rolls>\d+)?d(?<sides>\d+)(?<sign>\+|\-)?(?<mod>\d+)?/
    rules.scan(pat) { matches << $~ }
    result = 0
    matches.each do |match|
        c = Hash[ match.names.zip( match.captures ) ]
        c['rolls'] = 1 unless c['rolls'] 
        
        r = roll(c['rolls'].to_i, c['sides'].to_i)
        
        if c['sign'] == '-'
            puts "#{r} - #{c['mod'].to_i}"
            r = r - c['mod'].to_i
        else
            puts "#{r} + #{c['mod'].to_i}"
            r = r + c['mod'].to_i
        end
        
        #puts r
        return r
    end
    
end

def roll(amount = 0, sides = 0)
    #this is the function that gets a random roll with the amount of sides and the amount of dice inputted.
    srand
    total = 0
    (amount.to_i).times do 
        total += rand(1..sides.to_i)
    end
    return total
end

while true
    raw_input = gets.chomp.to_s
    if raw_input == "exit"
        abort("May your rolls be ever natural.")
    end
    fin = parse(raw_input)
    puts "#{fin.to_s}"
end