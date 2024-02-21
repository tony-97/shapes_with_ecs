include Makefile.vars

EXEC_NAME := app
LIB_NAME  := mylib
SRC_DIR   := src
ARFLAGS   := rcs
BUILD_NAME := build$(addprefix _,$(call to_lower,$(TARGET)))
BUILD_DIR  := $(BUILD_NAME)

ifndef MSVC
    CPPFLAGS += -I ./external/raylib/src/ -I ./external/oop-ecs/src -I ./external/oop-ecs/external/
    CXXFLAGS += -std=c++20
    RELEASE_FLAGS += -flto
    LDFLAGS += -L ./libs/raylib/$(BUILD_NAME)/debug -flto
    LDLIBS += -lraylib
endif
ifdef EMSCRIPTEN
    CXXFLAGS += -fexperimental-library
endif
ifdef GCC
    LDLIBS += -ltbb
endif

# Compilation flags
ifeq ($(TARGET),WEB)
    AR  := emar
    CC  := emcc
    CXX := em++
    EXEC_NAME := app.html
    LDLIBS   += -sUSE_GLFW=3
    LDFLAGS  += -sASSERTIONS=2
    DEBUG_FLAGS   += -O0 -g -ggdb --profiling 
    RELEASE_FLAGS += 
    WFLAGS   += 
    CPPFLAGS += 
    CXXFLAGS += 
    CFLAGS   += 
endif
ifeq ($(TARGET),ANDROID)
	ANDROID_ARCH ?= arm64
	ANDROID_API_VERSION ?= 29
    LDLIBS   += 
    LDFLAGS  += 
    DEBUG_FLAGS   += 
    RELEASE_FLAGS += 
    WFLAGS   += 
    CPPFLAGS += 
    CXXFLAGS += 
    CFLAGS   += 
endif
ifeq ($(TARGET),WINDOWS)
ifdef MSVC
    LDLIBS   += 
    LDFLAGS  += 
    DEBUG_FLAGS   += 
    RELEASE_FLAGS += 
    WFLAGS   += 
    CPPFLAGS += 
    CXXFLAGS += 
    CFLAGS   += 
else
    LDLIBS   += 
    LDFLAGS  += 
    DEBUG_FLAGS   += -g -ggdb -O0
    RELEASE_FLAGS += -march=native -Ofast -s -DNDEBUG
    WFLAGS   += 
    CPPFLAGS += 
    CXXFLAGS += 
    CFLAGS   += 
endif
endif
ifeq ($(TARGET),LINUX)
    LDLIBS   += 
    LDFLAGS  += 
    DEBUG_FLAGS   += -g -ggdb -O0
    RELEASE_FLAGS += -march=native -Ofast -s -DNDEBUG
    WFLAGS   += 
    CPPFLAGS += 
    CXXFLAGS += 
    CFLAGS   += 
endif
ifeq ($(TARGET),OSX)
    LDLIBS   += 
    LDFLAGS  += 
    DEBUG_FLAGS   += 
    RELEASE_FLAGS += 
    WFLAGS   += 
    CPPFLAGS += 
    CXXFLAGS += 
    CFLAGS   += 
endif
# Add more targets

export

.PHONY: all run run_cgdb info clean cleanall

all:
	$(MAKE) -f Makefile.options all

lib:
	$(MAKE) -f Makefile.options lib

run:
	$(MAKE) -f Makefile.options run

run_valgrind:
	$(MAKE) -f Makefile.options run_valgrind

run_cgdb:
	$(MAKE) -f Makefile.options run_cgdb

info:
	$(MAKE) -f Makefile.options info

clean:
	$(MAKE) -f Makefile.options clean

cleanall:
	$(MAKE) -f Makefile.options cleanall
