#include <math.h>
#include "coord.h"
#include "geo.h"
#include "tile.h"

extern void futile_for_zoom_range(unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata) {
    for (unsigned int z = zoom_start; z <= zoom_until; z++) {
        int limit = pow(2, z);
        for (int x = 0; x < limit; x++) {
            for (int y = 0; y < limit; y++) {
                futile_coord_s coord = {.x = x, .y = y, .z = z};
                for_coord(&coord, userdata);
            }
        }
    }
}

extern bool futile_for_zoom_range_array(futile_coord_cursor_s *cursor, futile_coord_group_s *group) {
    unsigned int coord_index = 0;
    unsigned int x = cursor->x;
    unsigned int y = cursor->y;
    unsigned int z = cursor->z;
    bool is_group_complete = true;
    for (; is_group_complete && z <= cursor->zoom_until; z++) {
        unsigned int limit = pow(2, z);
        for (; is_group_complete && x < limit; x++) {
            for (; is_group_complete && y < limit; y++) {
                if (coord_index >= group->n) {
                    is_group_complete = false;
                    cursor->x = x;
                    cursor->y = y;
                    cursor->z = z;
                    break;
                }
                futile_coord_s coord = {.x = x, .y = y, .z = z};
                group->coords[coord_index++] = coord;
            }
            y = 0;
        }
        x = 0;
    }
    group->n = coord_index;
    return is_group_complete;
}

// TODO test
extern void futile_for_coord_zoom_range(unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y, unsigned int start_zoom, unsigned int end_zoom, futile_coord_fn for_coord, void *userdata) {
    unsigned int zoom_multiplier = 1;
    // all the "end" parameters are inclusive
    // bump them all up here to make them exclusive for range
    end_x += 1;
    end_y += 1;
    end_zoom += 1;
    for (unsigned int zoom_index = start_zoom; zoom_index < end_zoom; zoom_index++) {
        for (unsigned int x_index = start_x * zoom_multiplier;
             x_index < end_x * zoom_multiplier;
             x_index++) {
            for (unsigned int y_index = start_y * zoom_multiplier;
                 y_index < end_y * zoom_multiplier;
                 y_index++) {
                futile_coord_s coord = {.x = x_index, .y = y_index, .z = zoom_index};
                for_coord(&coord, userdata);
            }
        }
        zoom_multiplier *= 2;
    }
}


extern void futile_for_coord_parents(futile_coord_s *start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata) {
    futile_coord_s coord = *start;
    while (coord.z >= zoom_until) {
        for_coord(&coord, userdata);
        futile_coord_zoom(-1, &coord);
    }
}

extern long futile_n_for_zoom(unsigned int zoom) {
    // geometric series, each zoom containing 4 times more tiles
    return (1 - pow(4, zoom + 1)) / -3;
}

extern void futile_for_bounds(futile_bounds_s *bounds, unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata) {
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
    for (unsigned int z = zoom_start; z <= zoom_until; z++) {
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

void futile_for_zoom_range_group(futile_bounds_s *bounds, unsigned int zoom_start, unsigned int zoom_until, futile_coord_cursor_s *cursor, futile_coord_group_s *coords) {
}
