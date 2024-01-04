#include "co/co.h"
#include "co/unitest.h"

int main(int argc, char** argv) {
    flag::parse(argc, argv);
    unitest::run_tests();
    return 0;
}
