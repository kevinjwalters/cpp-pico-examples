/**
 * Copyright (c) 2023 Kevin J. Walters
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * A quick benchmark of various styles of iteration over an array
 * intended for use on the Pi Pico
 */

/**
 * TODO
 * General tidying
 * Check compiler options for optimisation and similar
 */


#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <array>
#include <vector>
#include <iostream>
#include <iterator>

#include "pico.h"
#include "pico/stdlib.h"


#define RUNS_PER_TEST 10

#define ARRAY_LEN 8192

#define THE_NUMBER 123  // Used for some basic maths

static const char *VERSION = "1.0.1";

// 0 here means leave it alone for default
static uint32_t target_frequencies_k[] = {
    0,
    25'000,
    125'000,  // Pi Pico default
    133'000,  // RP2040 maximum
    200'000   // typical over-clocking
};

static int debug = 0;

// Relevant / interesting links (perhaps)
//
// https://stackoverflow.com/questions/6706059/c-best-way-to-use-for-loop
// https://forums.raspberrypi.com/viewtopic.php?t=301902
// https://www.raspberrypi.com/documentation//pico-sdk/vreg_8h.html


// Lots of similar code below, templates, macros, etc could be used...

struct iteratingBenchmarkResults {
    absolute_time_t start;
    int64_t duration_instantiation;
    int64_t duration_iteration;
};

typedef struct iteratingBenchmarkResults IteratingBenchmarkResults;

// C array[]
IteratingBenchmarkResults bm_classic_array(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    int32_t classic_array[ARRAY_LEN];

    t2 = get_absolute_time();
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        classic_array[i] = THE_NUMBER;
    }
    int32_t total = 0;
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        total += classic_array[i];
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(classic_array), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}

// C++ STL std::array (fixed size array)
IteratingBenchmarkResults bm_cpp_array(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::array<int32_t, ARRAY_LEN> cpp_array;
    t2 = get_absolute_time();
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        cpp_array[i] = THE_NUMBER;
    }
    int32_t total = 0;
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        total += cpp_array[i];
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_array), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}


IteratingBenchmarkResults bm_cpp_array_iter1(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::array<int32_t, ARRAY_LEN> cpp_array;
    t2 = get_absolute_time();
    for (auto it = cpp_array.begin(); it != cpp_array.end(); it++) {
        *it = THE_NUMBER;
    }
    int32_t total = 0;
    for (auto it = cpp_array.begin(); it != cpp_array.end(); it++) {
        total += *it;
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_array), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}

IteratingBenchmarkResults bm_cpp_array_iter2(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::array<int32_t, ARRAY_LEN> cpp_array;
    t2 = get_absolute_time();
    for (auto& elem : cpp_array) {
        elem = THE_NUMBER;
    }
    int32_t total = 0;
    for (const auto& elem : cpp_array) {
        total += elem;
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_array), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}

IteratingBenchmarkResults bm_cpp_array_at(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::array<int32_t, ARRAY_LEN> cpp_array;
    t2 = get_absolute_time();
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        cpp_array.at(i) = THE_NUMBER;
    }
    int32_t total = 0;
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        total += cpp_array.at(i);
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_array), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}


// C++ STL std::vector (variable size array)
IteratingBenchmarkResults bm_cpp_vector(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::vector<int32_t> cpp_vector(ARRAY_LEN);
    t2 = get_absolute_time();
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        cpp_vector[i] = THE_NUMBER;
    }
    int32_t total = 0;
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        total += cpp_vector[i];
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_vector), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}

IteratingBenchmarkResults bm_cpp_vector_iter1(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::vector<int32_t> cpp_vector(ARRAY_LEN);
    t2 = get_absolute_time();
    for (auto it = cpp_vector.begin(); it != cpp_vector.end(); it++) {
        *it = THE_NUMBER;
    }
    int32_t total = 0;
    for (auto it = cpp_vector.begin(); it != cpp_vector.end(); it++) {
        total += *it;
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_vector), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}

IteratingBenchmarkResults bm_cpp_vector_iter2(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::vector<int32_t> cpp_vector(ARRAY_LEN);
    t2 = get_absolute_time();
    for (auto& elem : cpp_vector) {
        elem = THE_NUMBER;
    }
    int32_t total = 0;
    for (const auto& elem : cpp_vector) {
        total += elem;
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_vector), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}

IteratingBenchmarkResults bm_cpp_vector_at(void) {
    absolute_time_t t1, t2, t3;
    t1 = get_absolute_time();
    std::vector<int32_t> cpp_vector(ARRAY_LEN);
    t2 = get_absolute_time();
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        cpp_vector.at(i) = THE_NUMBER;
    }
    int32_t total = 0;
    for (int32_t i=0; i < ARRAY_LEN; i++) {
        total += cpp_vector.at(i);
    }
    t3 = get_absolute_time();
    int64_t duration_us = absolute_time_diff_us(t2, t3);

    if (debug) printf("size %zu bytes, duration %" PRId64 " us\n", sizeof(cpp_vector), duration_us);
    return { t1, absolute_time_diff_us(t1, t2), duration_us };
}


struct iteratingBenchmark {
    IteratingBenchmarkResults (*func)(void);
    const char *name;
    const char *data_store;
    const char *iterator_style;
};

typedef struct iteratingBenchmark IteratingBenchmark;

static IteratingBenchmark benchmarks[] = {
    {bm_classic_array, "bm_classic_array", "C", "C"},
    {bm_cpp_array, "bm_cpp_array", "array", "C"},
    {bm_cpp_array_iter1, "bm_cpp_array_iter1", "array", "iter1"},
    {bm_cpp_array_iter2, "bm_cpp_array_iter2", "array", "iter2"},
    {bm_cpp_array_at, "bm_cpp_array_at", "array", "at"},
    {bm_cpp_vector, "bm_cpp_vector", "vector", "C"},
    {bm_cpp_vector_iter1, "bm_cpp_vector_iter1", "vector", "iter1"},
    {bm_cpp_vector_iter2, "bm_cpp_vector_iter2", "vector", "iter2"},
    {bm_cpp_vector_at, "bm_cpp_vector_at", "vector", "at"}
};


int main() {
    stdio_init_all();

    if (debug) printf("A look at the performance of different array types in C++ on Pi Pico\n");
    sleep_ms(5'000);

    bool clock_default_run = false;

    printf("start_ms,benchmark,data_store,iterator_style,duration_us,run,core,cpu_frequency,compiler_flags\n");
    while (1) {
        for (int target_freq_k : target_frequencies_k) {
            if (target_freq_k) {
                set_sys_clock_khz(target_freq_k, true);
            } else {
                if (clock_default_run) {
                    continue;  // Only do the benchmark once at the power on default
                } else {
                    clock_default_run = true;
                }
            };
            for (const IteratingBenchmark &bm : benchmarks) {
                if (debug) printf("Running benchmark %10s : %10s : %30s"
                                  "(core %d, %d MHz, default V)\n",
                                  bm.data_store, bm.iterator_style, bm.name,
                                  0, target_freq_k * 1000);
                for (int run=0; run < RUNS_PER_TEST; run++) {
                    IteratingBenchmarkResults bm_res = bm.func();
                    printf("%" PRIu32 ",\"%s\",\"%s\",\"%s\",%" PRId64 ",%d,%d,%" PRIu32 ",\"%s\"\n",
                           to_ms_since_boot(bm_res.start),
                           bm.name, bm.data_store, bm.iterator_style,
                           bm_res.duration_iteration,
                           run,
                           0, target_freq_k * 1000,
                           "(pico_project default)");
                }
                sleep_ms(1'000);
            };
        };
        sleep_ms(15'000);
    };

    return 0;
}

