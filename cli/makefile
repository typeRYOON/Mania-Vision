.PHONY: all setup run clean

CXX      = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -DWIN32

SRCDIR  = src
S1      = $(SRCDIR)/main.cpp
S2      = $(SRCDIR)/maniaparser.cpp
S3      = $(SRCDIR)/maniaplayer.cpp
SOURCES = $(S1) $(S2) $(S3)

OBJDIR  = obj
O1      = $(OBJDIR)/main.o
O2      = $(OBJDIR)/maniaparser.o
O3      = $(OBJDIR)/maniaplayer.o
OBJECTS = $(O1) $(O2) $(O3)

ARGS    = test.txt

# Main build target
BINDIR = bin
TARGET = bin/mania-vision-cli.exe
all: $(TARGET)

run: $(TARGET)
	@clear
	@$(TARGET) $(ARGS)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Windows is not allowing $(OBJDIR)/%.o rules bruh
$(O1): $(S1)
	@$(CXX) $(CXXFLAGS) -c $(S1) -o $(O1)

$(O2): $(S2)
	@$(CXX) $(CXXFLAGS) -c $(S2) -o $(O2)

$(O3): $(S3)
	@$(CXX) $(CXXFLAGS) -c $(S3) -o $(O3)

setup:
	@mkdir $(OBJDIR)
	@mkdir $(BINDIR)

clean:
	@rm -f $(OBJECTS)
	@rm -f $(TARGET)