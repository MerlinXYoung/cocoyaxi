#pragma once

#include <cstddef>
#include <cstdint>

#define DISALLOW_COPY_AND_ASSIGN(T) \
    T(const T&) = delete;           \
    void operator=(const T&) = delete

#if SIZE_MAX == UINT64_MAX
#define __arch64 1
#else
#define __arch32 1
#endif

#ifdef _MSC_VER
#ifndef __thread
#define __thread __declspec(thread)
#endif
#else
#ifndef __forceinline
#define __forceinline __attribute__((always_inline)) inline
#endif
#endif

#ifndef unlikely
#if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define unlikely(x) (x)
#endif
#endif

#ifndef PP_STRIFY
#define _PP_STRIFY(x) #x
#define PP_STRIFY(x) _PP_STRIFY(x)
#endif

#ifndef PP_CONCAT
#define _PP_CONCAT(x, y) x##y
#define PP_CONCAT(x, y) _PP_CONCAT(x, y)
#endif

// generated from config.h.in
#include "config.h"

// __coapi: export symbols in shared library (libco.so or co.dll)
#if COOST_SHARED > 0
#ifdef _WIN32
#ifdef BUILDING_CO_SHARED
#define __coapi __declspec(dllexport)
#else
#define __coapi __declspec(dllimport)
#endif
#else
#define __coapi __attribute__((visibility("default")))
#endif
#else
#define __coapi
#endif
