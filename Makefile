# Usage:
# make babylon2tet       # compile emscripten wasm files
# make clean             # clean build artifacts

# Compiler and Archiver
CC=emcc
AR=emar
CXX=em++

# Compiler Flags
DEBUG_LEVEL = -g2
CXX_STANDARD = -std=c++11
EXTRA_CCFLAGS = 
CXXFLAGS = -O2 $(DEBUG_LEVEL) $(CXX_STANDARD) $(EXTRA_CCFLAGS)
PREDCXXFLAGS = -O0 $(DEBUG_LEVEL) $(CXX_STANDARD) $(EXTRA_CCFLAGS)

# Directories
BUILD = build
TETGEN = tetgen

# Source Files
TETGEN_SOURCES = $(TETGEN)/predicates.cxx $(TETGEN)/tetgen.cxx
TETGEN_OBJECTS = $(BUILD)/predicates.o $(BUILD)/libtetgen.o
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
    -s MAXIMUM_MEMORY=2147483648 # 2GB in bytes

# Phony Targets
.PHONY: all babylon2tet clean

# Default Target
all: babylon2tet

# Ensure build directory exists
$(BUILD):
	mkdir -p $(BUILD)

# Compile predicates.o
$(BUILD)/predicates.o: $(TETGEN)/predicates.cxx | $(BUILD)
	$(CXX) $(PREDCXXFLAGS) -DSELF_CHECK -DNDEBUG -DTETLIBRARY -c $(TETGEN)/predicates.cxx -o $(BUILD)/predicates.o

# Compile tetgen.o
$(BUILD)/libtetgen.o: $(TETGEN)/tetgen.cxx | $(BUILD)
	$(CXX) $(CXXFLAGS) -DSELF_CHECK -DNDEBUG -DTETLIBRARY -c $(TETGEN)/tetgen.cxx -o $(BUILD)/libtetgen.o

# Link the final JavaScript module
babylon2tet: $(TETGEN_OBJECTS) $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -I $(TETGEN) $(TETGEN_OBJECTS) $(MAIN_SRC) -o $(TARGET) $(EMSCRIPTEN_FLAGS)

# Clean build artifacts
clean:
	rm -rf $(BUILD) $(TARGET)
