integer i;
integer j;
for i := 1 to 3 do
begin
  for j := 1 to 5 do
    begin
        if j = 3 then continue;
        if j = 5 then break;
        print(j);
    end;
end;