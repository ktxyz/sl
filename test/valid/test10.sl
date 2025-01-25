integer count;
string message;

count := 5;
message := "Hello World";

print(message);
print(count);

for count := 1 to 3 do
    print(concatenate("Iteration: ", message));

if count > 2 then begin
    print("Count is greater than 2");
end
else
    print("Count is small");