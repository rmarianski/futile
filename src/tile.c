#include <math.h>
#include "coord.h"
#include "geo.h"
#include "tile.h"

extern void futile_for_zoom_range(int zoom_start, int zoom_until, futile_coord_fn for_coord, void *userdata) {
    for (int z = zoom_start; z <= zoom_until; z++) {
        int limit = pow(2, z);
        for (int x = 0; x < limit; x++) {
            for (int y = 0; y < limit; y++) {
                futile_coord_s coord = {.x = x, .y = y, .z = z};
                for_coord(&coord, userdata);
            }
        }
    }
}

extern void futile_for_coord_parents(futile_coord_s *start, int zoom_until, futile_coord_fn for_coord, void *userdata) {
    futile_coord_s coord = *start;
    while (coord.zoom >= zoom_until) {
        for_coord(&coord, userdata);
        futile_coord_zoom(-1, &coord);
    }
}

extern long futile_n_for_zoom(int zoom) {
    // geometric series, each zoom containing 4 times more tiles
    return (1 - pow(4, zoom + 1)) / -3;
}

extern void futile_for_bounds(futile_bounds_s *bounds, int zoom_start, int zoom_until, futile_coord_fn for_coord, void *userdata) {
    futile_coord_s coords[2];
    int n_coords = futile_bounds_to_coords(bounds, zoom_start, coords);
    int start_x, until_x, start_y, until_y;
    if (n_coords == 2) {
        start_x = coords[0].x;
        start_y = coords[0].y;
        until_x = coords[1].x;
        until_y = coords[1].y;
    } else {
        start_x = until_x = coords[0].x;
        start_y = until_y = coords[0].y;
    }
    futile_coord_s coord_zoom = {.x=start_x, .y=start_y, .z=zoom_start};
    for (int z = zoom_start; z <= zoom_until; z++) {
        futile_coord_s coord_row = coord_zoom;
        for (int y = start_y; y <= until_y; y++) {
            futile_coord_s coord_column = coord_row;
            for (int x = start_x; x <= until_x; x++) {
                for_coord(&coord_column, userdata);
                coord_column.x++;
            }
            coord_row.y++;
        }
        futile_coord_zoom(1, &coord_zoom);
        start_y *= 2;
        until_y *= 2;
        start_x *= 2;
        until_x *= 2;
    }
}
