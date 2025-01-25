integer height;
integer row;
integer spaces;
integer stars;
string space_base;
string star_base;
string space_part;
string star_part;
string line;

space_base := "                    ";
star_base := "***************************************";

height := readint;

if height < 1 or height > 20 then
    print("Error: Height must be between 1 and 20")
else
    for row := 1 to height do
        begin
            spaces := height - row;
            stars := 2 * row - 1;
            
            if spaces > 0 then
                space_part := substring(space_base, 1, spaces)
            else
                space_part := "";
            
            star_part := substring(star_base, 1, stars);
            
            line := concatenate(space_part, star_part);
            print(line);
        end;