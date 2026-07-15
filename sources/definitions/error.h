#ifndef DEFINITIONS_ERROR_H
#define DEFINITIONS_ERROR_H

#include "integer.h"

typedef int Error;

#define OK 0
#define ERROR -1
#define ERROR_OUT_OF_MEMORY -2
#define ERROR_OVERFLOW -3

#define IS_OK(result) ((result) == OK)
#define IS_ERROR(result) ((result) != OK)

#define TRY(action) do { \
	Error result = (action); \
	if IS_ERROR(result) return result; \
} while (0)

#define TRY_OR_ELSE(action, execute) do { \
	Error result = (action); \
	if IS_ERROR(result) { \
		execute; \
		return result; \
	} \
} while (0)

#define TRY_SAFE(action) (action);

#define TRY_MEMORY(memory) do { \
	if ((memory) == NULL) { \
		return ERROR_OUT_OF_MEMORY; \
	} \
} while (0)

#define TRY_ADD(a, b) do { \
	if ((b) > USIZE_MAX - (a)) { \
		return ERROR_OVERFLOW; \
	} \
} while (0)

#define TRY_MULTIPLY(a, b) do { \
	usize _a = (a); \
	if ((_a) > 0 && (b) > USIZE_MAX / (_a)) { \
		return ERROR_OVERFLOW; \
	} \
} while (0)

#define TRY_SIZEOF(type, count) \
	TRY_MULTIPLY(sizeof(type), (usize)(count))

#define SIZEOF(type, count) (sizeof(type) * (usize)(count))

#endif /* DEFINITIONS_ERROR_H */