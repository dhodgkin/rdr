def parse_opt(s)
  r = s.partition("d")
  r2 = r[2].partition("+")
  r = r + r2
  r.delete_at(2)
  return r
end

def roll(roll, die, mod)
  rolls = []

  roll.to_i.times do |i|
    ans = rand(1..die.to_i)
    puts "roll #{i + 1}: #{ans.to_s} + #{mod.to_s} = #{ans.to_i + mod.to_i}"
    ans = ans + mod.to_i
    rolls.push(ans)
  end

  return rolls
end

r = parse_opt(ARGV[0])
p r

roll = roll(r[0],r[2],r[4])
p "roll: " + roll.to_s