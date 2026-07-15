#ifndef COLLECTIONS_STRING_H
#define COLLECTIONS_STRING_H

#include "definitions/types.h"

typedef struct String {
	char* data;
	usize length;
	bool is_view;
} String;

void string_create(String* string);

#define String_destroy string_destroy
void string_destroy(String* string);
void string_recreate(String* string);

#define STRING_CREATE(string) \
	String string; \
	string_create(&string)

void string_view(String* string, const String* source);

#define String_copy string_copy
Error string_copy(String* string, const String* source);
Error string_copy_raw(String* string, const char* source);

Error string_append(String* string, const String* another);
Error string_append_raw(String* string, const char* another);

/* ========================================================================= */
/* Hash Section                                                              */
/* ========================================================================= */

usize hash_string(const String* string);
usize hash_string_raw(const char* string);

#define String_hash string_hash
static inline usize string_hash(const void* a) {
	return (usize)hash_string((const String*)a);
}

#define String_equals string_equals
bool string_equals(const String* string, const String* another);
bool string_equals_raw(const String* string, const char* another);

/* ========================================================================= */
/* Sort Section                                                              */
/* ========================================================================= */

int string_compare(const String* string, const String* another);
bool string_greater(const String* string, const String* another);
bool string_lesser(const String* string, const String* another);

#define String_sort_ascending string_sort_ascending
static inline int string_sort_ascending(const void* a, const void* b) {
	return string_compare((const String*)a, (const String*)b);
}

#define String_sort_descending string_sort_descending
static inline int string_sort_descending(const void* a, const void* b) {
	return string_compare((const String*)b, (const String*)a);
}

#endif /* COLLECTIONS_STRING_H */