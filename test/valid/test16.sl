string s;
integer len;
s := "hello";
len := length(s);
if len = 0 then print("empty")
elif len = 5 and s == "hello" then print("match1")
elif len = 5 then print("match2")
else print("no match");