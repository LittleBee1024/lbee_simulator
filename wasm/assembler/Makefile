CC=g++
CFLAGS=-Wall -g
# http://dinosaur.compilertools.net/flex/manpage.html
LEX=flex

LEX_GEN=lexer_gen.cpp
TARGET=yas

SRCS := isa.cpp lexer.cpp main.cpp output.cpp
OBJS := $(LEX_GEN:.cpp=.o) $(SRCS:.cpp=.o)

all: clean $(TARGET)

$(LEX_GEN): lexer.lex
	$(LEX) -P yas -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run:
	./$(TARGET) code.ys && diff code.yo code.yo.golden && cat code.yo

demo:
	./$(TARGET) && diff demo.yo code.yo.golden

mem_check:
	valgrind ./$(TARGET) --show-reachable=no

clean:
	rm -rf $(TARGET) $(LEX_GEN) *.o *.yo
