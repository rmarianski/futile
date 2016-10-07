#include <glib.h>
#include <stdlib.h>
#include "tile.h"

void _for_zoom_range(__attribute__((unused)) futile_coord_s *c, void *userdata) {
    int *n = userdata;
    *n = *n + 1;
}

void test_tile_for_zoom_range() {
    int n = 0;
    futile_for_zoom_range(0, 2, _for_zoom_range, &n);
    g_assert(n == 21);
}

void print_coords(futile_coord_s *coords, size_t n) {
    for (size_t i = 0; i < n; i++) {
        futile_coord_println(coords + i, stderr);
    }
}

void test_tile_for_zoom_range_array() {
    futile_coord_cursor_s cursor = {};
    cursor.zoom_until = 2;
    futile_coord_s coords[255];
    futile_coord_group_s group = {.coords=coords, .n=255};
    g_assert(futile_for_zoom_range_array(&cursor, &group));
    g_assert(group.n == 21);
    fprintf(stderr, "\n");
    print_coords(group.coords, group.n);
}

void zero_coords(futile_coord_s *coords, size_t n) {
    for (size_t i = 0; i < n; i++) {
        coords[i] = (futile_coord_s){};
    }
}

void test_tile_for_zoom_range_array_multiple() {
    futile_coord_cursor_s cursor = {};
    cursor.zoom_until = 2;
    futile_coord_s coords[10];
    size_t n = sizeof(coords)/sizeof(coords[0]);
    futile_coord_group_s group = {.coords=coords, .n=n};

    zero_coords(coords, n);
    g_assert(!futile_for_zoom_range_array(&cursor, &group));
    g_assert(group.n == 10);
    fprintf(stderr, "\nFirst\n");
    print_coords(group.coords, group.n);

    zero_coords(coords, n);
    g_assert(!futile_for_zoom_range_array(&cursor, &group));
    g_assert(group.n == 10);
    fprintf(stderr, "Second\n");
    print_coords(group.coords, group.n);

    g_assert(futile_for_zoom_range_array(&cursor, &group));
    g_assert_cmpint(1, ==, group.n);
    fprintf(stderr, "Third\n");
    print_coords(group.coords, group.n);
}

typedef struct {
    int count;
} userdata_test_tile_for_coord_zoom_range;

void for_coord_test_tile_for_coord_zoom_range(futile_coord_s *coord, void *userdata_) {
    userdata_test_tile_for_coord_zoom_range *userdata = (userdata_test_tile_for_coord_zoom_range *)userdata_;
    userdata->count++;
}

void test_tile_for_coord_zoom_range() {
    userdata_test_tile_for_coord_zoom_range userdata = {};
    unsigned int
        start_x = 1, start_y = 1,
        end_x = 2, end_y = 2,
        start_zoom = 1, end_zoom = 2;
    futile_for_coord_zoom_range(start_x, start_y, end_x, end_y, start_zoom, end_zoom, for_coord_test_tile_for_coord_zoom_range, &userdata);
    g_assert(userdata.count == 20);
}

static futile_coord_s parent_coords[] = {
    {.z = 3, .x = 4, .y = 4},
    {.z = 2, .x = 2, .y = 2},
    {.z = 1, .x = 1, .y = 1}
};

void _for_coord_parents(futile_coord_s *coord, void *userdata) {
    int *n = userdata;
    futile_coord_s *exp = &parent_coords[*n];
    g_assert(exp->z == coord->z);
    g_assert(exp->x == coord->x);
    g_assert(exp->y == coord->y);
    *n = *n + 1;
}

void test_tile_parents() {
    futile_coord_s coord = {.z = 3, .x = 4, .y = 4};
    int n = 0;
    futile_for_coord_parents(&coord, 1, _for_coord_parents, &n);
    g_assert(3 == n);
}

void test_tile_n_for_zoom() {
    long n_zoom_7 = futile_n_for_zoom(7);
    g_assert(21845 == n_zoom_7);
}

struct _tile_bounds_userdata {
    int n;
};

void _for_tile_bounds_test(__attribute__((unused)) futile_coord_s *coord, void *userdata) {
    struct _tile_bounds_userdata *data = userdata;
    data->n += 1;
}

void test_tile_for_bounds() {
    double minx = -74.009399414062;
    double miny = 40.705627938206;
    double maxx = -74.003906250000;
    double maxy = 40.709792012435;
    futile_bounds_s bounds = {minx, miny, maxx, maxy};
    struct _tile_bounds_userdata userdata = {};
    futile_for_bounds(&bounds, 16, 17, _for_tile_bounds_test, &userdata);
    g_assert_cmpint(13, ==, userdata.n);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/tile/zoom-range", test_tile_for_zoom_range);
    g_test_add_func("/tile/zoom-range-array", test_tile_for_zoom_range_array);
    g_test_add_func("/tile/zoom-range-array-multiple", test_tile_for_zoom_range_array_multiple);
    g_test_add_func("/tile/coord-zoom-range", test_tile_for_coord_zoom_range);
    g_test_add_func("/tile/coord-parents", test_tile_parents);
    g_test_add_func("/tile/n-for-zoom", test_tile_n_for_zoom);
    g_test_add_func("/tile/for-tile/bounds", test_tile_for_bounds);

    return g_test_run();
}
