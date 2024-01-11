run: cpython
	./cpython $(INPUT) $(OUTPUT)

cpython: parser.cpp lexer.cpp utils.cpp
	g++ parser.cpp lexer.cpp utils.cpp -lm -o cpython

parser.hpp parser.cpp: parser.y
	bison -d -t -v -o parser.cpp parser.y

lexer.h lexer.cpp: lexer.l
	flex --header-file=lexer.h -o lexer.cpp lexer.l