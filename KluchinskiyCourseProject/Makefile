CXX ?= g++
CXXFLAGS += -g -Wall -pedantic -std=gnu++20 -fPIC
CXXPOSTFLAGS += -lpqxx -lpq
SRCDIR := src
OBJDIR := obj

SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(filter-out $(OBJDIR)/daemon.o, $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC)))

all: $(OBJ) ScanDaemon
	$(CXX) $(CXXFLAGS) -o ScanDaemon $(SRCDIR)/daemon.cpp $(OBJ) $(CXXPOSTFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(CXXPOSTFLAGS)

ScanDaemon: $(OBJ)
	$(CXX) -shared -o libnewnotify.so $^ $(CXXPOSTFLAGS)
	ar cr libnewnotify.a $^

static: $(OBJ)
	ar cr libnewnotify.a $^

clean:
	rm -f $(OBJDIR)/*.o
	rm -f *.so
	rm -f *.a
	rm -f daemon
	rm -f *.out