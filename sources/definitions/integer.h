#ifndef DEFINITIONS_INTEGER_H
#define DEFINITIONS_INTEGER_H

#include <stdint.h>
#include <stddef.h>

typedef int8_t i8;
#define I8_MIN INT8_MIN
#define I8_MAX INT8_MAX

typedef int16_t i16;
#define I16_MIN INT16_MIN
#define I16_MAX INT16_MAX

typedef int32_t i32;
#define I32_MIN INT32_MIN
#define I32_MAX INT32_MAX

typedef int64_t i64;
#define I64_MIN INT64_MIN
#define I64_MAX INT64_MAX

typedef ptrdiff_t isize;
#define ISIZE_MIN PTRDIFF_MIN
#define ISIZE_MAX PTRDIFF_MAX

typedef uint8_t u8;
#define U8_MAX UINT8_MAX

typedef uint16_t u16;
#define U16_MAX UINT16_MAX

typedef uint32_t u32;
#define U32_MAX UINT32_MAX

typedef uint64_t u64;
#define U64_MAX UINT64_MAX

typedef size_t usize;
#define USIZE_MAX SIZE_MAX

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif /* DEFINITIONS_INTEGER_H */