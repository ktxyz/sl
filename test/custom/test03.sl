integer x; string y;
x := readint; y := readstr;
if x < length(y) and not (y == "secret") then
    print("Case1")
elif position(y, substring("mystery",1,3)) > 0 then
    print("Case2")
else
    print(concatenate(y, "3"));