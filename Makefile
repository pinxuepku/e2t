compiler:  grammar.cpp token.cpp
	g++ -o compiler token.cpp grammar.tab.cpp tree.cpp 

grammar.cpp: grammar.y
	bison -d  grammar.y -o grammar.tab.cpp

token.cpp: token.l
	flex -o  token.cpp token.l

clean:
	rm compiler token.cpp grammar.tab.cpp grammar.tab.hpp
