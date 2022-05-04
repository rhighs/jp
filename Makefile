cxx := g++
flags  := -std=c++17 -Wno-switch -lcurl -g
include := -Iinclude
allsrc := src/main.cpp src/tokenizer.cpp src/json_parser.cpp
objects := tokenizer.o json_parser.o main.o
progname := jp

all: tokenizer.o json_parser.o main.o
	$(cxx) $(objects) $(include) $(flags) -o $(progname) && rm *.o

liner:
	$(cxx) $(allsrc) $(include) $(flags) -o $(progname)

main.o:
	$(cxx) -c src/main.cpp $(include) $(flags)

tokenizer.o:
	$(cxx) -c src/tokenizer.cpp $(include) $(flags)

json_parser.o:
	$(cxx) -c src/json_parser.cpp $(include) $(flags)

.PHONY: clean cleanho

clean:
	rm -f *.o jp

cleanho:
	rm header_only/*

