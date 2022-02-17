CC = em++
AR = emar
CFLAGS = -Wall -Winline -pipe

STATIC = libutil.a
SRC    = util.cpp
OBJ    = $(SRC:.cpp=.o)

all: $(STATIC)

$(STATIC): $(OBJ)
	@$(AR) rcs $(STATIC) $(OBJ)

.cpp.o:
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a
