#include <criterion/criterion.h>

Test(dbms, hello) {
    cr_assert_str_eq("hello", "hello");
}

Test(dbms, functionality1) {
    // Add test for dbms functionality 1
}

Test(dbms, functionality2) {
    // Add test for dbms functionality 2
}