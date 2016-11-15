#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#define FUTILE_IMPLEMENTATION 1
#define FUTILE_STATIC 1
#include "futile.h"

void test_coord_zoom_down_one() {
    futile_coord_s c = {.x=1, .y=1, .z=1};
    futile_coord_zoom(1, &c);
    g_assert(c.z == 2);
    g_assert(c.x == 2);
    g_assert(c.y == 2);
}

void test_coord_zoom_down_two() {
    futile_coord_s c = {.x=1, .y=0, .z=1};
    futile_coord_zoom(2, &c);
    g_assert(c.z == 3);
    g_assert(c.x == 4);
    g_assert(c.y == 0);
}

void test_coord_zoom_up() {
    futile_coord_s c = {.x=5, .y=7, .z=4};
    futile_coord_zoom(-2, &c);
    g_assert(c.z == 2);
    g_assert(c.x == 1);
    g_assert(c.y == 1);
}

void test_coord_parent_invalid() {
    futile_coord_s invalid_coord = {.x=1, .y=1, .z=0};
    futile_coord_s unused;
    g_assert(!futile_coord_parent(&invalid_coord, &unused));
}

void test_coord_parent_valid() {
    futile_coord_s coord = {.x=3, .y=1, .z=2};
    futile_coord_s parent;
    g_assert(futile_coord_parent(&coord, &parent));
    g_assert_cmpint(1, ==, parent.z);
    g_assert_cmpint(1, ==, parent.x);
    g_assert_cmpint(0, ==, parent.y);
}

void test_coord_parent_inplace() {
    futile_coord_s coord = {.x=3, .y=1, .z=2};
    g_assert(futile_coord_parent(&coord, &coord));
    g_assert_cmpint(1, ==, coord.z);
    g_assert_cmpint(1, ==, coord.x);
    g_assert_cmpint(0, ==, coord.y);
}

void test_coord_children() {
    futile_coord_s coord = {.x=0, .y=1, .z=1};
    futile_coord_s children[4];
    futile_coord_children(&coord, children);

    g_assert_cmpint(2, ==, children[0].z);
    g_assert_cmpint(0, ==, children[0].x);
    g_assert_cmpint(2, ==, children[0].y);

    g_assert_cmpint(2, ==, children[1].z);
    g_assert_cmpint(1, ==, children[1].x);
    g_assert_cmpint(2, ==, children[1].y);

    g_assert_cmpint(2, ==, children[2].z);
    g_assert_cmpint(0, ==, children[2].x);
    g_assert_cmpint(3, ==, children[2].y);

    g_assert_cmpint(2, ==, children[3].z);
    g_assert_cmpint(1, ==, children[3].x);
    g_assert_cmpint(3, ==, children[3].y);
}

void test_coord_not_valid() {
    futile_coord_s c = {.x=2, .y=2, .z=1};
    g_assert(!futile_coord_is_valid(&c));
}

void test_coord_valid() {
    futile_coord_s c = {.x=2, .y=2, .z=2};
    g_assert(futile_coord_is_valid(&c));
}

void test_coord_serialize_ok() {
    futile_coord_s c = {.x=1, .y=2, .z=3};
    char str[16];
    g_assert(futile_coord_serialize(&c, sizeof(str), str));
    g_assert_cmpstr("3/1/2", ==, str);
}

void test_coord_serialize_fail() {
    futile_coord_s c = {.x=1, .y=2, .z=3};
    char str[2];
    g_assert(!futile_coord_serialize(&c, sizeof(str), str));
}

void test_coord_deserialize_fail() {
    char *s = "something bogus";
    futile_coord_s c;
    g_assert(!futile_coord_deserialize(s, &c));
}

void test_coord_deserialize_fail_close() {
    char *s = "1/2/garbage";
    futile_coord_s c;
    g_assert(!futile_coord_deserialize(s, &c));
}

void test_coord_deserialize_fail_out_of_bounds() {
    char *s = "1/2/2";
    futile_coord_s c;
    g_assert(!futile_coord_deserialize(s, &c));
}

void test_coord_deserialize_fail_neg() {
    char *s = "1/1/-1";
    futile_coord_s c;
    g_assert(!futile_coord_deserialize(s, &c));
}

void test_coord_deserialize_ok() {
    char *s = "5/1/2";
    futile_coord_s c;
    g_assert(futile_coord_deserialize(s, &c));
    g_assert(c.z == 5);
    g_assert(c.x == 1);
    g_assert(c.y == 2);
}

void test_coord_deserialize_ok_trailing_newline() {
    char *s = "5/1/2\n";
    futile_coord_s c;
    g_assert(futile_coord_deserialize(s, &c));
    g_assert(c.z == 5);
    g_assert(c.x == 1);
    g_assert(c.y == 2);
}

void test_coord_print() {
    FILE *f = tmpfile();
    futile_coord_s c = {.x=1, .y=2, .z=3};
    futile_coord_print(&c, f);
    fseek(f, 0, SEEK_SET);
    char str[8];
    int n = fread(str, 1, sizeof(str), f);
    g_assert(n > 0 && n < 8);
    str[n] = '\0';
    g_assert_cmpstr("3/1/2", ==, str);
}

void test_coord_cmp() {
    futile_coord_s coord = {.x=2, .y=2, .z=2};
    futile_coord_s less[] = {
        {.x=1, .y=1, .z=1},
        {.x=2, .y=1, .z=2},
        {.x=2, .y=2, .z=1}
    };
    futile_coord_s greater[] = {
        {.x=3, .y=2, .z=2},
        {.x=2, .y=3, .z=2},
        {.x=2, .y=2, .z=3}
    };
    futile_coord_s equal = {.x=2, .y=2, .z=2};
    for (unsigned int i = 0; i < sizeof(less) / sizeof(futile_coord_s); i++) {
        g_assert(futile_coord_cmp(&less[i], &coord) < 0);
    }
    for (unsigned int i = 0; i < sizeof(greater) / sizeof(futile_coord_s); i++) {
        g_assert(futile_coord_cmp(&greater[i], &coord) > 0);
    }
    g_assert(futile_coord_cmp(&coord, &equal) == 0);
}

void test_coord_equal() {
    const int n = 5;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                futile_coord_s c1 = {.x=i, .y=j, .z=k};
                for (int x = 0; x < n; x++) {
                    for (int y = 0; y < n; y++) {
                        for (int z = 0; z < n; z++) {
                            futile_coord_s c2 = {.x=x, .y=y, .z=z};
                            bool cmp_equal = memcmp(&c1, &c2, sizeof(futile_coord_s)) == 0;
                            g_assert(cmp_equal == futile_coord_equal(&c1, &c2));
                        }
                    }
                }
            }
        }
    }
}

void _test_coord_marshall_up_to_zoom_5(futile_coord_s *coord, __attribute__((unused)) void *userdata) {
    futile_coord_s result;
    uint64_t x = futile_coord_marshall_int(coord);
    futile_coord_unmarshall_int(x, &result);
    g_assert(0 == futile_coord_cmp(coord, &result));
}

void test_coord_marshall_up_to_zoom_5() {
    futile_for_zoom_range(0, 5, _test_coord_marshall_up_to_zoom_5, NULL);
}

void test_coord_marshall_examples() {
    futile_coord_s coords[2] = {
        {.x=1002463,  .y=312816,   .z=20},
        {.x=12345678, .y=12345678, .z=30}
    };
    for (unsigned int i = 0; i < sizeof(coords) / sizeof(futile_coord_s); i++) {
        futile_coord_s result;
        uint64_t x = futile_coord_marshall_int(&coords[i]);
        futile_coord_unmarshall_int(x, &result);
        g_assert(0 == futile_coord_cmp(&coords[i], &result));
    }
}

void test_coord_int_zoom_up_examples() {
    futile_coord_s coords[] = {
        {.x=31, .y=31, .z=5},
        {.x=1, .y=1, .z=2}
    };
    futile_coord_s exp_coords[] = {
        {.x=15, .y=15, .z=4},
        {.x=0, .y=0, .z=1}
    };
    for (unsigned int i = 0; i < sizeof(coords) / sizeof(futile_coord_s); i++) {
        uint64_t coord_int = futile_coord_marshall_int(&coords[i]);
        uint64_t coord_parent_int = futile_coord_int_zoom_up(coord_int);
        futile_coord_s act_coord;
        futile_coord_unmarshall_int(coord_parent_int, &act_coord);
        g_assert(futile_coord_equal(&exp_coords[i], &act_coord));
    }
}

void test_coord_int_zoom_up_small_range() {
    const int n = 5;
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            for (int z = 1; z <= 5; z++) {
                futile_coord_s coord = {.x=x, .y=y, .z=z};
                futile_coord_s exp_parent;
                g_assert(futile_coord_parent(&coord, &exp_parent));
                uint64_t coord_int = futile_coord_marshall_int(&coord);
                uint64_t exp_parent_coord_int = futile_coord_marshall_int(&exp_parent);
                uint64_t act_parent_coord_int = futile_coord_int_zoom_up(coord_int);
                g_assert(exp_parent_coord_int == act_parent_coord_int);
            }
        }
    }
}

void test_explode_bounds() {
    futile_bounds_s bounds = {1, 2, 3, 4};
    double a, b, c, d;
    futile_explode_bounds(&bounds, &a, &b, &c, &d);
    g_assert_cmpfloat(1, ==, a);
    g_assert_cmpfloat(2, ==, b);
    g_assert_cmpfloat(3, ==, c);
    g_assert_cmpfloat(4, ==, d);
}

static int float_cmp(double x, double y, double eps) {
    return abs(x - y) < eps;
}

void test_coord_to_lnglat() {
    futile_coord_s c = {.x=19295, .y=24641, .z=16};
    double lng, lat;
    futile_coord_to_lnglat(&c, &lng, &lat);
    g_assert(float_cmp(lng, -74.0093994140625, 0.00001));
    g_assert(float_cmp(lat, 40.709792012434946, 0.00001));
}

void test_lnglat_to_coord() {
    futile_coord_s c;
    double lng = -74.0093994140625;
    double lat = 40.709792012434946;
    int zoom = 16;
    futile_lnglat_to_coord(lng, lat, zoom, &c);
    g_assert_cmpint(zoom,  ==, c.z);
    g_assert_cmpint(19295, ==, c.x);
    g_assert_cmpint(24641, ==, c.y);
}

void test_coord_to_bounds() {
    futile_coord_s c = {.x=19295, .y=24641, .z=16};
    futile_bounds_s b;
    futile_coord_to_bounds(&c, &b);
    g_assert(float_cmp(-74.009399414062, b.minx, 0.00001));
    g_assert(float_cmp(40.705627938206, b.miny, 0.00001));
    g_assert(float_cmp(-74.003906250000, b.maxx, 0.00001));
    g_assert(float_cmp(40.709792012435, b.maxy, 0.00001));
}

void test_bounds_to_multiple_coords() {
    double minx = -74.009399414062;
    double miny = 40.705627938206;
    double maxx = -74.003906250000;
    double maxy = 40.709792012435;
    futile_bounds_s bounds = {minx, miny, maxx, maxy};
    futile_coord_s coords[2];
    int n = futile_bounds_to_coords(&bounds, 16, coords);
    g_assert_cmpint(2, ==, n);
    g_assert_cmpint(coords[0].z, ==, 16);
    g_assert_cmpint(coords[0].x, ==, 19295);
    g_assert_cmpint(coords[0].y, ==, 24640);
    g_assert_cmpint(coords[1].z, ==, 16);
    g_assert_cmpint(coords[1].x, ==, 19296);
    g_assert_cmpint(coords[1].y, ==, 24641);
}

void test_bounds_to_single_coord() {
    double minx = -74.009399414062;
    double miny = 40.705627938206;
    double maxx = -74.0090;
    double maxy = 40.70563;
    futile_bounds_s bounds = {minx, miny, maxx, maxy};
    futile_coord_s coords[2];
    int n = futile_bounds_to_coords(&bounds, 16, coords);
    g_assert_cmpint(1, ==, n);
    g_assert_cmpint(coords[0].z, ==, 16);
    g_assert_cmpint(coords[0].x, ==, 19295);
    g_assert_cmpint(coords[0].y, ==, 24641);
}

void test_mercator_to_wgs84() {
    futile_point_s merc = {.x=-8233978.22, .y=4980225.91};
    futile_point_s latlng;
    futile_mercator_to_wgs84(&merc, &latlng);
    g_assert(float_cmp(-73.96708488464355, latlng.x, 0.00001));
    g_assert(float_cmp(40.781906259287, latlng.y, 0.00001));
}

void test_wgs84_to_mercator() {
    futile_point_s latlng = {.x=-73.96708488464355, .y=40.781906259287};
    futile_point_s merc;
    futile_wgs84_to_mercator(&latlng, &merc);
    g_assert(float_cmp(-8233978.22, merc.x, 0.00001));
    g_assert(float_cmp(4980225.91, merc.y, 0.00001));
}

void test_coord_to_mercator() {
    futile_coord_s coord = {.x=19302, .y=24623, .z=16};
    futile_point_s merc;
    futile_coord_to_mercator(&coord, &merc);
    g_assert(float_cmp(-8234408.183105, merc.x, 0.00001));
    g_assert(float_cmp(4980636.763062, merc.y, 0.00001));
}

void test_mercator_to_coord() {
    futile_point_s merc = {.x=-8233978.22, .y=4980225.91};
    int zoom = 16;
    futile_coord_s coord;
    futile_mercator_to_coord(&merc, zoom, &coord);
    g_assert_cmpint(16, ==, coord.z);
    g_assert_cmpint(19302, ==, coord.x);
    g_assert_cmpint(24623, ==, coord.y);
}

void test_coord_mercator_roundtrip() {
    futile_coord_s coord = {.x=19302, .y=24623, .z=16};
    futile_point_s merc;
    futile_coord_to_mercator(&coord, &merc);
    futile_coord_s coord_rountrip = {};
    futile_mercator_to_coord(&merc, 16, &coord_rountrip);
    g_assert_cmpint(coord.x, ==, coord_rountrip.x);
    g_assert_cmpint(coord.y, ==, coord_rountrip.y);
    g_assert_cmpint(coord.z, ==, coord_rountrip.z);
}

void test_coord_mercator_to_bounds() {
    futile_coord_s coord = {.x=38600, .y=49295, .z=17};
    futile_bounds_s bounds;
    futile_coord_to_mercator_bounds(&coord, &bounds);
    g_assert(float_cmp(-8235631.175558, bounds.minx, 0.00001));
    g_assert(float_cmp(4965349.357405, bounds.miny, 0.00001));
    g_assert(float_cmp(-8235325.427445, bounds.maxx, 0.00001));
    g_assert(float_cmp(4965655.105518, bounds.maxy, 0.00001));
}

void test_mercator_bounds_to_coords() {
    futile_bounds_s bounds = {-8235631.175558, 4965349.357405, -8235325.427445, 4965655.105518};
    futile_coord_s coords[2];
    int n = futile_mercator_bounds_to_coords(&bounds, 17, coords);
    g_assert_cmpint(n, ==, 2);
    g_assert_cmpint(coords[0].x, ==, 38600);
    g_assert_cmpint(coords[0].y, ==, 49295);
    g_assert_cmpint(coords[0].z, ==, 17);
    g_assert_cmpint(coords[1].x, ==, 38600);
    g_assert_cmpint(coords[1].y, ==, 49296);
    g_assert_cmpint(coords[1].z, ==, 17);
}

void test_coord_to_quadkey() {
    futile_coord_s coords[] = {
        {.x=2, .y=2, .z=3},
        {.x=1, .y=1, .z=1},
        {.x=0, .y=1, .z=2},
        {.x=5, .y=0, .z=3}
    };
    char *expected_quadkeys[] = {
        "030",
        "3",
        "02",
        "101"
    };
    for (unsigned int i = 0; i < sizeof(coords) / sizeof(futile_coord_s); i++) {
        char quadkey[4];
        futile_coord_to_quadkey(&coords[i], quadkey);
        g_assert(strcmp(expected_quadkeys[i], quadkey) == 0);
    }
}

void test_quadkey_to_coord() {
    char *quadkeys[] = {
        "030",
        "3",
        "02",
        "101"
    };
    futile_coord_s expected_coords[] = {
        {.x=2, .y=2, .z=3},
        {.x=1, .y=1, .z=1},
        {.x=0, .y=1, .z=2},
        {.x=5, .y=0, .z=3}
    };
    for (unsigned int i = 0; i < sizeof(quadkeys) / sizeof(char *); i++) {
        futile_coord_s coord = {};
        futile_quadkey_to_coord(quadkeys[i], strlen(quadkeys[i]), &coord);
        g_assert_cmpint(expected_coords[i].x, ==, coord.x);
        g_assert_cmpint(expected_coords[i].y, ==, coord.y);
        g_assert_cmpint(expected_coords[i].z, ==, coord.z);
    }
}

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

    zero_coords(coords, n);
    g_assert(!futile_for_zoom_range_array(&cursor, &group));
    g_assert(group.n == 10);

    g_assert(futile_for_zoom_range_array(&cursor, &group));
    g_assert_cmpint(1, ==, group.n);
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

void noop(futile_coord_s *coord, void *ignored) {
}

unsigned int now() {
    return time(NULL);
}

void test_timing_for_zoom_range_array() {
    unsigned int start, took, zoom_start, zoom_until;
    zoom_start = 0;
    zoom_until = 16;

    start = now();
    futile_for_zoom_range(zoom_start, zoom_until, noop, NULL);
    took = now() - start;
    printf("\nfor zoom range: %u\n", took);

    futile_coord_cursor_s cursor = {};
    // cursor.zoom_start = zoom_start;
    cursor.zoom_until = zoom_until;
    size_t n = 100;
    futile_coord_s *coords = malloc(sizeof(futile_coord_s) * n);
    futile_coord_group_s group = {.coords=coords, .n=n};

    start = now();
    while (1) {
        if (futile_for_zoom_range_array(&cursor, &group)) {
            break;
        }
    }
    free(coords);
    took = now() - start;
    printf("array: %u\n", took);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/coord/zoom/down-one", test_coord_zoom_down_one);
    g_test_add_func("/coord/zoom/down-two", test_coord_zoom_down_two);
    g_test_add_func("/coord/zoom/up", test_coord_zoom_up);
    g_test_add_func("/coord/coord-parent-invalid", test_coord_parent_invalid);
    g_test_add_func("/coord/coord-parent-valid", test_coord_parent_valid);
    g_test_add_func("/coord/coord-parent-valid", test_coord_parent_inplace);
    g_test_add_func("/coord/coord-children", test_coord_children);
    g_test_add_func("/coord/coord-not-valid", test_coord_not_valid);
    g_test_add_func("/coord/coord-valid", test_coord_valid);
    g_test_add_func("/coord/serialize-ok", test_coord_serialize_ok);
    g_test_add_func("/coord/serialize-fail", test_coord_serialize_fail);
    g_test_add_func("/coord/deserialize-fail", test_coord_deserialize_fail);
    g_test_add_func("/coord/deserialize-fail-close", test_coord_deserialize_fail_close);
    g_test_add_func("/coord/deserialize-fail-out-of-bounds", test_coord_deserialize_fail_out_of_bounds);
    g_test_add_func("/coord/deserialize-fail-neg", test_coord_deserialize_fail_neg);
    g_test_add_func("/coord/deserialize-ok", test_coord_deserialize_ok);
    g_test_add_func("/coord/deserialize-ok-trailing-newline", test_coord_deserialize_ok_trailing_newline);
    g_test_add_func("/coord/coord_print", test_coord_print);
    g_test_add_func("/coord/coord_cmp", test_coord_cmp);
    g_test_add_func("/coord/coord_equal", test_coord_equal);
    g_test_add_func("/coord/marshall/int", test_coord_marshall_up_to_zoom_5);
    g_test_add_func("/coord/marshall/examples", test_coord_marshall_examples);
    g_test_add_func("/coord/int/zoom-up/examples", test_coord_int_zoom_up_examples);
    g_test_add_func("/coord/int/zoom-up/small-range", test_coord_int_zoom_up_small_range);

    g_test_add_func("/geo/explode-bounds", test_explode_bounds);
    g_test_add_func("/geo/coord->lnglat", test_coord_to_lnglat);
    g_test_add_func("/geo/lnglat->coord", test_lnglat_to_coord);
    g_test_add_func("/geo/coord->bounds", test_coord_to_bounds);
    g_test_add_func("/geo/bounds->coords", test_bounds_to_multiple_coords);
    g_test_add_func("/geo/bounds->coord", test_bounds_to_single_coord);
    g_test_add_func("/geo/mercator->wgs84", test_mercator_to_wgs84);
    g_test_add_func("/geo/wgs84->mercator", test_wgs84_to_mercator);
    g_test_add_func("/geo/coord->mercator", test_coord_to_mercator);
    g_test_add_func("/geo/mercator->coord", test_mercator_to_coord);
    g_test_add_func("/geo/mercator-coord-roundtrip", test_coord_mercator_roundtrip);
    g_test_add_func("/geo/coord->mercator-bounds", test_coord_mercator_to_bounds);
    g_test_add_func("/geo/mercator-bounds->coord", test_mercator_bounds_to_coords);
    g_test_add_func("/geo/coord->quadkey", test_coord_to_quadkey);
    g_test_add_func("/geo/quadkey->coord", test_quadkey_to_coord);

    g_test_add_func("/tile/zoom-range", test_tile_for_zoom_range);
    g_test_add_func("/tile/zoom-range-array", test_tile_for_zoom_range_array);
    g_test_add_func("/tile/zoom-range-array-multiple", test_tile_for_zoom_range_array_multiple);
    g_test_add_func("/tile/coord-zoom-range", test_tile_for_coord_zoom_range);
    g_test_add_func("/tile/coord-parents", test_tile_parents);
    g_test_add_func("/tile/n-for-zoom", test_tile_n_for_zoom);
    g_test_add_func("/tile/for-tile/bounds", test_tile_for_bounds);

    // g_test_add_func("/timing/for-zoom-range-array", test_timing_for_zoom_range_array);

    return g_test_run();
}
