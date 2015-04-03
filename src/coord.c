#include <math.h>
#include <stdio.h>
#include "coord.h"

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
    int n = sscanf(coord_str, "%d/%d/%d", &out->z, &out->x, &out->y);
    return n == 3;
}

extern void futile_coord_print(futile_coord_s *coord, FILE *out) {
    fprintf(out, "%d/%d/%d", coord->z, coord->x, coord->y);
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
