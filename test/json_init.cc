#include <inttypes.h>

#include <cstdio>

#include "co/benchmark.h"
#include "co/flag.h"
DEF_string(out, "./json.h.in", "output file");

static char g_s2e_tb[256];
static char g_e2s_tb[256];
static char g_hex_tb[256];
struct Initializer {
    Initializer();
    ~Initializer() = default;
};

Initializer::Initializer() {
    g_s2e_tb[(uint8_t)'r'] = '\r';
    g_s2e_tb[(uint8_t)'n'] = '\n';
    g_s2e_tb[(uint8_t)'t'] = '\t';
    g_s2e_tb[(uint8_t)'b'] = '\b';
    g_s2e_tb[(uint8_t)'f'] = '\f';
    g_s2e_tb[(uint8_t)'"'] = '"';
    g_s2e_tb[(uint8_t)'\\'] = '\\';
    g_s2e_tb[(uint8_t)'/'] = '/';
    g_s2e_tb[(uint8_t)'u'] = 'u';

    g_e2s_tb[(uint8_t)'\r'] = 'r';
    g_e2s_tb[(uint8_t)'\n'] = 'n';
    g_e2s_tb[(uint8_t)'\t'] = 't';
    g_e2s_tb[(uint8_t)'\b'] = 'b';
    g_e2s_tb[(uint8_t)'\f'] = 'f';
    g_e2s_tb[(uint8_t)'\"'] = '"';
    g_e2s_tb[(uint8_t)'\\'] = '\\';

    memset(g_hex_tb, 16, 256);
    for (char c = '0'; c <= '9'; ++c) g_hex_tb[(uint8_t)c] = c - '0';
    for (char c = 'A'; c <= 'F'; ++c) g_hex_tb[(uint8_t)c] = c - 'A' + 10;
    for (char c = 'a'; c <= 'f'; ++c) g_hex_tb[(uint8_t)c] = c - 'a' + 10;
}

static Initializer g_initializer;
int main(int argc, char** argv) {
    flag::parse(argc, argv);
    FILE* f = fopen(FLG_out.c_str(), "w+");
    fprintf(f, "\n");
    fprintf(f, "static char g_s2e_tb[256] = {\n");
    for (int i = 0; i < 256; i += 8) {
        fprintf(f, "0x%X,", g_s2e_tb[i]);
        fprintf(f, "0x%X,", g_s2e_tb[i + 1]);
        fprintf(f, "0x%X,", g_s2e_tb[i + 2]);
        fprintf(f, "0x%X,", g_s2e_tb[i + 3]);
        fprintf(f, "0x%X,", g_s2e_tb[i + 4]);
        fprintf(f, "0x%X,", g_s2e_tb[i + 5]);
        fprintf(f, "0x%X,", g_s2e_tb[i + 6]);
        fprintf(f, "0x%X,\n", g_s2e_tb[i + 7]);
    }
    fprintf(f, "};\n");

    fprintf(f, "\n");
    fprintf(f, "static char g_e2s_tb[256] = {\n");
    for (int i = 0; i < 256; i += 8) {
        fprintf(f, "0x%X,", g_e2s_tb[i]);
        fprintf(f, "0x%X,", g_e2s_tb[i + 1]);
        fprintf(f, "0x%X,", g_e2s_tb[i + 2]);
        fprintf(f, "0x%X,", g_e2s_tb[i + 3]);
        fprintf(f, "0x%X,", g_e2s_tb[i + 4]);
        fprintf(f, "0x%X,", g_e2s_tb[i + 5]);
        fprintf(f, "0x%X,", g_e2s_tb[i + 6]);
        fprintf(f, "0x%X,\n", g_e2s_tb[i + 7]);
    }
    fprintf(f, "};\n");

    fprintf(f, "\n");
    fprintf(f, "static char g_hex_tb[256] = {\n");
    for (int i = 0; i < 256; i += 8) {
        fprintf(f, "0x%X,", g_hex_tb[i]);
        fprintf(f, "0x%X,", g_hex_tb[i + 1]);
        fprintf(f, "0x%X,", g_hex_tb[i + 2]);
        fprintf(f, "0x%X,", g_hex_tb[i + 3]);
        fprintf(f, "0x%X,", g_hex_tb[i + 4]);
        fprintf(f, "0x%X,", g_hex_tb[i + 5]);
        fprintf(f, "0x%X,", g_hex_tb[i + 6]);
        fprintf(f, "0x%X,\n", g_hex_tb[i + 7]);
    }

    fprintf(f, "};\n");

    fflush(f);
    fclose(f);
    bm::run_benchmarks();
    return 0;
}