#ifndef FUTILE_H
#define FUTILE_H

#ifdef FUTILE_STATIC
#define FUTILE_DEF static
#else
#define FUTILE_DEF extern
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @mainpage Futile
 *
 * Futile is a library for tile related utility functions. It assumes
 * that an x/y/z gridset is used, as in
 * openstreetmap. http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
 *
 * coord.h contains the core data structure, and basic manipulation of
 * coordinates.
 *
 * geo.h contains the bounds and point data structures, and geographic
 * operations.
 *
 * tile.h contains some higher level tile functions, like tile
 * generation.
 */

/**
 * @brief Tile coordinate representation
 *
 * futile_coord_s is the coordinate structure. It represents a tile in
 * x, y, z coordinates.
 *
 * A union is used to allow access either via x,y,z, or
 * column,row,zoom members.
 */
typedef struct futile_coord_s {
    /** @brief coordinate x or column value */
    uint32_t x;
    /** @brief coordinate y or row value */
    uint32_t y;
    /** @brief coordinate z or zoom value */
    uint32_t z;
} futile_coord_s;

/**
 * @brief Zoom a coordinate, updating column/row appropriately
 *
 * futile_coord_zoom zooms a coordinate down or up depending on the
 * delta. Positive deltas zoom in to higher zoom levels. This will
 * update the column and row values of the coordinate appropriately.
 *
 * @param[in] delta The amount to zoom, positive zooms in, negative
 * zooms out.
 * @param[out] out The coordinate to zoom.
 */
FUTILE_DEF void futile_coord_zoom(int delta, futile_coord_s *out);

/**
 * @brief Generate a coordinate's parent
 *
 * futile_coord_parent will generate a parent coordinate given an
 * input coordinate. If the input coordinate is not positive, nothing
 * is modified, and false is returned. The same address can be passed
 * for both parameters, in which case the operation is done in place.
 *
 * @param[in] coord Input coordindate
 * @param[out] out_coord Output parent coordinate
 * @return true if input coordinate zoom is positive
 */
FUTILE_DEF bool futile_coord_parent(futile_coord_s *coord, futile_coord_s *out_coord);

/**
 * @brief Generate a coordinate's children
 *
 * futile_coord_children will generate the 4 children for a given
 * coordinate. out_children should point enough space to accommodate 4
 * coordinates.
 *
 * @param[in] coord Input coordinate
 * @param[out] out_children Output children coordinates
 */
FUTILE_DEF void futile_coord_children(futile_coord_s *coord, futile_coord_s *out_children);

/**
 * @brief Serialize coordinate to string
 *
 * futile_coord_serialize serializes a coordinate into a string that
 * looks like "<zoom>/<column>/<row>". Returns false if the
 * serialized version will not fit into the provided string buffer.
 *
 * @param[in] coord Input coordinate.
 * @param[in] n_out Size of memory that out points to, which should include \0.
 * @param[out] out The memory to serialize the string to.
 * @return false if memory could not hold serialized string.
 */
FUTILE_DEF bool futile_coord_serialize(futile_coord_s *coord, ssize_t n_out, char *out);

/**
 * @brief Deserializes a string into a coordinate structure
 *
 * futile_coord_deserialize takes a string that looks like
 * "<zoom>/<column>/<row>" and fills the coordinate with it. Returns
 * false if the string could not be parsed.
 *
 * @param[in] str Input string to parse
 * @param[out] out_coord Coordinate to update
 * @return false if str could no be parsed
 */
FUTILE_DEF bool futile_coord_deserialize(char *str, futile_coord_s *out_coord);

/**
 * @brief Print coord to file object
 *
 * futile_coord_print prints out the serialized coord into out.
 *
 * @param[in] coord Input coordinate
 * @param[out] out File handle to write to
 */
FUTILE_DEF void futile_coord_print(futile_coord_s *coord, FILE *out);

/**
 * @brief Print coord to file object with newline
 *
 * futile_coord_println prints out the serialized coord into out with a newline.
 *
 * @param[in] coord Input coordinate
 * @param[out] out File handle to write to
 */
FUTILE_DEF void futile_coord_println(futile_coord_s *coord, FILE *out);

/**
 * @brief Compare two coordinates
 *
 * futile_coord_cmp compares two coordinates. First the zooms are
 * compared, then the columns, then the rows.
 *
 * @param[in] lhs,rhs Coordinates to compare - z -> x -> y
 * @return -1 if lhs < rhs, +1 if lhs > rhs, and 0 if equal
 */
FUTILE_DEF int futile_coord_cmp(futile_coord_s *lhs, futile_coord_s *rhs);

/**
 * @brief Test two coordinates for equality
 *
 * futile_coord_equal tests two coordinates to determine if they are
 * equal. This is equivalent to checking if they cmp 0.
 *
 * @param[in] lhs,rhs Coordinates to check for equality
 * @return true if coordinates are equal
 */
FUTILE_DEF bool futile_coord_equal(futile_coord_s *lhs, futile_coord_s *rhs);

/**
 * @brief Marshall a coordinate into a 64 bit integer
 *
 * futile_coord_marshall_int will convert a coordinate into a 64 bit
 * integer. Only coordinates up to zoom level 29 are supported. Higher zoom levels can work, provided that they have "low" column or row values. The highest value is 2^29 -1.
 *
 * This is how the data is encoded into a 64 bit integer:
 * 1 bit unused | 29 bits column | 29 bits row | 5 bits zoom
 *
 * @param[in] coord Input coordinate
 * @return 64 bit integer representation of coordinate
 */
FUTILE_DEF uint64_t futile_coord_marshall_int(futile_coord_s *coord);

/**
 * @brief Unmarshall a 64 bit integer into a coordinate
 *
 * futile_coord_unmarshall_int will convert a 64 bit integer into a
 * coordinate. The 64 bit integer is expected to be formatted as the
 * futile_coord_marshall_int function would.
 *
 * @param[in] val Input integer
 * @param[out] out_coord Output coordinate
 */
FUTILE_DEF void futile_coord_unmarshall_int(uint64_t val, futile_coord_s *out_coord);

/**
 * @brief Zoom a 64 bit integer, representing a coordinate, up
 *
 * futile_coord_int_zoom_up will take a 64 bit integer, assumed to be
 * a marshalled coordinate, and will return a 64 bit integer
 * representing its parent.
 *
 * @param[in] val Input integer
 * @return 64 bit integer representing the input coordinate's parent
*/
FUTILE_DEF uint64_t futile_coord_int_zoom_up(uint64_t val);

typedef struct futile_bounds_s {
    /** @brief minimum x value */
    double minx;
    /** @brief minimum y value */
    double miny;
    /** @brief maximum x value */
    double maxx;
    /** @brief maximum y value */
    double maxy;
} futile_bounds_s;

/**
 * @brief Point representation
 */
typedef struct futile_point_s {
    /** @brief x value */
    double x;
    /** @brief y value */
    double y;
} futile_point_s;

/**
 * @brief Update 4 doubles from a bounds representation
 *
 * futile_explode_bounds is a convenience function to explode a single
 * bounds into 4 doubles.
 *
 * @param[in] bounds Input bounds
 * @param[out] out_minx,out_miny,out_maxx,out_maxy Doubles to update from input bounds
 */
FUTILE_DEF void futile_explode_bounds(futile_bounds_s *bounds, double *out_minx, double *out_miny, double *out_maxx, double *out_maxy);

/**
 * @brief Convert a coordinate to lng/lat
 *
 * futile_coord_to_lnglat takes an input coordinate, and converts that to
 * lng lat doubles, which are in degrees.
 *
 * @param[in] coord Input coordinate
 * @param[out] out Output longitude latitude point
 */
FUTILE_DEF void futile_coord_to_lnglat(futile_coord_s *coord, futile_point_s *out);

/**
 * @brief Convert a lng/lat to a coordinate
 *
 * futile_lnglat_to_coord takes an input longitude and latitude in degrees,
 * a zoom, and converts those to a coordinate.
 *
 * @param[in] lnglat Input longitude latitude
 * @param[in] zoom Input zoom
 * @param[out] out Output coordinate
 */
FUTILE_DEF void futile_lnglat_to_coord(futile_point_s *lnglat, int zoom, futile_coord_s *out);

/**
 * @brief Generate a bounding box in 4326 to encompass the coordinate
 *
 * futile_coord_to_bounds takes an input coordinate, and generates a
 * surrounding bounding box. The bounding box will be in 4326 lng/lat
 * coordinates.
 *
 * @param[in] coord Input coordinate
 * @param[out] out Output bounds
 */
FUTILE_DEF void futile_coord_to_bounds(futile_coord_s *coord, futile_bounds_s *out);

/**
 * @brief Generate coordinate(s) for bounds in 4326 lng/lat and zoom
 *
 * futile_bounds_to_coords will generate the necessary coordinates
 * needed to encompass the given bounds, in 4326 lng/lat, at the
 * specified zoom level. If a single coordinate suffices, only one
 * will be updated. If more than one coordinate is required to cover
 * the given bounds at the specified zoom level, 2 coordinates will be
 * updated. The 2 coords will be the topleft, and bottom right
 * respectively. The number of coordinates that were updated is
 * returned.
 *
 * Note that the memory pointed to by out_coords should have enough
 * space to update 2 coordinates.
 *
 * @param[in] bounds Input bounds
 * @param[in] zoom Zoom level
 * @param[out] out_coords Output coordinate(s) (space for 2 coords)
 * @return Number of coordinates updated
 */
FUTILE_DEF unsigned int futile_bounds_to_coords(futile_bounds_s *bounds, int zoom, futile_coord_s *out_coords);

/**
 * @brief Reproject a 3857 mercator point to 4326 lng/lat
 *
 * futile_mercator_to_lnglat will reproject a 3857 mercator point to a
 * 4326 lng/lat point.
 *
 * @param[in] in Input point in 3857 mercator
 * @param[out] out Output point in 4326 lng/lat
 */
FUTILE_DEF void futile_mercator_to_lnglat(futile_point_s *in, futile_point_s *out);

/**
 * @brief Reproject a point in 4326 lng/lat to 3857 mercator
 *
 * futile_lnglat_to_mercator will reproject a 4326 lng/lat point to a
 * 3857 mercator point.
 *
 * @param[in] in Input point in 4326 lng/lat
 * @param[out] out Output point in 3857 mercator
 */
FUTILE_DEF void futile_lnglat_to_mercator(futile_point_s *in, futile_point_s *out);

/**
 * @brief Convert a coordinate to a 3857 mercator point
 *
 * futile_coord_to_mercator will take in a coordinate, and update a
 * point in 3857 mercator coordinates.
 *
 * @param[in] in Input coordinate
 * @param[out] out Output point in 3857 mercator
 */
FUTILE_DEF void futile_coord_to_mercator(futile_coord_s *in, futile_point_s *out);

/**
 * @brief Convert a point in 3857 mercator and zoom to a coordinate
 *
 * futile_mercator_to_coord will convert a 3857 mercator point and
 * zoom level to a coordinate.
 *
 * @param[in] in Input point in 3857 mercator
 * @param[in] zoom Input zoom level
 * @param[out] out Output coordinate
 */
FUTILE_DEF void futile_mercator_to_coord(futile_point_s *in, int zoom, futile_coord_s *out);

/**
 * @brief Convert a coordinate to bounds in mercator meters
 *
 * futile_coord_to_mercator_bounds will convert a coordinate to bounds
 * in mercator meters.
 *
 * @param[in] in Input coordinate
 * @param[out] out Output bounds in mercator meters
 */
FUTILE_DEF void futile_coord_to_mercator_bounds(futile_coord_s *in, futile_bounds_s *out);

/**
 * @brief Convert bounds in mercator meters to a coordinate
 *
 * futile_mercator_bounds_to_coords will convert bounds in mercator
 * meters to one or more coordinates. If a single coordinate suffices,
 * only one will be updated. If more than one coordinate is required
 * to cover the given bounds at the specified zoom level, 2
 * coordinates will be updated. The 2 coords will be the topleft, and
 * bottom right respectively. The number of coordinates that were
 * updated is returned.
 *
 * Note that the memory pointed to by out should have enough
 * space to update 2 coordinates.
 *
 * @param[in] in Input bounds in mercator meters
 * @param[in] zoom Zoom level
 * @param[out] out Output coordinate(s) (space for 2 coords)
 * @return Number of coordinates updated
 */
FUTILE_DEF int futile_mercator_bounds_to_coords(futile_bounds_s *in, int zoom, futile_coord_s *out);

/**
 * @brief Convert a coord to its quadkey representation
 *
 * futile_coord_to_quadkey takes an input coordinate, and generates
 * its quad key representation. It's critical that quadkey contains
 * enough space to hold zoom + 1 characters (zoom chars + \0)
 *
 * @param[in] coord Input coordinate
 * @param[out] out_quadkey Output quad key, must accommodate zoom + 1 characters
 */
FUTILE_DEF void futile_coord_to_quadkey(futile_coord_s *coord, char *out_quadkey);

/**
 * @brief Convert a quadkey to a coordinate
 *
 * futile_quadkey_to_coord will convert a quad key to a coordinate. It
 * returns false if a non quadkey char is detected. It assumes that
 * quadkey points to at least level_of_detail characters.
 *
 * @param[in] quadkey Input quad key
 * @param[in] level_of_detail Level of detail for quad key, ie strlen(quadkey)+1
 * @param[out] out_coord Output coordinate
 * @return false if an invalid quadkey string is passed in
 */
FUTILE_DEF bool futile_quadkey_to_coord(char *quadkey, size_t level_of_detail, futile_coord_s *out_coord);

/**
 * @brief Coordinate callback
 *
 * For functions that generate multiple coordinates, this callback is
 * used to perform per coordinate operations. All functions that use
 * this callback type take in a userdata "baton", which gets passed
 * through to the callback.
 */
typedef void (*futile_coord_fn)(futile_coord_s *coord, void *userdata);

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int zoom_until;
} futile_coord_cursor_s;

typedef struct {
    size_t n;
    futile_coord_s *coords;
} futile_coord_group_s;

/**
 * @brief Visit coordinates in a given range
 *
 * futile_for_zoom_range will call the given coord callback function
 * for all coordinates between zoom start and until.
 *
 * @param[in] zoom_start Input start zoom
 * @param[in] zoom_until Input end zoom (inclusive)
 * @param[in] for_coord Callback function for each coordinate
 * @param[in] userdata Baton passed into callback function
 */
FUTILE_DEF void futile_for_zoom_range(unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata);

FUTILE_DEF bool futile_for_zoom_range_array(futile_coord_cursor_s *cursor, futile_coord_group_s *group);

FUTILE_DEF void futile_for_coord_zoom_range(unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y, unsigned int start_zoom, unsigned int end_zoom, futile_coord_fn for_coord, void *userdata);

/**
 * @brief Visit coordinate parents
 *
 * futile_for_coord_parents will call the given coord callback
 * function for all parent coordinates.
 *
 * @param[in] start Start coordinate
 * @param[in] zoom_until Ending zoom (inclusive)
 * @param[in] for_coord Callback function for each coordinate
 * @param[in] userdata Baton passed into callback function
 */
FUTILE_DEF void futile_for_coord_parents(futile_coord_s *start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata);

/**
 * @brief Calculate number of cumulative tiles in zoom level
 *
 * futile_n_for_zoom will return the total number of tiles in a
 * particular zoom level, and all those below it.
 *
 * @param[in] zoom Zoom level
 * @return[out] Total number of cumulative tiles for a given zoom level
*/
FUTILE_DEF long futile_n_for_zoom(unsigned int zoom);

/**
 * @brief Visit coordinates within bounds for a zoom level range
 *
 * futile_for_bounds will call the given coord callback function for
 * all coordinates within the bounds at the zoom level range.
 *
 * @param[in] bounds Input bounds
 * @param[in] zoom_start Starting zoom level
 * @param[in] zoom_until Ending zoom level, inclusive
 * @param[in] for_coord Callback function for each coordinate
 * @param[in] userdata Baton passed into callback function
 */
FUTILE_DEF void futile_for_bounds(futile_bounds_s *bounds, unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata);

FUTILE_DEF bool futile_coord_is_valid(futile_coord_s *coord);

#ifdef __cplusplus
}
#endif

#ifdef FUTILE_IMPLEMENTATION

#include <math.h>

FUTILE_DEF void futile_coord_zoom(int delta, futile_coord_s *out) {
    out->x *= pow(2, delta);
    out->y *= pow(2, delta);
    out->z += delta;
}

FUTILE_DEF bool futile_coord_parent(futile_coord_s *coord, futile_coord_s *out_coord) {
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

FUTILE_DEF void futile_coord_children(futile_coord_s *coord, futile_coord_s *out_children) {
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

FUTILE_DEF bool futile_coord_is_valid(futile_coord_s *coord) {
    bool result = false;
    int max_row_col = pow(2, coord->z);
    if (coord->x < max_row_col && coord->y < max_row_col) {
        result = true;
    }
    return result;
}

FUTILE_DEF bool futile_coord_serialize(futile_coord_s *coord, ssize_t n_out, char *out) {
    int n_required = snprintf(out, n_out, "%d/%d/%d", coord->z, coord->x, coord->y);
    return n_required <= n_out;
}

FUTILE_DEF bool futile_coord_deserialize(char *coord_str, futile_coord_s *out) {
    bool result = false;
    int x, y, z;
    if (sscanf(coord_str, "%10d/%10d/%10d", &z, &x, &y) == 3) {
        if (z >= 0 && x >= 0 && y >= 0) {
            out->z = z;
            out->x = x;
            out->y = y;
            result = futile_coord_is_valid(out);
        }
    }
    return result;
}

FUTILE_DEF void futile_coord_print(futile_coord_s *coord, FILE *out) {
    fprintf(out, "%d/%d/%d", coord->z, coord->x, coord->y);
}

FUTILE_DEF void futile_coord_println(futile_coord_s *coord, FILE *out) {
    futile_coord_print(coord, out);
    fputc('\n', out);
}

FUTILE_DEF int futile_coord_cmp(futile_coord_s *lhs, futile_coord_s *rhs) {
    if (lhs->z < rhs->z) return -1;
    if (lhs->z > rhs->z) return 1;
    if (lhs->x < rhs->x) return -1;
    if (lhs->x > rhs->x) return 1;
    if (lhs->y < rhs->y) return -1;
    if (lhs->y > rhs->y) return 1;
    return 0;
}

FUTILE_DEF bool futile_coord_equal(futile_coord_s *lhs, futile_coord_s *rhs) {
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

FUTILE_DEF uint64_t futile_coord_marshall_int(futile_coord_s *coord) {
    uint64_t x = coord->x;
    uint64_t y = coord->y;
    uint64_t z = coord->z;
    return z | (y << row_offset) | (x << col_offset);
}

FUTILE_DEF void futile_coord_unmarshall_int(uint64_t val, futile_coord_s *out_coord) {
    out_coord->z = zoom_mask & val;
    out_coord->y = row_mask  & (val >> row_offset);
    out_coord->x = col_mask  & (val >> col_offset);
}

FUTILE_DEF uint64_t futile_coord_int_zoom_up(uint64_t val) {
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

FUTILE_DEF void futile_explode_bounds(futile_bounds_s *bounds, double *out_minx, double *out_miny, double *out_maxx, double *out_maxy) {
    *out_minx = bounds->minx;
    *out_miny = bounds->miny;
    *out_maxx = bounds->maxx;
    *out_maxy = bounds->maxy;
}

// http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
FUTILE_DEF void futile_coord_to_lnglat(futile_coord_s *coord, futile_point_s *out) {
    double n = pow(2, coord->z);
    double lng_deg = coord->x / n * 360.0 - 180.0;
    double lat_rad = atan(sinh(M_PI * (1 - 2 * coord->y / n)));
    double lat_deg = radians_to_degrees(lat_rad);
    out->x = lng_deg;
    out->y = lat_deg;
}

// http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
// make input point
FUTILE_DEF void futile_lnglat_to_coord(futile_point_s *lnglat, int zoom, futile_coord_s *out) {
    double lng_deg = lnglat->x;
    double lat_deg = lnglat->y;

    double lat_rad = degrees_to_radians(lat_deg);
    double n = pow(2.0, zoom);
    out->x = (lng_deg + 180.0) / 360.0 * n;
    out->y = (1.0 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2.0 * n;
    out->z = zoom;
}

FUTILE_DEF void futile_coord_to_bounds(futile_coord_s *coord, futile_bounds_s *out) {
    futile_point_s topleft, bottomright;
    futile_coord_s coord_bottomright = {
        .x=coord->x + 1,
        .y=coord->y + 1,
        .z=coord->z
    };
    futile_coord_to_lnglat(coord, &topleft);
    futile_coord_to_lnglat(&coord_bottomright, &bottomright);
    double minx = topleft.x;
    double miny = bottomright.y;
    double maxx = bottomright.x;
    double maxy = topleft.y;

    // coord_to_bounds is used to calculate boxes that could be off the grid
    // clamp the max values in that scenario
    maxx = min(180, maxx);
    maxy = min(90, maxy);

    *out = (futile_bounds_s){minx, miny, maxx, maxy};
}

FUTILE_DEF unsigned int futile_bounds_to_coords(futile_bounds_s *bounds, int zoom, futile_coord_s out_coords[]) {
    futile_point_s topleft, bottomright;
    futile_explode_bounds(bounds, &topleft.x, &bottomright.y, &bottomright.x, &topleft.y);

    futile_coord_s topleft_coord, bottomright_coord;
    futile_lnglat_to_coord(&topleft, zoom, &topleft_coord);
    futile_lnglat_to_coord(&bottomright, zoom, &bottomright_coord);

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

FUTILE_DEF void futile_mercator_to_lnglat(futile_point_s *in, futile_point_s *out) {
    double x = in->x, y = in->y;

    x /= half_circumference_meters;
    y /= half_circumference_meters;

    y = (2 * atan(exp(y * M_PI)) - (M_PI / 2)) / M_PI;

    x *= 180;
    y *= 180;

    out->x = x;
    out->y = y;
}

FUTILE_DEF void futile_lnglat_to_mercator(futile_point_s *in, futile_point_s *out) {
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
static double zoom_with_mercator_meters = 25.256199785270;

FUTILE_DEF void futile_coord_to_mercator(futile_coord_s *in, futile_point_s *out) {
    // update the source x, y values to their corresponding values at
    // the zoom where mercator units are in meters
    double x = in->x * pow(2, zoom_with_mercator_meters - in->z);
    double y = in->y * pow(2, zoom_with_mercator_meters - in->z);

    // adjust for coordinate system
    out->x = x - half_circumference_meters;
    // y grid starts from 0 at top and goes down
    out->y = half_circumference_meters - y;
}

FUTILE_DEF void futile_mercator_to_coord(futile_point_s *in, int zoom, futile_coord_s *out) {
    // adjust for coordinate system
    double x = in->x + half_circumference_meters;
    // y grid starts from 0 at top and goes down
    double y = half_circumference_meters - in->y;

    out->x = x * pow(2, zoom - zoom_with_mercator_meters);
    out->y = y * pow(2, zoom - zoom_with_mercator_meters);
    out->z = zoom;
}

FUTILE_DEF void futile_coord_to_mercator_bounds(futile_coord_s *in, futile_bounds_s *out) {
    futile_coord_s coord_bottom_right = {.x=in->x + 1, .y=in->y + 1, in->z};
    futile_point_s merc_topleft, merc_bottomright;
    futile_coord_to_mercator(in, &merc_topleft);
    futile_coord_to_mercator(&coord_bottom_right, &merc_bottomright);
    out->minx = min(merc_topleft.x, merc_bottomright.x);
    out->miny = min(merc_topleft.y, merc_bottomright.y);
    out->maxx = max(merc_topleft.x, merc_bottomright.x);
    out->maxy = max(merc_topleft.y, merc_bottomright.y);
}

FUTILE_DEF int futile_mercator_bounds_to_coords(futile_bounds_s *bounds, int zoom, futile_coord_s *out) {
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

FUTILE_DEF void futile_coord_to_quadkey(futile_coord_s *coord, char *quadkey) {
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

FUTILE_DEF bool futile_quadkey_to_coord(char *quadkey, size_t n_quadkey, futile_coord_s *coord) {
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

FUTILE_DEF void futile_for_zoom_range(unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata) {
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

FUTILE_DEF bool futile_for_zoom_range_array(futile_coord_cursor_s *cursor, futile_coord_group_s *group) {
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

FUTILE_DEF void futile_for_coord_zoom_range(unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y, unsigned int start_zoom, unsigned int end_zoom, futile_coord_fn for_coord, void *userdata) {
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


FUTILE_DEF void futile_for_coord_parents(futile_coord_s *start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata) {
    futile_coord_s coord = *start;
    while (coord.z >= zoom_until) {
        for_coord(&coord, userdata);
        futile_coord_zoom(-1, &coord);
    }
}

FUTILE_DEF long futile_n_for_zoom(unsigned int zoom) {
    // geometric series, each zoom containing 4 times more tiles
    return (1 - pow(4, zoom + 1)) / -3;
}

FUTILE_DEF void futile_for_bounds(futile_bounds_s *bounds, unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata) {
    unsigned int start_x, until_x, start_y, until_y, n_coords;
    futile_coord_s coords[2];

    for (unsigned int z = zoom_start; z <= zoom_until; z++) {
        n_coords = futile_bounds_to_coords(bounds, z, coords);;
        if (n_coords == 2) {
            start_x = coords[0].x;
            start_y = coords[0].y;
            until_x = coords[1].x;
            until_y = coords[1].y;
        } else {
            start_x = until_x = coords[0].x;
            start_y = until_y = coords[0].y;
        }

        coords->z = z;
        for (unsigned int y = start_y; y <= until_y; y++) {
            coords->y = y;
            for (unsigned int x = start_x; x <= until_x; x++) {
                coords->x = x;
                for_coord(coords, userdata);
            }
        }
    }
}

#endif

#endif
