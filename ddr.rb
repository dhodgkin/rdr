# ddr - DnD dice roller
# Copyright (c) 2016 Damien Hodgkin

# parse commandline arguments and put into an array
def parse_opt(s)
  if s.nil? or s == "help"
    return 0
  else
    r = s.partition("d")
    r2 = r[2].partition("+")
    r = r + r2
    r.delete_at(2)
    return r
  end
end

# rolls the dice
def roll(roll, die)
  rolls = []

  roll.to_i.times do |i|
    ans = rand(1..die.to_i)
    rolls.push(ans)
  end

  return rolls
end

# do standard roll
def die_roll(roll, die, mod)
  if mod == ""
    text = "Rolling: #{roll.to_s}d#{die.to_s}"
    puts "\n#{text.upcase.center(60)}\n\n"
  else
    text = "Rolling: #{roll.to_s}d#{die.to_s} + #{mod.to_s}."
    puts "\n#{text.upcase.center(60)}\n\n"
  end

  rolls = roll(roll, die)
  sum = rolls.reduce(:+)

  if mod == ""
    puts "Final: #{sum.to_s}"
  else
    puts "Final: #{sum.to_s} + #{mod.to_s} = #{sum.to_i + mod.to_i}"
  end
end

# do a stat roll
def stat_roll(roll, die)
  text = "Rolling Stats: #{roll.to_s}d#{die.to_s} drop lowest."
  puts "\n#{text.upcase.center(60)}\n\n"

  6.times do
    rolls = roll(roll, die)
    rolls.sort!
    rolls.delete_at(0)
    
    r = sum = rolls.reduce(:+)
    
    puts r.to_s
  end
end

def help
  print "DnD dice roller.\n"
  print "Copyright (c) 2016 Damien Hodgkin.\n"
  print "\n"
  print "usage: ddr [n1]d[n2][+n3] or ddr s\n"
  print "\tn1 is the amount of times to roll the dice\n"
  print "\tn2  is the type of die ie. 6\n"
  print "\tn3 is any modifier to add to the roll\n"
  print "\n"
  print "\ts is a stat roll\n"
end

opts = parse_opt(ARGV[0])
roll = opts[0]
die  = opts[2]
mod  = opts[4]

if roll == "s"
  stat_roll(4, 6)
elsif roll == 0
  help
else
  die_roll(roll, die, mod)
end
