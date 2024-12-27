# Define the output directories
OUTDIR = build
OBJDIR = $(OUTDIR)/obj
BINDIR = $(OUTDIR)/bin
LIBDIR = $(OUTDIR)/lib
TESTDIR = tests
# EXAMPLESDIR = examples

# Set the compiler and flags
# CC = clang
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -fPIC
LDFLAGS = -L$(BINDIR)

ifeq ($(shell uname), Linux)
  LIBEXT = .so
  APPEXT = 
else
  LIBEXT = .dll
  APPEXT = .exe
endif

# Default target
# Target for building libraries only
only-lib: libraries

# Target for building all project (with tests)
all: libraries tests

# Target for building tests only
only-test: tests
 
# # Target for building examples only
# only-examples: examples

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# Build libraries
libraries: $(OBJDIR) $(LIBDIR)
	@echo "Building compra library..."
	$(CXX) $(CXXFLAGS) -Iinclude -c src/compra.cpp -o $(OBJDIR)/libcompra.o
	$(CXX) -shared $(OBJDIR)/libcompra.o -o $(LIBDIR)/libcompra$(LIBEXT)

# Build tests
tests: $(OBJDIR) $(BINDIR)
	@echo "Building tests..."

	@echo "Building framework..."
	$(CXX) $(CXXFLAGS) -c $(TESTDIR)/framework/test_framework.cpp -o $(OBJDIR)/unit.test_framework.o

	@echo "Building test executable..."
	$(CXX) $(CXXFLAGS) -Iinclude -I$(TESTDIR)/framework -c $(TESTDIR)/unit.cpp -o $(OBJDIR)/unit.o

	$(CXX) $(CXXFLAGS) $(OBJDIR)/unit.o $(OBJDIR)/unit.test_framework.o -o $(BINDIR)/unit$(APPEXT) $(LDFLAGS) -lcompra

# Build examples
# examples: $(OBJDIR) $(BINDIR)
# 	@echo "Building examples..."

# Clean up files
clean-objs:
	rm -rf $(OBJDIR)

clean-bins:
	rm -rf $(BINDIR)

clean-libs:
	rm -rf $(LIBDIR)

clean-all: clean-objs clean-bins clean-libs

clean: clean-all

.PHONY: all only-lib only-test libraries tests clean-objs clean-bins clean-libs clean-all clean