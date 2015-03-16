#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "geo.h"

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

void test_num_to_deg() {
    futile_coord_s c = {.x=19295, .y=24641, .z=16};
    double lng, lat;
    futile_num_to_deg(&c, &lng, &lat);
    g_assert(float_cmp(lng, -74.0093994140625, 0.00001));
    g_assert(float_cmp(lat, 40.709792012434946, 0.00001));
}

void test_deg_to_num() {
    futile_coord_s c;
    double lng = -74.0093994140625;
    double lat = 40.709792012434946;
    int zoom = 16;
    futile_deg_to_num(lng, lat, zoom, &c);
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
    for (int i = 0; i < sizeof(coords) / sizeof(futile_coord_s); i++) {
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
    for (int i = 0; i < sizeof(quadkeys) / sizeof(char *); i++) {
        futile_coord_s coord;
        futile_quadkey_to_coord(quadkeys[i], strlen(quadkeys[i]), &coord);
        g_assert_cmpint(expected_coords[i].x, ==, coord.x);
        g_assert_cmpint(expected_coords[i].y, ==, coord.y);
        g_assert_cmpint(expected_coords[i].z, ==, coord.z);
    }
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/geo/explode-bounds", test_explode_bounds);
    g_test_add_func("/geo/num->deg", test_num_to_deg);
    g_test_add_func("/geo/deg->num", test_deg_to_num);
    g_test_add_func("/geo/coord->bounds", test_coord_to_bounds);
    g_test_add_func("/geo/bounds->coords", test_bounds_to_multiple_coords);
    g_test_add_func("/geo/bounds->coord", test_bounds_to_single_coord);
    g_test_add_func("/geo/mercator->wgs84", test_mercator_to_wgs84);
    g_test_add_func("/geo/wgs84->mercator", test_wgs84_to_mercator);
    g_test_add_func("/geo/coord->mercator", test_coord_to_mercator);
    g_test_add_func("/geo/mercator->coord", test_mercator_to_coord);
    g_test_add_func("/geo/mercator-coord-roundtrip", test_coord_mercator_roundtrip);
    g_test_add_func("/geo/coord->quadkey", test_coord_to_quadkey);
    g_test_add_func("/geo/quadkey->coord", test_quadkey_to_coord);

    return g_test_run();
}
