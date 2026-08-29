#ifndef DEFINITIONS_ERROR_H
#define DEFINITIONS_ERROR_H

#include "integer.h"

typedef int Error;

#define OK 0
#define ERROR -1
#define ERROR_OUT_OF_MEMORY -2
#define ERROR_OVERLAPPING_MEMORY_REGIONS -3
#define ERROR_INTEGER_OVERFLOW -4

#define ERROR_STRING_OR(result, error) ( \
	(result) == ERROR \
		? (error) \
	: (result) == ERROR_OUT_OF_MEMORY \
		? "out of memory" \
	: (result) == ERROR_OVERLAPPING_MEMORY_REGIONS \
		? "overlapping memory regions" \
	: (result) == ERROR_INTEGER_OVERFLOW \
		? "integer overflow" \
	: (result) != OK \
		? "unknown error" \
	: "ok")

#define ERROR_STRING(result) \
	ERROR_STRING_OR("generic error")

#define IS_OK(result) ((result) == OK)
#define IS_ERROR(result) ((result) != OK)

#define TRY_SAFE(action) action

#define TRY(action) do { \
	Error _result = (action); \
	if IS_ERROR(_result) return _result; \
} while (0)

#define TRY_OR_ELSE(action, execute) do { \
	Error _result = (action); \
	if IS_ERROR(_result) { \
		execute; \
		return _result; \
	} \
} while (0)

#define TRY_OR_ERROR(action, execute) do { \
	Error _result = (action); \
	if IS_ERROR(_result) { \
		execute; \
	} \
} while (0)

#define TRY_NEW_MEMORY(memory) do { \
	if ((memory) == NULL) { \
		return ERROR_OUT_OF_MEMORY; \
	} \
} while (0)

#define TRY_NEW_MEMORY_OR_ELSE(memory, execute) do { \
	if ((memory) == NULL) { \
		execute; \
		return ERROR_OUT_OF_MEMORY; \
	} \
} while (0)

#define TRY_NEW_MEMORY_OR_ERROR(memory, execute) do { \
	if ((memory) == NULL) { \
		execute; \
	} \
} while (0)

#define TRY_MEMORY_REGIONS(a, aa, b, bb) do { \
	if ((aa) > (b) && (a) < (bb)) { \
		return ERROR_OVERLAPPING_MEMORY_REGIONS; \
	} \
} while (0)

#define TRY_MEMORY_REGIONS_OR_ELSE(a, aa, b, bb, execute) do { \
	if ((aa) > (b) && (a) < (bb)) { \
		execute; \
		return ERROR_OVERLAPPING_MEMORY_REGIONS; \
	} \
} while (0)

#define TRY_MEMORY_REGIONS_OR_ERROR(a, aa, b, bb, execute) do { \
	if ((aa) > (b) && (a) < (bb)) { \
		execute; \
	} \
} while (0)

#define TRY_ADD(a, b) do { \
	if ((b) > USIZE_MAX - (a)) { \
		return ERROR_INTEGER_OVERFLOW; \
	} \
} while (0)

#define TRY_ADD_OR_ELSE(a, b, execute) do { \
	if ((b) > USIZE_MAX - (a)) { \
		execute; \
		return ERROR_INTEGER_OVERFLOW; \
	} \
} while (0)

#define TRY_ADD_OR_ERROR(a, b, execute) do { \
	if ((b) > USIZE_MAX - (a)) { \
		execute; \
	} \
} while (0)

#define TRY_MULTIPLY(a, b) do { \
	usize _a = (a); \
	if ((_a) > 0 && (b) > USIZE_MAX / (_a)) { \
		return ERROR_INTEGER_OVERFLOW; \
	} \
} while (0)

#define TRY_MULTIPLY_OR_ELSE(a, b, execute) do { \
	usize _a = (a); \
	if ((_a) > 0 && (b) > USIZE_MAX / (_a)) { \
		execute; \
		return ERROR_INTEGER_OVERFLOW; \
	} \
} while (0)

#define TRY_MULTIPLY_OR_ERROR(a, b, execute) do { \
	usize _a = (a); \
	if ((_a) > 0 && (b) > USIZE_MAX / (_a)) { \
		execute; \
	} \
} while (0)

#define TRY_SIZEOF(type, count) \
	TRY_MULTIPLY(sizeof(type), (usize)(count))

#define TRY_SIZEOF_OR_ELSE(type, count, execute) \
	TRY_MULTIPLY_OR_ELSE(sizeof(type), (usize)(count), execute)

#define TRY_SIZEOF_OR_ERROR(type, count, execute) \
	TRY_MULTIPLY_OR_ERROR(sizeof(type), (usize)(count), execute)

#endif /* DEFINITIONS_ERROR_H */