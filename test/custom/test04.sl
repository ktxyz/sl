integer a; string b;
a := readint; b := readstr;
if a < length(b) then
    print("case1")
elif b == concatenate("a", substring(b, 1, 1)) then
    print("case2")
else
    print("default");