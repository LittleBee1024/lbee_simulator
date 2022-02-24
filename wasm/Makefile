CC     = em++
CFLAGS = -Wall -Wno-misleading-indentation -g
LFLAGS = --no-entry -s 'EXPORTED_FUNCTIONS=['_malloc', '_free']' -g

BUILDDIR=./build
SRCDIR=./src

TARGET = web_api
SRCS = isa.cpp lexer_gen.cpp lexer.cpp output.cpp input.cpp api.cpp
OBJS = $(SRCS:%.cpp=$(BUILDDIR)/%.o)

all: clean $(TARGET)

.PHONY: all $(TARGET) clean

$(BUILDDIR)/lexer_gen.o:
	make lexer_gen.cpp -C $(SRCDIR)
	$(CC) -c $(CFLAGS) $(SRCDIR)/lexer_gen.cpp -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) -c $(CFLAGS) $(TARGET).cpp -o $(BUILDDIR)/$(TARGET).o
	$(CC) $(LFLAGS) $^ $(BUILDDIR)/$(TARGET).o -o $@.js

clean:
	make clean -C $(SRCDIR)
	rm -rf *.o *.wasm $(TARGET).js $(BUILDDIR)
