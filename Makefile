CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=modplay.c mod_file.c channels.c pattern.c sample.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=modplay

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
