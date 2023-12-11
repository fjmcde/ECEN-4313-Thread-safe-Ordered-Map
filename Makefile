# Compiler setup
CC = g++
CFLAGS = -Wall -Wextra -O3 -g -pthread --std=c++2a

# Optional DEBUG compiler flag for compiling with debugging code:
# $ make DEBUG=1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

# Source files
SRC_DIR     = src
MAIN_FILE   = main.cpp
SRC_FILES   = $(wildcard $(SRC_DIR)/*.cpp)

# Header files
HEADER_DIR      = headers
HEADER_FILES    = $(wildcard $(HEADER_DIR)/*.h)

# Object files
OBJ_DIR     = obj
OBJ_FILES   = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES)) $(OBJ_DIR)/main.o

# # Answer files
# FILES_DIR   = files
# SORTED_DIR	= sorted
# ANS_FILES   = $(wildcard $(SORTED_DIR)/*.ans)

# # Autograde files
# AUTOGRADE_DIR       = autograde_tests
# AUTOGRADE_TESTS     = $(wildcard $(AUTOGRADE_DIR)/*.my)

# Output executable name
EXECUTABLE = myMap

# Cross-platform settings
ifeq ($(OS), Windows_NT)
    # Windows configuration
    RM                          = del /Q /F
    RM_EXECUTABLE_EXTENSION     = .exe
    RM_OBJ_FILES                = $(subst /, \, $(OBJ_FILES))
    RM_ANS_FILES                = $(subst /, \, $(ANS_FILES))
    RM_AG_FILES                 = $(subst /, \, $(AUTOGRADE_TESTS))
    RMDIR                       = rmdir /S /Q
    MKDIR                       = mkdir
else
    # Unix configuration
    RM                          = rm -f
    RM_EXECUTABLE_EXTENSION     = ""
    RM_OBJ_FILES                = $(OBJ_FILES)
    RM_ANS_FILES                = $(ANS_FILES)
    RM_AG_FILES                 = $(AUTOGRADE_TESTS)
    RMDIR                       = rm -rf
    MKDIR                       = mkdir
endif

#################
#    Targets    #
#################

# Executable target
# Generate .exe
$(EXECUTABLE): $(OBJ_FILES) $(OBJ_DIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^

# Object directory target
# Having object files everywhere makes me crazy; so
# put them all in the same place
$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)


# Object file(s) target
# Generate object files from source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_FILES) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o $@ $<

# Cleanup object files/directory, and executable
clean:
	$(RM) $(RM_OBJ_FILES) $(EXECUTABLE)$(RM_EXECUTABLE_EXTENSION)
	$(RMDIR) $(OBJ_DIR)

# Not real build targets
.PHONY: clean