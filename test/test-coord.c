#include <stdio.h>
#include <glib.h>
#include <string.h>
#include "coord.h"
#include "tile.h"

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

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/coord/zoom/down-one", test_coord_zoom_down_one);
    g_test_add_func("/coord/zoom/down-two", test_coord_zoom_down_two);
    g_test_add_func("/coord/zoom/up", test_coord_zoom_up);
    g_test_add_func("/coord/coord-parent-invalid", test_coord_parent_invalid);
    g_test_add_func("/coord/coord-parent-valid", test_coord_parent_valid);
    g_test_add_func("/coord/coord-parent-valid", test_coord_parent_inplace);
    g_test_add_func("/coord/coord-children", test_coord_children);
    g_test_add_func("/coord/serialize-ok", test_coord_serialize_ok);
    g_test_add_func("/coord/serialize-fail", test_coord_serialize_fail);
    g_test_add_func("/coord/deserialize-fail", test_coord_deserialize_fail);
    g_test_add_func("/coord/deserialize-fail-close", test_coord_deserialize_fail_close);
    g_test_add_func("/coord/deserialize-ok", test_coord_deserialize_ok);
    g_test_add_func("/coord/deserialize-ok-trailing-newline", test_coord_deserialize_ok_trailing_newline);
    g_test_add_func("/coord/coord_print", test_coord_print);
    g_test_add_func("/coord/coord_cmp", test_coord_cmp);
    g_test_add_func("/coord/coord_equal", test_coord_equal);
    g_test_add_func("/coord/marshall/int", test_coord_marshall_up_to_zoom_5);
    g_test_add_func("/coord/marshall/examples", test_coord_marshall_examples);
    g_test_add_func("/coord/int/zoom-up/examples", test_coord_int_zoom_up_examples);
    g_test_add_func("/coord/int/zoom-up/small-range", test_coord_int_zoom_up_small_range);

    return g_test_run();
}
