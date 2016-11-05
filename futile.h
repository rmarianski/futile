#ifndef FUTILE_H
#define FUTILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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
void futile_coord_zoom(int delta, futile_coord_s *out);

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
bool futile_coord_parent(futile_coord_s *coord, futile_coord_s *out_coord);

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
void futile_coord_children(futile_coord_s *coord, futile_coord_s *out_children);

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
bool futile_coord_serialize(futile_coord_s *coord, ssize_t n_out, char *out);

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
bool futile_coord_deserialize(char *str, futile_coord_s *out_coord);

/**
 * @brief Print coord to file object
 *
 * futile_coord_print prints out the serialized coord into out.
 *
 * @param[in] coord Input coordinate
 * @param[out] out File handle to write to
 */
void futile_coord_print(futile_coord_s *coord, FILE *out);

/**
 * @brief Print coord to file object with newline
 *
 * futile_coord_println prints out the serialized coord into out with a newline.
 *
 * @param[in] coord Input coordinate
 * @param[out] out File handle to write to
 */
void futile_coord_println(futile_coord_s *coord, FILE *out);

/**
 * @brief Compare two coordinates
 *
 * futile_coord_cmp compares two coordinates. First the zooms are
 * compared, then the columns, then the rows.
 *
 * @param[in] lhs,rhs Coordinates to compare - z -> x -> y
 * @return -1 if lhs < rhs, +1 if lhs > rhs, and 0 if equal
 */
int futile_coord_cmp(futile_coord_s *lhs, futile_coord_s *rhs);

/**
 * @brief Test two coordinates for equality
 *
 * futile_coord_equal tests two coordinates to determine if they are
 * equal. This is equivalent to checking if they cmp 0.
 *
 * @param[in] lhs,rhs Coordinates to check for equality
 * @return true if coordinates are equal
 */
bool futile_coord_equal(futile_coord_s *lhs, futile_coord_s *rhs);

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
uint64_t futile_coord_marshall_int(futile_coord_s *coord);

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
void futile_coord_unmarshall_int(uint64_t val, futile_coord_s *out_coord);

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
uint64_t futile_coord_int_zoom_up(uint64_t val);

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
void futile_explode_bounds(futile_bounds_s *bounds, double *out_minx, double *out_miny, double *out_maxx, double *out_maxy);

/**
 * @brief Convert a coordinate to lng/lat
 *
 * futile_coord_to_lnglat takes an input coordinate, and converts that to
 * lng lat doubles, which are in degrees.
 *
 * @param[in] coord Input coordinate
 * @param[out] out_lng_deg Output longitude
 * @param[out] out_lat_deg Output latitude
 */
void futile_coord_to_lnglat(futile_coord_s *coord, double *out_lng_deg, double *out_lat_deg);

/**
 * @brief Convert a lng/lat to a coordinate
 *
 * futile_lnglat_to_coord takes an input longitude and latitude in degrees,
 * a zoom, and converts those to a coordinate.
 *
 * @param[in] lng_deg Input longitude
 * @param[in] lat_deg Input latitude
 * @param[in] zoom Input zoom
 * @param[out] out Output coordinate
 */
void futile_lnglat_to_coord(double lng_deg, double lat_deg, int zoom, futile_coord_s *out);

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
void futile_coord_to_bounds(futile_coord_s *coord, futile_bounds_s *out);

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
int futile_bounds_to_coords(futile_bounds_s *bounds, int zoom, futile_coord_s *out_coords);

/**
 * @brief Reproject a 3857 mercator point to 4326 lng/lat
 *
 * futile_mercator_to_wgs84 will reproject a 3857 mercator point to a
 * 4326 lng/lat point.
 *
 * @param[in] in Input point in 3857 mercator
 * @param[out] out Output point in 4326 lng/lat
 */
void futile_mercator_to_wgs84(futile_point_s *in, futile_point_s *out);

/**
 * @brief Reproject a point in 4326 lng/lat to 3857 mercator
 *
 * futile_wgs84_to_mercator will reproject a 4326 lng/lat point to a
 * 3857 mercator point.
 *
 * @param[in] in Input point in 4326 lng/lat
 * @param[out] out Output point in 3857 mercator
 */
void futile_wgs84_to_mercator(futile_point_s *in, futile_point_s *out);

/**
 * @brief Convert a coordinate to a 3857 mercator point
 *
 * futile_coord_to_mercator will take in a coordinate, and update a
 * point in 3857 mercator coordinates.
 *
 * @param[in] in Input coordinate
 * @param[out] out Output point in 3857 mercator
 */
void futile_coord_to_mercator(futile_coord_s *in, futile_point_s *out);

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
void futile_mercator_to_coord(futile_point_s *in, int zoom, futile_coord_s *out);

/**
 * @brief Convert a coordinate to bounds in mercator meters
 *
 * futile_coord_to_mercator_bounds will convert a coordinate to bounds
 * in mercator meters.
 *
 * @param[in] in Input coordinate
 * @param[out] out Output bounds in mercator meters
 */
void futile_coord_to_mercator_bounds(futile_coord_s *in, futile_bounds_s *out);

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
int futile_mercator_bounds_to_coords(futile_bounds_s *in, int zoom, futile_coord_s *out);

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
void futile_coord_to_quadkey(futile_coord_s *coord, char *out_quadkey);

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
bool futile_quadkey_to_coord(char *quadkey, size_t level_of_detail, futile_coord_s *out_coord);

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
void futile_for_zoom_range(unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata);

bool futile_for_zoom_range_array(futile_coord_cursor_s *cursor, futile_coord_group_s *group);

void futile_for_coord_zoom_range(unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y, unsigned int start_zoom, unsigned int end_zoom, futile_coord_fn for_coord, void *userdata);

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
void futile_for_coord_parents(futile_coord_s *start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata);

/**
 * @brief Calculate number of cumulative tiles in zoom level
 *
 * futile_n_for_zoom will return the total number of tiles in a
 * particular zoom level, and all those below it.
 *
 * @param[in] zoom Zoom level
 * @return[out] Total number of cumulative tiles for a given zoom level
*/
long futile_n_for_zoom(unsigned int zoom);

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
void futile_for_bounds(futile_bounds_s *bounds, unsigned int zoom_start, unsigned int zoom_until, futile_coord_fn for_coord, void *userdata);

bool futile_coord_is_valid(futile_coord_s *coord);

#endif
