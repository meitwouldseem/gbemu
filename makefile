CPP=g++
CPPFLAGS=-std=c++14 -Wall -fexceptions -g -Iinclude
LINKFLAGS=-lsfml-graphics -lsfml-window -lsfml-system

BUILD_DIR=obj
BIN_NAME=emu
BIN_DIR=bin

CPP_SRCS := $(wildcard *.cpp) $(wildcard */*.cpp)
CPP_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SRCS))

.PHONY: all clean

all: $(CPP_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CPP) -o $(BIN_DIR)/$(BIN_NAME) $(CPP_OBJS) $(LINKFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CPP) -c $< -o $@ $(CPPFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
