CC     = em++
CFLAGS = -Wall -g
LFLAGS = --no-entry -s 'EXPORTED_FUNCTIONS=['_malloc', '_free']' -g

TARGET = web_api
SRC    = $(wildcard *.cpp)
OBJ    = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) $^ -o $@.js

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.wasm $(TARGET).js
