# Compiler and Archiver
CC=emcc
AR=emar
CXX=em++

# Compiler Flags
DEBUG_LEVEL = -g2
CXX_STANDARD = -std=c++11
EXTRA_CCFLAGS = 
CXXFLAGS = -O3 $(DEBUG_LEVEL) $(CXX_STANDARD) $(EXTRA_CCFLAGS) -MMD -MP
PREDCXXFLAGS = -O0 $(DEBUG_LEVEL) $(CXX_STANDARD) $(EXTRA_CCFLAGS) -MMD -MP

# Directories
BUILD = build
TETGEN = tetgen

# Source and Object Files
TETGEN_SOURCES = $(wildcard $(TETGEN)/*.cxx)
TETGEN_OBJECTS = $(patsubst $(TETGEN)/%.cxx,$(BUILD)/%.o,$(TETGEN_SOURCES))

MAIN_SRC = main.cxx

# Output
TARGET = babylon2tet.js

# Emscripten Flags
EMSCRIPTEN_FLAGS = \
    -s ENVIRONMENT=web \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    --memory-init-file 0 \
    -s FORCE_FILESYSTEM=1 \
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
    -s ASSERTIONS=2 \
    -s SAFE_HEAP=0 \
    -s ALIASING_FUNCTION_POINTERS=0 \
    -s DISABLE_EXCEPTION_CATCHING=2 \
    -s MAXIMUM_MEMORY=2147483648 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="createTetGenModule"

# Phony Targets
.PHONY: all babylon2tet clean

# Default Target
all: babylon2tet

# Ensure build directory exists
$(BUILD):
	mkdir -p $(BUILD)

# Pattern Rule for Compiling TetGen Sources
$(BUILD)/%.o: $(TETGEN)/%.cxx | $(BUILD)
	$(CXX) $(CXXFLAGS) -DSELF_CHECK -DNDEBUG -DTETLIBRARY -c $< -o $@

# Link the final JavaScript module
babylon2tet: $(TETGEN_OBJECTS) $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -I $(TETGEN) $(TETGEN_OBJECTS) $(MAIN_SRC) -o babylon2tet.js $(EMSCRIPTEN_FLAGS)

# Clean build artifacts
clean:
	rm -rf $(BUILD) $(TARGET)

# Include dependency files
-include $(TETGEN_OBJECTS:.o=.d)