BUILDMAKEFILE := build/build.ninja
VERBOSE := true
CMAKE_BUILD_TYPE ?= Debug

$(BUILDMAKEFILE): $(shell find . -name CMakeLists.txt)
	cmake -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
	      -B build \
	      VERBOSE=$(VERBOSE) .

.PHONY: compile
compile: $(BUILDMAKEFILE)
	cmake --build build
