string s1;
string s2;
integer len;
integer i;
string digits;
string empty;

empty := ""; 
digits := "0123456789"; // Helper to convert integers to strings
s1 := concatenate(substring("hello", 2, 3), "!!!"); 
s2 := substring(s1, position(s1, "l"), 4); 
len := length(s2); 
print(len);

if len > 2 then 
  s2 := concatenate(s2, substring("world", len - 2, 3))
else 
  s2 := concatenate(s2, "error"); 
len := length(s2); 

print(concatenate("s1: ", s1)); 
print(concatenate("s2: ", s2));
for i := 0 to 9 do
    print(substring(digits, i + 1, 1));
print(concatenate("s2 length: ", substring(digits, len + 1, 1))); 

// Test empty string edge case 
print(concatenate("Empty length: ", substring(digits, length(empty) + 1, 1))); 
print(concatenate("Empty substring: ", substring(empty, 1, 1))); 