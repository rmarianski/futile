P=libfutile
OBJECT_NAMES=coord.o geo.o tile.o
OBJECTS = $(addprefix src/,$(OBJECT_NAMES))
CFLAGS = -g -Wall -std=gnu11 -I./src -fPIC -O3
LDLIBS = -lm
TEST_NAMES = test-coord test-geo test-tile
TESTS = $(addprefix test/,$(TEST_NAMES))
TEST_OBJECTS = $(addsuffix .o,$(TESTS))

.PHONY: all run clean check shared static doc cleandoc

all: shared static

shared: $(P).so

static: $(P).a

$(P).so: $(OBJECTS)
	$(CC) -shared -o $(P).so $(OBJECTS) $(LDLIBS)

$(P).a: $(OBJECTS)
	ar rc $(P).a $(OBJECTS)
	ranlib $(P).a

clean: cleandoc
	rm -f $(P).so $(P).a $(OBJECTS) $(TESTS) $(TEST_OBJECTS) core

check: $(TESTS)
	@for t in $(TESTS); do ./$$t; done

$(TESTS): CFLAGS+=`pkg-config --cflags glib-2.0`
$(TESTS): LDLIBS+=`pkg-config --libs glib-2.0`
$(TESTS): $(OBJECTS)

doc: $(OBJECTS)
	doxygen doxygen.cfg

cleandoc:
	rm -rf doc
