#ifndef FUTILE_COORD_H
#define FUTILE_COORD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 *  @file coord.h
 *
 *  @brief Coordinate specific functionality
 *
 * This includes serializing and marshalling operations, comparing two
 * coordinates, and basic zooming.
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

#endif
