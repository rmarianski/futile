#ifndef FUTILE_TILE_H
#define FUTILE_TILE_H

#include "coord.h"
#include "geo.h"

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
 * @file tile.h
 *
 * @brief Higher level tile functions
 *
 * This includes parent/children, and bounds/zooms tile generation.
 */

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

#endif
