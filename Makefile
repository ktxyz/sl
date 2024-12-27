all: sl

src/sl.tab.h: src/sl.y
	bison -osrc/sl.tab.h src/sl.y

src/lex.yy.c: src/sl.l src/sl.tab.h
	flex -o src/lex.yy.c src/sl.l

sl: src/lex.yy.c src/sl.tab.h
	gcc -o sl src/lex.yy.c -Isrc/

clean:
	rm sl src/sl.tab.h src/lex.yy.c 
