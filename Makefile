# Compiler
CXX := g++
CXXFLAGS := -Wall -g -MMD -MP
CPPFLAGS := -I./include -I./vendor

# Directories
SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    EXE := .exe
    LDLIBS := -lws2_32
    MKDIR_P := mkdir
    RM := rmdir /S /Q
else
    EXE :=
    LDLIBS :=
    MKDIR_P := mkdir -p
    RM := rm -rf
endif

# Targets
TARGET := $(BUILD_DIR)/main$(EXE)
TEST_TARGET := $(BUILD_DIR)/test_runner$(EXE)

# Sources and objects
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/src/%.o,$(SOURCES))

LIB_SOURCES := $(filter-out $(SRC_DIR)/main.cpp,$(SOURCES))
LIB_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/src/%.o,$(LIB_SOURCES))

TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))

# Collect generated header dependency files (.d)
DEPS := $(OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@$(MKDIR_P) $(@D)
	$(CXX) $^ -o $@ $(LDLIBS)

$(TEST_TARGET): $(TEST_OBJECTS) $(LIB_OBJECTS)
	@$(MKDIR_P) $(@D)
	$(CXX) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR_P) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@$(MKDIR_P) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Auto-include generated dependency files
-include $(DEPS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	$(RM) $(BUILD_DIR)
