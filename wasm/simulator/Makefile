CC=g++
CFLAGS=-Wall -g

TARGET=sim
BUILD_DIR=build

.PHONY: all $(TARGET) test

all: clean $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

# Source code in current folder
SRCS=state.cpp storage.cpp sim_interface.cpp sim_yis.cpp main.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Source code in dependent folder
DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=input.cpp output.cpp isa.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Final target
$(TARGET): $(DEP_COMMON_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

run:
	$(BUILD_DIR)/sim

debug:
	gdb -q $(BUILD_DIR)/sim \
	-ex "start"
