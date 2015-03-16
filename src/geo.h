#ifndef FUTILE_GEO_H
#define FUTILE_GEO_H

#include "coord.h"

/**
 *  @file geo.h
 *
 *  @brief Geographic operations
 *
 * Geographic operations on coordinates, bounding boxes and points.
 */

/**
 * @brief Bounds representation: minx, miny, maxx, maxy
 */
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
 * futile_num_to_deg takes an input coordinate, and converts that to
 * lng lat doubles, which are in degrees.
 *
 * @param[in] coord Input coordinate
 * @param[out] out_lng_deg Output longitude
 * @param[out] out_lat_deg Output latitude
 */
void futile_num_to_deg(futile_coord_s *coord, double *out_lng_deg, double *out_lat_deg);

/**
 * @brief Convert a lng/lat to a coordinate
 *
 * futile_deg_to_num takes an input longitude and latitude in degrees,
 * a zoom, and converts those to a coordinate.
 *
 * @param[in] lng_deg Input longitude
 * @param[in] lat_deg Input latitude
 * @param[in] zoom Input zoom
 * @param[out] out Output coordinate
 */
void futile_deg_to_num(double lng_deg, double lat_deg, int zoom, futile_coord_s *out);

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

#endif
