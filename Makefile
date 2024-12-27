# Define the output directories
OUTDIR = build
OBJDIR = $(OUTDIR)/obj
BINDIR = $(OUTDIR)/bin
LIBDIR = $(OUTDIR)/lib
INCDIR = include
SRCDIR = src
TESTDIR = tests
# EXAMPLESDIR = examples

# Set the compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fPIC
LDFLAGS = -L$(LIBDIR)

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
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $(SRCDIR)/compra.cpp -o $(OBJDIR)/libcompra.o
	$(CXX) -shared $(OBJDIR)/libcompra.o -o $(LIBDIR)/libcompra$(LIBEXT)

# Build tests
tests: $(OBJDIR) $(BINDIR)
	@echo "Building tests..."

	@echo "Building framework..."
	$(CXX) $(CXXFLAGS) -c $(TESTDIR)/framework/test_framework.cpp -o $(OBJDIR)/unit.test_framework.o

	@echo "Building test executable..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -I$(TESTDIR)/framework -c $(TESTDIR)/unit.cpp -o $(OBJDIR)/unit.o

	$(CXX) $(CXXFLAGS) $(OBJDIR)/unit.o $(OBJDIR)/unit.test_framework.o -o $(BINDIR)/unit$(APPEXT) $(LDFLAGS) -lcompra

# Build examples
# examples: $(EXAMPLESDIR) $(BINDIR)
# 	@echo "Building examples..."

# 	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $(EXAMPLESDIR)/cli.cpp -o $(OBJDIR)/cli.o

# 	$(CXX) $(CXXFLAGS) $(OBJDIR)/cli.o -o $(BINDIR)/cli$(APPEXT) $(LDFLAGS) -lcompra

install: libraries
	@echo "Installing binaries, libraries, and headers..."
	mkdir -p $(DESTDIR)/bin
	mkdir -p $(DESTDIR)/lib
	mkdir -p $(DESTDIR)/include

	cp $(LIBDIR)/libcompra$(LIBEXT) $(DESTDIR)/lib/

	mkdir -p $(DESTDIR)/include/compra
	cp $(INCDIR)/compra/*.h $(DESTDIR)/include/compra/

uninstall:
	@echo "Uninstalling binaries, libraries, and headers..."
	rm -f $(DESTDIR)/bin/unit$(APPEXT)
	rm -f $(DESTDIR)/lib/libcompra$(LIBEXT)
	rm -f $(DESTDIR)/include/compra/*.h

reinstall: uninstall install

# Clean up files
clean-objs:
	rm -rf $(OBJDIR)

clean-bins:
	rm -rf $(BINDIR)

clean-libs:
	rm -rf $(LIBDIR)

clean-all: clean-objs clean-bins clean-libs

clean: clean-all

.PHONY: all only-lib only-test libraries tests clean-objs clean-bins clean-libs clean-all clean install uninstall reinstall