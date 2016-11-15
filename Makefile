P=futile
OBJECTS=$(P).o
TEST=test-futile
CFLAGS=-Wall -g -std=gnu11 -fPIC -O3
LDLIBS=-lm
DESTDIR=$(HOME)/opt

all: shared static

shared: lib$(P).so
static: lib$(P).a

lib$(P).so: $(OBJECTS)
	$(CC) -shared -o lib$(P).so $(OBJECTS)

lib$(P).a: $(OBJECTS)
	ar rcs lib$(P).a $(OBJECTS)

$(P).o: $(P).h
$(TEST).o: $(P).h

$(TEST).o: CFLAGS += `pkg-config --cflags glib-2.0`
$(TEST): LDLIBS += `pkg-config --libs glib-2.0`

check: $(TEST)
	./test-futile

$(TEST): $(TEST).o

clean:
	rm -f lib$(P).so lib$(P).a $(TEST) $(TEST).o $(OBJECTS)

install: all
	cp -f $(P).h $(DESTDIR)/include
	cp -f lib$(P).so lib$(P).a $(DESTDIR)/lib

.PHONY: check clean shared static install all
