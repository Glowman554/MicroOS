--!/opt/bin/lua
print("Hello from Lua!")

function factorial(n)
    local result = 1
    for i = 2, n do
        result = result * i
    end
    return result
end

for i = 1, 10 do
    print("factorial(" .. i .. ") = " .. factorial(i))
end