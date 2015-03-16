# Futile

Futile (also pronounced "eff you" tile) is a library to provide some basic
functionality surrounding tile operations. It assumes an x/y/z gridset.

## Installation

A gnu c compiler is required, but no other libraries are necessary to build.

    make

glib is required to run the tests.

    apt-get install libglib2.0-dev

To run the tests:

    make check

## Example usage

```
#include <stdio.h>
#include <assert.h>
#include "coord.h"

int main() {
    futile_coord_s coord = {.column=1, .row=2, .zoom=3};
    char coord_str[16];
    assert(futile_coord_serialize(&coord, sizeof(coord_str), coord_str));
    printf("%s\n", coord_str);
    return 0;
}
```

## Documentation

    make doc
