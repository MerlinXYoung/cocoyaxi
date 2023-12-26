#define CATCH_CONFIG_MAIN
#include "co/os.h"

#include "helper.h"

namespace test {

TEST_CASE("env", "[so]") {
    EXPECT_EQ(os::env("CO_TEST"), fastring());
    os::env("CO_TEST", "777");
    EXPECT_EQ(os::env("CO_TEST"), "777");
    os::env("CO_TEST", "");
    EXPECT_EQ(os::env("CO_TEST"), fastring());
}

TEST_CASE("homedir", "[so]") { EXPECT_NE(os::homedir(), fastring()); }

TEST_CASE("cwd", "[so]") { EXPECT_NE(os::cwd(), fastring()); }

TEST_CASE("exename", "[so]") {
    EXPECT_NE(os::exepath(), fastring());
    EXPECT_NE(os::exedir(), fastring());
    EXPECT_NE(os::exename(), fastring());
    EXPECT(os::exepath().starts_with(os::exedir()));
    EXPECT(os::exepath().ends_with(os::exename()));
    EXPECT(os::exename().starts_with("unitest"));
}

TEST_CASE("pid", "[so]") { EXPECT_GE(os::pid(), 0); }

TEST_CASE("cpunum", "[so]") { EXPECT_GT(os::cpunum(), 0); }

}  // namespace test
