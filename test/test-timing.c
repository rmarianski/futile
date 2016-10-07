#include <time.h>
#include "tile.h"
#include <stdlib.h>

void noop(futile_coord_s *coord, void *ignored) {
}

unsigned int now() {
    return time(NULL);
}

int main(int argc, char *argv[]) {

    unsigned int start, took, zoom_start, zoom_until;
    zoom_start = 0;
    zoom_until = 16;

    start = now();
    futile_for_zoom_range(zoom_start, zoom_until, noop, NULL);
    took = now() - start;
    printf("for zoom range: %d\n", took);

    futile_coord_cursor_s cursor = {};
    // cursor.zoom_start = zoom_start;
    cursor.zoom_until = zoom_until;
    size_t n = 100;
    futile_coord_s *coords = malloc(sizeof(futile_coord_s) * n);
    futile_coord_group_s group = {.coords=coords, .n=n};

    start = now();
    while (1) {
        if (futile_for_zoom_range_array(&cursor, &group)) {
            break;
        }
    }
    free(coords);
    took = now() - start;
    printf("array: %d\n", took);
}
