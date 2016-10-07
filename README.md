# Futile

Futile is a library to provide some basic functionality surrounding tile operations. It assumes an x/y/z gridset.

## Installation

A gnu c compiler is required, but no other libraries are necessary to build.

To build:
    make

glib is required to run the tests.

    apt-get install libglib2.0-dev

To run the tests:

    make check

To install headers and libraries:

    make install DESTDIR=${HOME}/opt

## Example usage

```
cat > test.c << EOF
#include <stdio.h>
#include <assert.h>
#include <futile.h>

int main() {
    futile_coord_s coord = {.x=1, .y=2, .z=3};
    char coord_str[16];
    assert(futile_coord_serialize(&coord, sizeof(coord_str), coord_str));
    printf("%s\n", coord_str);
    return 0;
}
EOF
gcc test.c -lfutile -lm
./a.out
```
