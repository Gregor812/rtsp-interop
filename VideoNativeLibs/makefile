CC = gcc
CXX = g++

SOURCES = remuxing.c streams.cpp
OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)
LIBS = -lavdevice -lavfilter -lavformat -lavcodec -lavutil
OUTLIB = libRtspSlicer.so

COMMON_FLAGS = -g -fPIC -Wall -I.
LDFLAGS = -shared -g

all: $(OUTLIB)

$(OUTLIB): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(COMMON_FLAGS) -std=c++11 -c $< -o $@

%.o: %.c
	$(CC) $(COMMON_FLAGS) -std=c11 -c $< -o $@

.PHONY: clean

clean:
	rm $(OBJECTS) $(OUTLIB) || exit 0
