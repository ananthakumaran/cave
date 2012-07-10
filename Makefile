CFLAGS=-g -O2 -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)
LDFLAGS=-lncurses

TARGET=build/cave
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

build:
	@mkdir -p build

$(TARGET): build $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

run: $(TARGET)
	./build/cave

valgrind: $(TARGET)
	valgrind --log-file=/tmp/valgrind.log ./build/cave

check-syntax: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAG)
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(CHK_SOURCES)

clean:
	rm -rf build $(OBJECTS)
