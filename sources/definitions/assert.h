#ifndef DEFINITIONS_ASSERT_H
#define DEFINITIONS_ASSERT_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "colors.h"

#ifndef NDEBUG

#define ASSERT(condition) do { \
	if (!(condition)) { \
		fprintf(stderr, \
			ANSI_COLOR_BOLD ANSI_COLOR_FUCHSIA \
			"%s:%d: assert violation" ANSI_COLOR_RESET "\n" \
			"  " ANSI_COLOR_BOLD ANSI_COLOR_WHITE \
			"condition:" ANSI_COLOR_RESET " (%s)\n" \
			"  " ANSI_COLOR_BOLD ANSI_COLOR_WHITE \
			"function:" ANSI_COLOR_RESET " %s()\n", \
			__FILE__, __LINE__, #condition, __func__); \
		abort(); \
	} \
} while(0)

#define ASSERT_ERROR(condition, error) do { \
	if (!(condition)) { \
		fprintf(stderr, \
			ANSI_COLOR_BOLD ANSI_COLOR_FUCHSIA \
			"%s:%d: %s" ANSI_COLOR_RESET "\n" \
			"  " ANSI_COLOR_BOLD ANSI_COLOR_WHITE \
			"condition:" ANSI_COLOR_RESET " (%s)\n" \
			"  " ANSI_COLOR_BOLD ANSI_COLOR_WHITE \
			"function:" ANSI_COLOR_RESET " %s()\n", \
			__FILE__, __LINE__, (error), #condition, __func__); \
		abort(); \
	} \
} while(0)

#else
	#define ASSERT(condition) ((void)0)
	#define ASSERT_ERROR(condition, error) ((void)0)
#endif

#endif /* DEFINITIONS_ASSERT_H */