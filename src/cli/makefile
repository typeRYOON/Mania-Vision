# For Mania-Vision project (cli version) :: - -                                        - -

# Compiler related :: - -                                                              - -
CXX      = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -DWIN32

# Source file related :: - -                                                           - -
SRCDIR  = src
S1      = $(SRCDIR)/main.cpp
S2      = $(SRCDIR)/maniaparser.cpp
S3      = $(SRCDIR)/maniaplayer.cpp
SOURCES = $(S1) $(S2) $(S3)

# Object file related :: - -                                                           - -
OBJDIR  = obj
O1      = $(OBJDIR)/main.o
O2      = $(OBJDIR)/maniaparser.o
O3      = $(OBJDIR)/maniaplayer.o
OBJECTS = $(O1) $(O2) $(O3)

# Main build target :: - -                                                             - -
ARGS   = test.txt
BINDIR = bin
TARGET = $(BINDIR)/mania-vision-cli.exe

.PHONY: all run clean setup

# Rules :: - -                                                                         - -
all: setup $(TARGET)

run: $(TARGET)
	@clear
	@$(TARGET) $(ARGS)

clean:
	@rm -f $(OBJECTS)
	@rm -f $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Windows is not allowing $(OBJDIR)/%.o rules :(
$(O1): $(S1)
	@$(CXX) $(CXXFLAGS) -c $(S1) -o $(O1)

$(O2): $(S2)
	@$(CXX) $(CXXFLAGS) -c $(S2) -o $(O2)

$(O3): $(S3)
	@$(CXX) $(CXXFLAGS) -c $(S3) -o $(O3)

setup:
	@-mkdir $(OBJDIR) 2> NUL || true
	@-mkdir $(BINDIR) 2> NUL || true
	@rm -f NUL

# :: - -                                                                               - -
