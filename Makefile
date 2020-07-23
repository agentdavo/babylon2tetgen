# Usage:
# make babylon2tet       # compile emscripten wasm files

CC=emcc
AR=emar
CXX=em++

DEBUG_LEVEL = -g3
EXTRA_CCFLAGS = 
CXXFLAGS = -O0 $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)
PREDCXXFLAGS = -O0 $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)

BUILD = build
TETGEN = tetgen

.PHONY: all

predicates.o: $(TETGEN)/predicates.cxx
	$(CXX) $(PREDCXXFLAGS) -DSELF_CHECK -DNDEBUG -DTETLIBRARY -c $(TETGEN)/predicates.cxx -o $(BUILD)/predicates.o

libtetgen.o: $(TETGEN)/tetgen.cxx 
	$(CXX) $(CXXFLAGS) -DSELF_CHECK -DNDEBUG -DTETLIBRARY -c $(TETGEN)/tetgen.cxx -o $(BUILD)/libtetgen.o

babylon2tet: predicates.o libtetgen.o main.cxx
	$(CXX) $(CXXFLAGS) -I $(TETGEN) $(BUILD)/predicates.o $(BUILD)/libtetgen.o main.cxx -o babylon2tet.js \
	-s ENVIRONMENT=web \
	-s WASM=1 \
	-s ALLOW_MEMORY_GROWTH=1 \
	--memory-init-file 0 \
    -s FORCE_FILESYSTEM=1 \
	-s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap', 'babylon2tetgen']" \
    -s ASSERTIONS=2 \
    -s SAFE_HEAP=0 -s ALIASING_FUNCTION_POINTERS=0 \
    -s DISABLE_EXCEPTION_CATCHING=2 \
    -s MAXIMUM_MEMORY=2GB \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s TOTAL_MEMORY=33554432
