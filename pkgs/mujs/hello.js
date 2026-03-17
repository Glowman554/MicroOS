//!/opt/bin/mujs
console.log("Hello from MuJS!");

function factorial(n) {
    var result = 1;
    for (var i = 2; i <= n; i++) {
        result = result * i;
    }
    return result;
}

for (var i = 1; i <= 10; i++) {
    console.log("factorial(" + i + ") = " + factorial(i));
}