# ===========================
# SCUC Makefile (onefile + modular)
# ===========================
# Usage:
#   make -j
#   make onefile -j
#   make modular -j
#   make run_modular ARGS="test_cases/case14/2017-01-01.json"
#   make run_onefile ARGS="test_cases/case14/2017-01-01.json"
#   make clean
#   make distclean
# ===========================

# ===========================
# Compiler / Build settings
# ===========================
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O3 -march=native -Wno-deprecated-declarations
CPPFLAGS ?=
LDFLAGS  ?=
LDLIBS   ?=

# OR-Tools + Abseil typically benefit from pthread on Unix-like systems
CXXFLAGS += -pthread
LDFLAGS  += -pthread

# ===========================
# PATHS (edit if needed)
# ===========================
LOCAL_PATH ?= /usr/local

# Homebrew prefix (auto-detect; can override: `make HB_PATH=/opt/homebrew`)
ifeq ($(HB_PATH),)
  ifneq ($(wildcard /opt/homebrew),)
    HB_PATH := /opt/homebrew
  else
    HB_PATH := /usr/local
  endif
endif

# ===========================
# Include paths
# ===========================
CPPFLAGS += -I$(LOCAL_PATH)/include
CPPFLAGS += -I$(HB_PATH)/include
CPPFLAGS += -I$(HB_PATH)/opt/eigen/include/eigen3
CPPFLAGS += -I$(HB_PATH)/opt/nlohmann-json/include
CPPFLAGS += -Iinclude

CPPFLAGS += -DOR_TOOLS_DLL
CPPFLAGS += -DPROTOBUF_USE_DLLS
CPPFLAGS += -DOR_PROTO_DLL=

# ===========================
# Linker flags / Libraries
# ===========================

# OR-Tools library
LDFLAGS += -L$(LOCAL_PATH)/lib
LDLIBS  += -lortools

# Dependencies libs
LDFLAGS += -L$(HB_PATH)/lib
LDFLAGS += -L$(HB_PATH)/opt/protobuf/lib
LDFLAGS += -L$(HB_PATH)/opt/abseil/lib
LDLIBS  += -lprotobuf

LDLIBS += -labsl_log_internal_message
LDLIBS += -labsl_log_internal_check_op
LDLIBS += -labsl_strings
LDLIBS += -labsl_strings_internal
LDLIBS += -labsl_time
LDLIBS += -labsl_int128
LDLIBS += -labsl_base
LDLIBS += -labsl_raw_logging_internal
LDLIBS += -labsl_spinlock_wait
LDLIBS += -labsl_throw_delegate
LDLIBS += -labsl_str_format_internal

# Runtime Search Paths (rpath)
LDFLAGS += -Wl,-rpath,$(LOCAL_PATH)/lib
LDFLAGS += -Wl,-rpath,$(HB_PATH)/lib

# ===========================
# Targets / Sources
# ===========================

# --- One-file version ---
ONEFILE_TARGET := scuc_onefile
ONEFILE_SRC    := scuc3.cpp

# --- Modular version ---
MODULAR_TARGET := scuc_modular
MODULAR_SRCS   := \
  src/main.cpp \
  src/Timer.cpp \
  src/JsonUtils.cpp \
  src/NetworkFactors.cpp \
  src/SolverFactory.cpp \
  src/InputLoader.cpp \
  src/VariableManager.cpp \
  src/ObjectiveBuilder.cpp \
  src/ConstraintBuilder.cpp \
  src/SolutionExporter.cpp \
  src/SCUCSolver.cpp


# Build directory for object files
BUILD_DIR := build_make

# Objects + dependency files (.d)
ONEFILE_OBJ   := $(BUILD_DIR)/onefile/$(ONEFILE_SRC:.cpp=.o)
MODULAR_OBJS  := $(patsubst src/%.cpp,$(BUILD_DIR)/modular/%.o,$(MODULAR_SRCS))
DEPS          := $(ONEFILE_OBJ:.o=.d) $(MODULAR_OBJS:.o=.d)

# ===========================
# Default target: build both
# ===========================
.PHONY: all
all: onefile modular

# ===========================
# Help
# ===========================
.PHONY: help
help:
	@echo "Targets:"
	@echo "  make / make all        Build both: $(ONEFILE_TARGET) and $(MODULAR_TARGET)"
	@echo "  make onefile           Build $(ONEFILE_TARGET)"
	@echo "  make modular           Build $(MODULAR_TARGET)"
	@echo "  make run_onefile ARGS=\"...\"   Run onefile with ARGS"
	@echo "  make run_modular ARGS=\"...\"   Run modular with ARGS"
	@echo "  make clean             Remove objects/deps only"
	@echo "  make distclean         Remove objects/deps and binaries"
	@echo ""
	@echo "Variables you can override:"
	@echo "  CXX=..., LOCAL_PATH=..., HB_PATH=..."

# ===========================
# Build onefile
# ===========================
.PHONY: onefile
onefile: $(ONEFILE_TARGET)

$(ONEFILE_TARGET): $(ONEFILE_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# ===========================
# Build modular
# ===========================
.PHONY: modular
modular: $(MODULAR_TARGET)

$(MODULAR_TARGET): $(MODULAR_OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# ===========================
# Compile rules (with deps)
# ===========================

# onefile objects
$(BUILD_DIR)/onefile/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

# modular objects
$(BUILD_DIR)/modular/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# ===========================
# Run helpers
# ===========================
.PHONY: run_onefile run_modular
run_onefile: $(ONEFILE_TARGET)
	./$(ONEFILE_TARGET) $(ARGS)

run_modular: $(MODULAR_TARGET)
	./$(MODULAR_TARGET) $(ARGS)

# ===========================
# Clean
# ===========================
.PHONY: clean distclean
clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -f $(ONEFILE_TARGET) $(MODULAR_TARGET)
