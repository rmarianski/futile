lib_LTLIBRARIES = libfutile.la

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wall

pkginclude_HEADERS = src/coord.h src/geo.h src/tile.h
libfutile_la_SOURCES = src/coord.c src/geo.c src/tile.c

TESTS = $(check_PROGRAMS)
check_PROGRAMS = test-coord test-geo test-tile test-timing
test_coord_SOURCES = test/test-coord.c $(libfutile_la_SOURCES)
test_geo_SOURCES = test/test-geo.c $(libfutile_la_SOURCES)
test_tile_SOURCES = test/test-tile.c $(libfutile_la_SOURCES)
test_timing_SOURCES = test/test-timing.c $(libfutile_la_SOURCES)

testing_cflags = $(AM_CFLAGS) -I$(top_srcdir)/src `pkg-config --cflags glib-2.0`
testing_ldflags = `pkg-config --libs glib-2.0`
test_coord_CFLAGS = $(testing_cflags)
test_coord_LDFLAGS = $(testing_ldflags)
test_geo_CFLAGS = $(testing_cflags)
test_geo_LDFLAGS = $(testing_ldflags)
test_tile_CFLAGS = $(testing_cflags)
test_tile_LDFLAGS = $(testing_ldflags)
test_timing_CFLAGS = $(testing_cflags)
test_timing_LDFLAGS = $(testing_ldflags)

.PHONY: doc
doc:
	test -d doc || mkdir doc
	doxygen doxygen.cfg
