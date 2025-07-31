#talgov44@gmail.com

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -g

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = test

# Add include directory to the compiler's search path.
# This allows it to find all headers, including doctest.h.
CXXFLAGS += -I$(INC_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system


TARGET_MAIN = Main
TARGET_TEST_EXEC = coup_test # The filename of the test executable


# Default 'all' target only builds the main executable.
# The test executable is built on demand by 'make test'.
all: $(TARGET_MAIN)

# Rule to create the main executable
$(TARGET_MAIN): $(OBJS) Demo.cpp
	@echo "--- Linking Main Executable ---"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)
	@echo "--- Setting Execute Permissions for Main ---"
	chmod +x $@

# Rule to BUILD the test executable. This is a prerequisite for the 'test' command.
$(TARGET_TEST_EXEC): $(OBJS) $(TEST_DIR)/Test.cpp
	@echo "--- Linking Test Executable ---"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "--- Setting Execute Permissions for Test ---"
	chmod +x $@

# Generic rule to compile source files from src/ into object files in obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create the object directory if it doesn't exist
# This is an order-only prerequisite.
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Phony target to RUN the tests. User types 'make test'.
# This depends on the EXECUTABLE file being up-to-date first.
test: $(TARGET_TEST_EXEC)
	@echo "--- Running Tests ---"
	./$(TARGET_TEST_EXEC)

valgrind: $(TARGET_MAIN)
	valgrind --leak-check=full ./$(TARGET_MAIN)
	make ./coup_test
	valgrind --leak-check=full ./coup_test


clean:
	@echo "--- Cleaning Up Build Files ---"
	rm -rf $(OBJ_DIR) $(TARGET_MAIN) $(TARGET_TEST_EXEC)


.PHONY: all test valgrind clean