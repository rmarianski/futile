#include <math.h>
#include <stdio.h>
#include "futile.h"

extern void futile_coord_zoom(int delta, futile_coord_s *out) {
    out->x *= pow(2, delta);
    out->y *= pow(2, delta);
    out->z += delta;
}

extern bool futile_coord_parent(futile_coord_s *coord, futile_coord_s *out_coord) {
    if (coord->z <= 0) {
        return false;
    }
    if (out_coord != coord) {
        *out_coord = *coord;
    }
    out_coord->z -= 1;
    out_coord->x >>= 1;
    out_coord->y >>= 1;
    return true;
}

extern void futile_coord_children(futile_coord_s *coord, futile_coord_s *out_children) {
    futile_coord_s *first_child = &out_children[0];
    *first_child = *coord;
    first_child->z++;
    first_child->x <<= 1;
    first_child->y <<= 1;

    out_children[1] = *first_child;
    out_children[1].x++;

    out_children[2] = *first_child;
    out_children[2].y++;

    out_children[3] = *first_child;
    out_children[3].x++;
    out_children[3].y++;
}

extern bool futile_coord_serialize(futile_coord_s *coord, ssize_t n_out, char *out) {
    int n_required = snprintf(out, n_out, "%d/%d/%d", coord->z, coord->x, coord->y);
    return n_required <= n_out;
}

extern bool futile_coord_deserialize(char *coord_str, futile_coord_s *out) {
    int n = sscanf(coord_str, "%10d/%10d/%10d", &out->z, &out->x, &out->y);
    return n == 3;
}

extern void futile_coord_print(futile_coord_s *coord, FILE *out) {
    fprintf(out, "%d/%d/%d", coord->z, coord->x, coord->y);
}

extern void futile_coord_println(futile_coord_s *coord, FILE *out) {
    futile_coord_print(coord, out);
    fputc('\n', out);
}

extern int futile_coord_cmp(futile_coord_s *lhs, futile_coord_s *rhs) {
    if (lhs->z < rhs->z) return -1;
    if (lhs->z > rhs->z) return 1;
    if (lhs->x < rhs->x) return -1;
    if (lhs->x > rhs->x) return 1;
    if (lhs->y < rhs->y) return -1;
    if (lhs->y > rhs->y) return 1;
    return 0;
}

extern bool futile_coord_equal(futile_coord_s *lhs, futile_coord_s *rhs) {
    return futile_coord_cmp(lhs, rhs) == 0;
}

// This is how the data is encoded into a 64 bit integer:
// 1 bit unused | 29 bits column | 29 bits row | 5 bits zoom

static const unsigned int zoom_mask = 31;       // 2^5 - 1
static const unsigned int row_mask = 536870911; // 2^29 - 1
static const unsigned int col_mask = 536870911; // 2^29 - 1
static const unsigned int row_offset = 5;       // zoom bits
static const unsigned int col_offset = 5 + 29;  // zoom bits + row bits

// the high row mask is all 1 bits except for the highest row bit
static const uint64_t high_row_mask = 18446744065119617023ULL;
// all 1 bits except for all zoom bits
static const uint64_t all_but_zoom_mask = 18446744073709551584ULL;

extern uint64_t futile_coord_marshall_int(futile_coord_s *coord) {
    uint64_t x = coord->x;
    uint64_t y = coord->y;
    uint64_t z = coord->z;
    return z | (y << row_offset) | (x << col_offset);
}

extern void futile_coord_unmarshall_int(uint64_t val, futile_coord_s *out_coord) {
    out_coord->z = zoom_mask & val;
    out_coord->y = row_mask  & (val >> row_offset);
    out_coord->x = col_mask  & (val >> col_offset);
}

extern uint64_t futile_coord_int_zoom_up(uint64_t val) {
    // First we'll update the row/col values both simultaneously by
    // shifting all bits to the right in an attempt to divide both by
    // 2. This is *almost* correct; we just need to account for the
    // fact that the lowest bit of the column value can "leak" into the
    // high bit of the row, which we do by zero'ing out just that bit
    // via the high_row_mask.
    uint64_t coord_int_shifted = (val >> 1) & high_row_mask;

    int zoom = zoom_mask & val;
    // Given that the row/col bits are now set correctly, all that
    // remains is to update the zoom bits. This is done by applying a
    // mask to zero out all the zoom bits, and then or'ing the new
    // parent zoom bits into place
    uint64_t parent_coord_int = (coord_int_shifted & all_but_zoom_mask) | (zoom - 1);
    return parent_coord_int;
}

static double min(double a, double b) {
    return a < b ? a : b;
}

static double max(double a, double b) {
    return a > b ? a : b;
}

static double radians_to_degrees(double radians) {
    return radians * 180 / M_PI;
}

static double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180;
}

extern void futile_explode_bounds(futile_bounds_s *bounds, double *out_minx, double *out_miny, double *out_maxx, double *out_maxy) {
    *out_minx = bounds->minx;
    *out_miny = bounds->miny;
    *out_maxx = bounds->maxx;
    *out_maxy = bounds->maxy;
}

// http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
// TODO make output into point
extern void futile_coord_to_lnglat(futile_coord_s *coord, double *out_lng_deg, double *out_lat_deg) {
    double n = pow(2, coord->z);
    double lng_deg = coord->x / n * 360.0 - 180.0;
    double lat_rad = atan(sinh(M_PI * (1 - 2 * coord->y / n)));
    double lat_deg = radians_to_degrees(lat_rad);
    *out_lng_deg = lng_deg;
    *out_lat_deg = lat_deg;
}

// http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
// make input point
extern void futile_lnglat_to_coord(double lng_deg, double lat_deg, int zoom, futile_coord_s *out) {
    double lat_rad = degrees_to_radians(lat_deg);
    double n = pow(2.0, zoom);
    out->x = (lng_deg + 180.0) / 360.0 * n;
    out->y = (1.0 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2.0 * n;
    out->z = zoom;
}

extern void futile_coord_to_bounds(futile_coord_s *coord, futile_bounds_s *out) {
    double topleft_lng, topleft_lat, bottomright_lng, bottomright_lat;
    futile_coord_s coord_bottomright = {
        .x=coord->x + 1,
        .y=coord->y + 1,
        .z=coord->z
    };
    futile_coord_to_lnglat(coord, &topleft_lng, &topleft_lat);
    futile_coord_to_lnglat(&coord_bottomright, &bottomright_lng, &bottomright_lat);
    double minx = topleft_lng;
    double miny = bottomright_lat;
    double maxx = bottomright_lng;
    double maxy = topleft_lat;

    // coord_to_bounds is used to calculate boxes that could be off the grid
    // clamp the max values in that scenario
    maxx = min(180, maxx);
    maxy = min(90, maxy);

    *out = (futile_bounds_s){minx, miny, maxx, maxy};
}

extern int futile_bounds_to_coords(futile_bounds_s *bounds, int zoom, futile_coord_s out_coords[]) {
    double topleft_lng, topleft_lat, bottomright_lng, bottomright_lat;
    futile_explode_bounds(bounds, &topleft_lng, &bottomright_lat, &bottomright_lng, &topleft_lat);

    futile_coord_s topleft_coord, bottomright_coord;
    futile_lnglat_to_coord(topleft_lng, topleft_lat, zoom, &topleft_coord);
    futile_lnglat_to_coord(bottomright_lng, bottomright_lat, zoom, &bottomright_coord);

    // clamp max values
    int maxval = pow(2, zoom) - 1;
    bottomright_coord.x = min(maxval, bottomright_coord.x);
    bottomright_coord.y = min(maxval, bottomright_coord.y);

    out_coords[0] = topleft_coord;
    // check if one coordinate subsumes the whole bounds at this zoom
    if (topleft_coord.x == bottomright_coord.x &&
        topleft_coord.y == bottomright_coord.y) {
        return 1;
    } else {
        // we have two inclusive coordinates representing the range
        out_coords[1] = bottomright_coord;
        return 2;
    }
}

// radius of earth in meters is 6378137
// static double circumference_meters = 40075016.685578487813;
static double half_circumference_meters = 20037508.342789243907;

extern void futile_mercator_to_wgs84(futile_point_s *in, futile_point_s *out) {
    double x = in->x, y = in->y;

    x /= half_circumference_meters;
    y /= half_circumference_meters;

    y = (2 * atan(exp(y * M_PI)) - (M_PI / 2)) / M_PI;

    x *= 180;
    y *= 180;

    out->x = x;
    out->y = y;
}

extern void futile_wgs84_to_mercator(futile_point_s *in, futile_point_s *out) {
    double x = in->x, y = in->y;

    // Latitude
    y = log(tan(y * M_PI / 360 + M_PI / 4)) / M_PI;
    y *= half_circumference_meters;

    // Longitude
    x *= half_circumference_meters / 180;

    out->x = x;
    out->y = y;
}

// log(circumference_meters) / log(2)
double zoom_with_mercator_meters = 25.256199785270;

extern void futile_coord_to_mercator(futile_coord_s *in, futile_point_s *out) {
    // update the source x, y values to their corresponding values at
    // the zoom where mercator units are in meters
    double x = in->x * pow(2, zoom_with_mercator_meters - in->z);
    double y = in->y * pow(2, zoom_with_mercator_meters - in->z);

    // adjust for coordinate system
    out->x = x - half_circumference_meters;
    // y grid starts from 0 at top and goes down
    out->y = half_circumference_meters - y;
}

extern void futile_mercator_to_coord(futile_point_s *in, int zoom, futile_coord_s *out) {
    // adjust for coordinate system
    double x = in->x + half_circumference_meters;
    // y grid starts from 0 at top and goes down
    double y = half_circumference_meters - in->y;

    out->x = x * pow(2, zoom - zoom_with_mercator_meters);
    out->y = y * pow(2, zoom - zoom_with_mercator_meters);
    out->z = zoom;
}

extern void futile_coord_to_mercator_bounds(futile_coord_s *in, futile_bounds_s *out) {
    futile_coord_s coord_bottom_right = {.x=in->x + 1, .y=in->y + 1, in->z};
    futile_point_s merc_topleft, merc_bottomright;
    futile_coord_to_mercator(in, &merc_topleft);
    futile_coord_to_mercator(&coord_bottom_right, &merc_bottomright);
    out->minx = min(merc_topleft.x, merc_bottomright.x);
    out->miny = min(merc_topleft.y, merc_bottomright.y);
    out->maxx = max(merc_topleft.x, merc_bottomright.x);
    out->maxy = max(merc_topleft.y, merc_bottomright.y);
}

extern int futile_mercator_bounds_to_coords(futile_bounds_s *bounds, int zoom, futile_coord_s *out) {
    double topleft_x, topleft_y, bottomright_x, bottomright_y;
    futile_explode_bounds(bounds, &topleft_x, &bottomright_y, &bottomright_x, &topleft_y);
    futile_point_s topleft_point = {topleft_x, topleft_y};
    futile_point_s bottomright_point = {bottomright_x, bottomright_y};
    futile_coord_s topleft_coord, bottomright_coord;
    futile_mercator_to_coord(&topleft_point, zoom, &topleft_coord);
    futile_mercator_to_coord(&bottomright_point, zoom, &bottomright_coord);
    out[0] = topleft_coord;
    if (topleft_coord.x == bottomright_coord.x &&
        topleft_coord.y == bottomright_coord.y) {
        return 1;
    } else {
        out[1] = bottomright_coord;
        return 2;
    }
}

extern void futile_coord_to_quadkey(futile_coord_s *coord, char *quadkey) {
    int n = 0;
    int x = coord->x;
    int y = coord->y;
    for (int i = coord->z; i > 0; i--) {
        char digit = '0';
        int mask = 1 << (i - 1);
        if ((x & mask) != 0) {
            digit++;
        }
        if ((y & mask) != 0) {
            digit += 2;
        }
        quadkey[n++] = digit;
    }
    quadkey[n] = '\0';
}

extern bool futile_quadkey_to_coord(char *quadkey, size_t n_quadkey, futile_coord_s *coord) {
    int x = 0, y = 0;
    int z = n_quadkey;
    for (int i = z; i > 0; i--) {
        int mask = 1 << (i - 1);
        switch (quadkey[z - i]) {
        case '0':
            break;
        case '1':
            x |= mask;
            break;
        case '2':
            y |= mask;
            break;
        case '3':
            x |= mask;
            y |= mask;
            break;
        default:
            return false;
        }
    }
    coord->x = x;
    coord->y = y;
    coord->z = z;
    return true;
}

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
