CC=g++
CLAGS=-Wall

LIB=-lws2_32
INC=-I./include

SRC_DIR=src
BUILD_DIR=build

TARGET = $(BUILD_DIR)/main

SOURCES     := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS     := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.cpp=.o))

dir_guard=@mkdir -p $(@D)

.phony: all clean

all: $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(dir_guard)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB) 

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(dir_guard)
	$(CC) $(CFLAGS) $(INC) -c $^ -o $@ 

clean:
	rm -rf $(BUILD_DIR)
