#include <inttypes.h>

#include <cstdio>

#include "benchmark.h"
#include "co/flag.h"
DEF_string(out, "./fast.h.in", "output file");

static uint16_t g_itoh_tb[256];
static uint32_t g_itoa_tb[10000];
struct Initializer {
    Initializer();
    ~Initializer() = default;
};
Initializer::Initializer() {
    for (int i = 0; i < 256; ++i) {
        char* b = (char*)(g_itoh_tb + i);
        b[0] = "0123456789abcdef"[i >> 4];
        b[1] = "0123456789abcdef"[i & 0x0f];
    }

    for (int i = 0; i < 10000; ++i) {
        char* b = (char*)(g_itoa_tb + i);
        b[3] = (char)(i % 10 + '0');
        b[2] = (char)(i % 100 / 10 + '0');
        b[1] = (char)(i % 1000 / 100 + '0');
        b[0] = (char)(i / 1000);

        // digits of i: (b[0] >> 4) + 1
        if (i > 999) {
            b[0] |= (3 << 4);  // 0x30
        } else if (i > 99) {
            b[0] |= (2 << 4);  // 0x20
        } else if (i > 9) {
            b[0] |= (1 << 4);  // 0x10
        }
    }
}

static Initializer g_initializer;
int main(int argc, char** argv) {
    flag::parse(argc, argv);
    FILE* f = fopen(FLG_out.c_str(), "w+");
    fprintf(f, "\n");
    fprintf(f, "static uint16_t g_itoh_tb[256] = {\n");
    for (int i = 0; i < 256; i += 8) {
        fprintf(f, "0x%X,", g_itoh_tb[i]);
        fprintf(f, "0x%X,", g_itoh_tb[i + 1]);
        fprintf(f, "0x%X,", g_itoh_tb[i + 2]);
        fprintf(f, "0x%X,", g_itoh_tb[i + 3]);
        fprintf(f, "0x%X,", g_itoh_tb[i + 4]);
        fprintf(f, "0x%X,", g_itoh_tb[i + 5]);
        fprintf(f, "0x%X,", g_itoh_tb[i + 6]);
        fprintf(f, "0x%X,\n", g_itoh_tb[i + 7]);
    }
    fprintf(f, "};\n");

    fprintf(f, "\n");
    fprintf(f, "static uint16_t g_itoa_tb[10000] = {\n");
    for (int i = 0; i < 10000; i += 10) {
        fprintf(f, "0x%X,", g_itoa_tb[i]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 1]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 2]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 3]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 4]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 5]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 6]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 7]);
        fprintf(f, "0x%X,", g_itoa_tb[i + 8]);
        fprintf(f, "0x%X,\n", g_itoa_tb[i + 9]);
    }
    fprintf(f, "};\n");
    fflush(f);
    fclose(f);
    bm::run_benchmarks();
    return 0;
}