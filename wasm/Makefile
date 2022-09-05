CC     = em++
CFLAGS = -Wall -Wno-misleading-indentation -g
LFLAGS = --no-entry -s 'EXPORTED_FUNCTIONS=['_malloc', '_free']' -g
LEX=flex

BUILD_DIR=./build
TARGET = web_api

all: clean $(TARGET)

.PHONY: all $(TARGET) clean

clean:
	rm -rf *.o *.wasm $(TARGET).js $(BUILD_DIR)

# Source code in current folder
SRCS=web_api.cpp c_api.cpp global.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I. -c $< -o $@

# Source code in simulator dependent folder
DEP_SIM_DIR=./simulator
DEP_SIM_SRCS=sim_yis.cpp sim_interface.cpp storage.cpp state.cpp
DEP_SIM_OBJS=$(DEP_SIM_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_SIM_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I. -c $< -o $@

# Source code in assembler dependent folder
DEP_ASM_DIR=./assembler
DEP_ASM_SRCS=yas_instr.cpp yas.cpp
DEP_ASM_OBJS=$(DEP_ASM_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_ASM_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I. -c $< -o $@

LEX_GEN=$(BUILD_DIR)/yas_gen.cpp
LEX_OBJ=$(LEX_GEN:%.cpp=%.o)

$(LEX_OBJ): $(DEP_ASM_DIR)/yas.lex
	@mkdir -p '$(@D)'
	$(LEX) -P yas_ -o $(LEX_GEN) $<
	$(CC) $(CFLAGS) -I. -c $(LEX_GEN) -o $@

# Source code in _common dependent folder
DEP_COM_DIR=./_common
DEP_COM_SRCS=input.cpp output.cpp isa.cpp
DEP_COM_OBJS=$(DEP_COM_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_COM_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(TARGET): $(OBJS) $(DEP_SIM_OBJS) $(DEP_ASM_OBJS) $(LEX_OBJ) $(DEP_COM_OBJS)
	$(CC) $(LFLAGS) $^ -o $@.js
