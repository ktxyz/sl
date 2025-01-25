integer age; 
string msg;
integer i; 
string name;

// Simulate input: age=25, name="Alice" 
age := readint; 
name := readstr; 

msg := concatenate("Name: ", name); 
print(msg); 

if age > 20 and length(name) > 3 then 
  begin 
    for i := 1 to age / 5 do 
      begin 
        print(concatenate("Decade ", substring("IIIII", 1, i))); 
        if i = 3 then break; 
      end; 
  end; 

// Force type error if uncommented 
// string error := age; 