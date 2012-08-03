CFLAGS=-g -O2 -Wall -Wextra -Isrc $(OPTFLAGS)
LDFLAGS=-lncurses

TARGET=build/cave
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
DEPS=$(patsubst %.c,%.d,$(SOURCES))

all: $(TARGET)

-include $(DEPS)

%.o: %.d %.c
	$(CC) $(CFLAGS) -c -o $@ $*.c

%.d: %.c
	$(CC) $(CFLAGS) -MM -MT '$*.o $*.d' -o $@ $<

build:
	@mkdir -p build

$(TARGET): build $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

run: $(TARGET)
	./build/cave 2>>/tmp/cave.err

valgrind: $(TARGET)
	valgrind --log-file=/tmp/valgrind.log ./build/cave 2>>/tmp/cave.err

check-syntax: CFLAGS=-g -Wall -Wextra -Isrc $(OPTFLAG)
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(CHK_SOURCES)

clean:
	rm -rf build $(OBJECTS) $(DEPS)
