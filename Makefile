run: korova
	./korova $(INPUT) $(OUTPUT)

korova: parser.cpp lexer.cpp
	g++ -std=c++20 parser.cpp lexer.cpp grammar.cpp -lm -o korova

parser.hpp parser.cpp: parser.y
	bison -d -t -v -o parser.cpp parser.y

lexer.h lexer.cpp: lexer.l
	flex --header-file=lexer.h -o lexer.cpp lexer.l