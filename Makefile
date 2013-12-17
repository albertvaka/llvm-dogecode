all: parser


OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \
       corefn.o  \

CPPFLAGS = `llvm-config-3.4 --cppflags`
LDFLAGS = `llvm-config-3.4 --ldflags`
LIBS = `llvm-config-3.4 --libs`

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

libdoge_builtins.so: builtins.c
	gcc -shared -o $@ -fPIC $<

parser: $(OBJS) libdoge_builtins.so
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS) -L. -l doge_builtins -Wl,-rpath,.



