CC = gcc
CXX = g++

SOURCES = remuxing.c streams.cpp
OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)
LIBS = -lavdevice -lavfilter -lavformat -lavcodec -laom -lavutil \
-lfdk-aac -lm -lmp3lame -lopus -lpostproc -lpthread \
-lswresample -lswscale -lva -lva-drm -lva-x11 -lvdpau \
-lvorbis -lvorbisenc -lvpx -lX11 -lx264 -lx265 -lz
EXECUTABLE = libRtspSlicer.so

COMMON_FLAGS = -g -fPIC -Wall -I./ffmpeg/include -I.
LDFLAGS = -shared -g -L./ffmpeg/lib $(LIBS)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(COMMON_FLAGS) -std=c++11 -c $< -o $@

%.o: %.c
	$(CC) $(COMMON_FLAGS) -std=c11 -c $< -o $@

.PHONY: clean

clean:
	rm $(OBJECTS) $(EXECUTABLE) || exit 0
