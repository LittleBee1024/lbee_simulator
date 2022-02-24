CC=g++
CFLAGS=-Wall -g
# http://dinosaur.compilertools.net/flex/manpage.html
LEX=flex

BUILDDIR=./build

LEX_GEN=lexer_gen.cpp
SRCS := isa.cpp lexer.cpp output.cpp input.cpp
OBJS := $(LEX_GEN:%.cpp=$(BUILDDIR)/%.o) $(SRCS:%.cpp=$(BUILDDIR)/%.o)

TEST_YAS=test_yas
TEST_YAS_SRC  := test_yas.cpp
TEST_YAS_OBJS := $(TEST_YAS_SRC:%.cpp=$(BUILDDIR)/%.o)

all: clean $(TEST_YAS)

$(LEX_GEN): lexer.lex
	$(LEX) -P yas -o $@ $<

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_YAS): $(OBJS) $(TEST_YAS_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILDDIR)/$@

run_yas:
	$(BUILDDIR)/$(TEST_YAS) code.ys && diff code.yo code.yo.golden && cat code.yo

clean:
	rm -rf $(LEX_GEN) $(BUILDDIR) *.o *.yo
