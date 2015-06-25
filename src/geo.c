#include <math.h>
#include "coord.h"
#include "geo.h"

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
