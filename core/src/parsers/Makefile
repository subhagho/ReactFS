CC    ?= clang
CXX   ?= clang++

EXE = parser

CDEBUG = -g -Wall

CXXDEBUG = -g -Wall

CSTD = -std=c99
CXXSTD = -std=c++14

CFLAGS = -Wno-deprecated-register -O0  $(CDEBUG) $(CSTD) 
CXXFLAGS = -Wno-deprecated-register -O0  $(CXXDEBUG) $(CXXSTD) -I../../.. -I/opt/local/include


CPPOBJ = main schema_driver
SOBJ =  schema_parser schema_lexer

FILES = $(addsuffix .cpp, $(CPPOBJ))

OBJS  = $(addsuffix .o, $(CPPOBJ))

CLEANLIST =  $(addsuffix .o, $(OBJ)) $(OBJS) \
				 mc_parser.tab.cc mc_parser.tab.hh \
				 location.hh position.hh \
			    stack.hh mc_parser.output parser.o \
				 lexer.o mc_lexer.yy.cc $(EXE)\
.PHONY: all
all: parser

parser: $(FILES)
	$(MAKE)  $(SOBJ)
	$(MAKE)  $(OBJS)
	$(CXX)  $(CXXFLAGS) -o $(EXE) $(OBJS) schema_parser.o schema_lexer.o $(LIBS)

schema_parser: ../../parsers/schema.yy
	bison -d -v ../../parsers/schema.yy -o schema.tab.cc
	$(CXX) -I../../.. -I/opt/local/include $(CXXFLAGS) -c -o schema_parser.o schema.tab.cc

schema_lexer: ../../parsers/schema.l
	flex --outfile=schema_lexer.yy.cc  $<
	$(CXX) -I../../.. -I/opt/local/include  $(CXXFLAGS) -c schema_lexer.yy.cc -o schema_lexer.o

.PHONY: test
test:
	test/test0.pl

.PHONY: clean
clean:
	rm -rf $(CLEANLIST)

