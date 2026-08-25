#include "string.h"

#include <stdlib.h>
#include <string.h>

#include "definitions/cpu.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void string_create(String* string) {
	string->data = NULL;
	string->length = 0;
	string->is_view = false;
}

void string_destroy(String* string) {
	if (NOT(string->is_view)) free(string->data);
	string_create(string);
}

void string_recreate(String* string) {
	string_destroy(string);
}

void string_view(String* string, const String* source) {
	if (string == source) return;
	string_destroy(string);
	string->data = source->data;
	string->length = source->length;
	string->is_view = true;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error string_copy(String* string, const String* source) {
	ASSERT(NOT(string->is_view));
	if (string == source) return OK;
	if (source->length == 0 ||
		source->data == NULL) {
		string_recreate(string);
		return OK;
	}

	if (string->data != NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			string->data, string->data + string->length,
			source->data, source->data + source->length,
			ASSERT_ERROR(false, "string memory overlaps source"));
	}

	TRY_ADD(source->length, 1);
	TRY_SIZEOF(char, source->length + 1);
	char* new_data = realloc(string->data,
		SIZEOF(char, source->length + 1));
	TRY_NEW_MEMORY(new_data);

	memcpy(new_data, source->data,
		SIZEOF(char, source->length));
	new_data[source->length] = '\0';

	string->data = new_data;
	string->length = source->length;
	return OK;
}

Error string_copy_raw(String* string, const char* source) {
	ASSERT(NOT(string->is_view));
	if (source == NULL) {
		string_recreate(string);
		return OK;
	}

	usize new_length = strlen(source);
	if (new_length == 0) {
		string_recreate(string);
		return OK;
	}

	if (string->data != NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			string->data, string->data + string->length,
			source, source + new_length,
			ASSERT_ERROR(false, "string memory overlaps source"));
	}

	TRY_ADD(new_length, 1);
	TRY_SIZEOF(char, new_length + 1);
	char* new_data = realloc(string->data,
		SIZEOF(char, new_length + 1));
	TRY_NEW_MEMORY(new_data);

	memcpy(new_data, source,
		SIZEOF(char, new_length));
	new_data[new_length] = '\0';

	string->data = new_data;
	string->length = new_length;
	return OK;
}

Error string_append(String* string, const String* another) {
	ASSERT(NOT(string->is_view));
	if (another->length == 0 ||
		another->data == NULL) return OK;

	if (string != another && string->data != NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			string->data, string->data + string->length,
			another->data, another->data + another->length,
			ASSERT_ERROR(false, "string memory overlaps another"));
	}

	TRY_ADD(string->length, another->length);
	usize new_length = string->length + another->length;

	TRY_ADD(new_length, 1);
	TRY_SIZEOF(char, new_length + 1);
	if (string != another) {
		char* new_data = realloc(string->data,
			SIZEOF(char, new_length + 1));
		TRY_NEW_MEMORY(new_data);

		memcpy(new_data + string->length, another->data,
			SIZEOF(char, another->length));
		new_data[new_length] = '\0';

		string->data = new_data;
	} else {
		char* new_data = malloc(SIZEOF(char, new_length + 1));
		TRY_NEW_MEMORY(new_data);

		memcpy(new_data, string->data, SIZEOF(char, string->length));
		memcpy(new_data + another->length, another->data,
			SIZEOF(char, another->length));
		new_data[new_length] = '\0';

		free(string->data);
		string->data = new_data;
	}

	string->length = new_length;
	return OK;
}

Error string_append_raw(String* string, const char* another) {
	ASSERT(NOT(string->is_view));
	if (another == NULL) return OK;

	usize another_length = strlen(another);
	if (another_length == 0) return OK;

	if (string->data != NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			string->data, string->data + string->length,
			another, another + another_length,
			ASSERT_ERROR(false, "string memory overlaps another"));
	}

	TRY_ADD(string->length, another_length);
	usize new_length = string->length + another_length;

	TRY_ADD(new_length, 1);
	TRY_SIZEOF(char, new_length + 1);
	char* new_data = realloc(string->data,
		SIZEOF(char, new_length + 1));
	TRY_NEW_MEMORY(new_data);

	memcpy(new_data + string->length, another,
		SIZEOF(char, another_length));
	new_data[new_length] = '\0';

	string->data = new_data;
	string->length = new_length;
	return OK;
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	#define FNV_PRIME 0x100000001B3ULL
	#define FNV_OFFSET_BASIS 0xCBF29CE484222325ULL
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	#define FNV_PRIME 0x01000193U
	#define FNV_OFFSET_BASIS 0x811C9DC5U
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	#define FNV_PRIME 0x0193U
	#define FNV_OFFSET_BASIS 0x811CU
#else
	#define FNV_PRIME 0xA3U
	#define FNV_OFFSET_BASIS 0x47U
#endif

usize hash_string(const String* string) {
	if (string->length == 0 ||
		string->data == NULL) return 0;
	const u8* data = (const u8*)string->data;
	usize data_size = SIZEOF(char, string->length);

	usize h = FNV_OFFSET_BASIS;
	FOR_EACH(i, data_size) {
		h ^= data[i];
		h *= FNV_PRIME;
	}

	return h;
}

usize hash_string_raw(const char* string) {
	if (string == NULL) return 0;
	const u8* data = (const u8*)string;
	usize data_size = SIZEOF(char, strlen(string));

	usize h = FNV_OFFSET_BASIS;
	FOR_EACH(i, data_size) {
		h ^= data[i];
		h *= FNV_PRIME;
	}

	return h;
}

bool string_equals(const String* string, const String* another) {
	if (string == another) return true;
	if (string->length != another->length) return false;
	if (string->length == 0) return true;

	if (string->data == NULL || another->data == NULL) return false;
	return memcmp(string->data, another->data,
		SIZEOF(char, string->length)) == 0;
}

bool string_equals_raw(const String* string, const char* another) {
	if (another == NULL) return (string->length == 0);
	if (string->data == NULL && string->length > 0) return false;
	if (string->length == 0) return (another[0] == 0);

	usize another_length = strlen(another);
	if (string->length != another_length) return false;
	return memcmp(string->data, another,
		SIZEOF(char, string->length)) == 0;
}

/* ========================================================================= */
/* Sorting                                                                   */
/* ========================================================================= */

int string_compare(const String* string, const String* another) {
	if (string == another) return 0;

	usize min_length = MIN(string->length, another->length);
	if (min_length > 0) {
		int result = memcmp(string->data, another->data,
			SIZEOF(char, min_length));
		if (result != 0) return result;
	}

	if (string->length < another->length) return -1;
	if (string->length > another->length) return 1;
	return 0;
}

bool string_greater(const String* string, const String* another) {
	return string_compare(string, another) > 0;
}

bool string_lesser(const String* string, const String* another) {
	return string_compare(string, another) < 0;
}