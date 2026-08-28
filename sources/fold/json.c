#include "file.h"
#include "frame.h"
#include "graph.h"

#include <yyjson.h>

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"

#include <string.h>
#include <ctype.h>

#define NS_ FOLD_EXTENSIONS_NAMESPACE

/* ========================================================================= */
/* FOLD Graph Deserialization                                                */
/* ========================================================================= */

static inline
Error _parse_json_array_coords(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;
	usize dimensions = 2;

	usize ii, __, i, _;
	yyjson_val *inner_array, *element;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		if NOT(yyjson_is_arr(inner_array)) return ERROR;
		usize inner_array_size = yyjson_arr_size(inner_array);
		if (dimensions == 2 && inner_array_size > 2) dimensions = 3;
		yyjson_arr_foreach(inner_array, i, _, element) {
			if NOT(yyjson_is_num(element)) return ERROR;
		}
	}

	array_destroy(array);
	array_create(array, (dimensions == 2)
		? sizeof(Vector2) : sizeof(Vector3));
	TRY(array_resize(array, size));

	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		Vector3 vector;
		vector.components[0] = 0.0;
		vector.components[1] = 0.0;
		vector.components[2] = 0.0;

		yyjson_arr_foreach(inner_array, i, _, element) {
			if (i >= dimensions) break;
			vector.components[i] = (real)yyjson_get_num(element);
		}

		array_set(array, ii, vector.components);
	}

	return OK;
}

static inline
Error _parse_json_array2_indices(yyjson_val* object, Array2* array2, const char* name, bool has_null) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize offsets_size = yyjson_arr_size(json_array);
	if (offsets_size == 0) return OK;
	usize data_size = 0;

	usize ii, __, i, _;
	yyjson_val *inner_array, *element;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		if NOT(yyjson_is_arr(inner_array)) return ERROR;
		usize inner_array_size = yyjson_arr_size(inner_array);
		yyjson_arr_foreach(inner_array, i, _, element) {
			if (has_null && yyjson_is_null(element)) continue;
			if NOT(yyjson_is_uint(element)) return ERROR;
			if (yyjson_get_uint(element) > USIZE_MAX - has_null) {
				return ERROR_OUT_OF_MEMORY;
			}
		}
		TRY_ADD(data_size, inner_array_size);
		data_size += inner_array_size;
	}

	array2_recreate(array2);
	TRY(array_resize(&array2->data, data_size));
	TRY_OR_ELSE(array_resize(&array2->offsets, offsets_size),
		array2_recreate(array2));

	usize data_index = 0;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		usize inner_array_size = yyjson_arr_size(inner_array);
		yyjson_arr_foreach(inner_array, i, _, element) {
			if (has_null && yyjson_is_null(element)) {
				ARRAY_SET(&array2->data, data_index,
					usize, FOLD_GRAPH_NULL);
			} else {
				ARRAY_SET(&array2->data, data_index,
					usize, yyjson_get_uint(element));
			}
			data_index++;
		}

		ARRAY_SET(&array2->offsets, ii,
			usize, array2_start_offset_at(array2, ii) +
				inner_array_size);
	}

	return OK;
}

static inline
Error _parse_json_array_edges(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize ii, __, i, _;
	yyjson_val *inner_array, *element;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		if NOT(yyjson_is_arr(inner_array)) return ERROR;
		usize inner_array_size = yyjson_arr_size(inner_array);
		if (inner_array_size != 2) return ERROR;
		yyjson_arr_foreach(inner_array, i, _, element) {
			if NOT(yyjson_is_uint(element)) return ERROR;
			if (yyjson_get_uint(element) > USIZE_MAX) {
				return ERROR_OUT_OF_MEMORY;
			}
		}
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		FoldGraphEdge edge;
		yyjson_arr_foreach(inner_array, i, _, element) {
			edge.components[i] = (usize)yyjson_get_uint(element);
		}
		array_set(array, ii, &edge);
	}

	return OK;
}

static inline
Error _parse_json_array_chars(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize i, _;
	yyjson_val *element;
	yyjson_arr_foreach(json_array, i, _, element) {
		if NOT(yyjson_is_str(element)) return ERROR;
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, i, _, element) {
		if (array->element_size == sizeof(char)) {
			char s0 = FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;
			if (yyjson_get_len(element) != 0) {
				const char* string = yyjson_get_str(element);
				s0 = toupper((unsigned char)string[0]);
			}
			array_set(array, i, &s0);
		} else {
			const char* string = yyjson_get_str(element);
			usize length = strlen(string);

			char word[array->element_size];
			usize word_length = (length < (array->element_size - 1))
				? length : (array->element_size - 1);

			memcpy(word, string, word_length);
			memset(word + word_length, 0,
				array->element_size - word_length);

			array_set(array, i, word);
		}
	}

	return OK;
}

static inline
Error _parse_json_array_floats(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize i, _;
	yyjson_val *element;
	yyjson_arr_foreach(json_array, i, _, element) {
		if NOT(yyjson_is_num(element)) return ERROR;
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, i, _, element) {
		ARRAY_SET(array, i, real, yyjson_get_num(element));
	}

	return OK;
}

static inline
Error _parse_json_array_orders(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize ii, __, i, _;
	yyjson_val *inner_array, *element;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		if NOT(yyjson_is_arr(inner_array)) return ERROR;
		usize inner_array_size = yyjson_arr_size(inner_array);
		if (inner_array_size != 3) return ERROR;
		yyjson_arr_foreach(inner_array, i, _, element) {
			if (i != 2) {
				if NOT(yyjson_is_uint(element)) return ERROR;
				if (yyjson_get_uint(element) > USIZE_MAX) {
					return ERROR_OUT_OF_MEMORY;
				}
			} else if (i == 2) {
				if NOT(yyjson_is_int(element)) return ERROR;
				i64 number = yyjson_get_sint(element);
				if NOT(number == -1 || number == 0 || number == 1) {
					return ERROR;
				}
			}
		}
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		FoldGraphOrder order;
		yyjson_arr_foreach(inner_array, i, _, element) {
			if (i != 2) {
				order.components[i] = (usize)yyjson_get_uint(element);
			} else if (i == 2) {
				i64 number = yyjson_get_sint(element);
				if (number == 1) {
					order.o = FOLD_GRAPH_ORDER_POSITIVE;
				} else if (number == -1) {
					order.o = FOLD_GRAPH_ORDER_NEGATIVE;
				} else {
					order.o = FOLD_GRAPH_ORDER_UNKNOWN;
				}
			}
		}
		array_set(array, ii, &order);
	}

	return OK;
}

static inline
Error _parse_json_array_colors(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize i, _;
	yyjson_val *element;
	yyjson_arr_foreach(json_array, i, _, element) {
		if NOT(yyjson_is_uint(element)) return ERROR;
		if (yyjson_get_uint(element) > USIZE_MAX) {
			return ERROR_OUT_OF_MEMORY;
		}
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, i, _, element) {
		u32 number = (u32)yyjson_get_uint(element);
		Color color = color_unpack(number);
		array_set(array, i, &color);
	}

	return OK;
}

static inline
Error _parse_json_array_coords2(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize ii, __, i, _;
	yyjson_val *inner_array, *element;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		if NOT(yyjson_is_arr(inner_array)) return ERROR;
		usize inner_array_size = yyjson_arr_size(inner_array);
		if (inner_array_size != 2) return ERROR;
		yyjson_arr_foreach(inner_array, i, _, element) {
			if NOT(yyjson_is_num(element)) return ERROR;
		}
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		Vector2 vector;
		yyjson_arr_foreach(inner_array, i, _, element) {
			vector.components[i] = (real)yyjson_get_num(element);
		}
		array_set(array, ii, &vector);
	}

	return OK;
}

static inline
Error _parse_json_array_coords3(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize ii, __, i, _;
	yyjson_val *inner_array, *element;
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		if NOT(yyjson_is_arr(inner_array)) return ERROR;
		usize inner_array_size = yyjson_arr_size(inner_array);
		if (inner_array_size != 3) return ERROR;
		yyjson_arr_foreach(inner_array, i, _, element) {
			if NOT(yyjson_is_num(element)) return ERROR;
		}
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, ii, __, inner_array) {
		Vector3 vector;
		yyjson_arr_foreach(inner_array, i, _, element) {
			vector.components[i] = (real)yyjson_get_num(element);
		}
		array_set(array, ii, &vector);
	}

	return OK;
}

static inline
Error _parse_json_array_indices(yyjson_val* object, Array* array, const char* name, bool has_null) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize i, _;
	yyjson_val *element;
	yyjson_arr_foreach(json_array, i, _, element) {
		if (has_null && yyjson_is_null(element)) continue;
		if NOT(yyjson_is_uint(element)) return ERROR;
		if (yyjson_get_uint(element) > USIZE_MAX - has_null) {
			return ERROR_OUT_OF_MEMORY;
		}
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, i, _, element) {
		if (has_null && yyjson_is_null(element)) {
			ARRAY_SET(array, i, usize, FOLD_GRAPH_NULL);
		} else {
			ARRAY_SET(array, i, usize, yyjson_get_uint(element));
		}
	}

	return OK;
}

static
Error fold_graph_extensions_from_json(FoldGraphExtensions* extensions, void* JSON, void* Object) {
	(void)JSON; yyjson_val* object = Object;
	TRY(_parse_json_array_colors(object, &extensions->PC, NS_"paints_color"));
	TRY(_parse_json_array_coords2(object, &extensions->TC, NS_"uvs_coords"));
	TRY(_parse_json_array_coords2(object, &extensions->T2C, NS_"uv2s_coords"));
	TRY(_parse_json_array_coords3(object, &extensions->NC, NS_"normals_coords"));
	TRY(_parse_json_array_chars(object, &extensions->MN, NS_"materials_name"));
	TRY(_parse_json_array_indices(object, &extensions->VP, "vertices_"NS_"paint", false));
	TRY(_parse_json_array2_indices(object, &extensions->FT, "faces_"NS_"uvs", true));
	TRY(_parse_json_array2_indices(object, &extensions->FT2, "faces_"NS_"uv2s", true));
	TRY(_parse_json_array2_indices(object, &extensions->FN, "faces_"NS_"normals", true));
	TRY(_parse_json_array_indices(object, &extensions->FM, "faces_"NS_"material", true));
	return OK;
}

Error fold_graph_from_json(FoldGraph* graph, void* JSON, void* Object) {
	(void)JSON; yyjson_val* object = Object;
	fold_graph_recreate(graph);
	TRY(_parse_json_array_coords(object, &graph->VC, "vertices_coords"));
	TRY(_parse_json_array2_indices(object, &graph->VV, "vertices_vertices", false));
	TRY(_parse_json_array2_indices(object, &graph->VE, "vertices_edges", false));
	TRY(_parse_json_array2_indices(object, &graph->VF, "vertices_faces", true));
	TRY(_parse_json_array_edges(object, &graph->EV, "edges_vertices"));
	TRY(_parse_json_array2_indices(object, &graph->EF, "edges_faces", true));
	TRY(_parse_json_array_chars(object, &graph->EA, "edges_assignment"));
	TRY(_parse_json_array_floats(object, &graph->EFA, "edges_foldAngle"));
	TRY(_parse_json_array_floats(object, &graph->EL, "edges_length"));
	TRY(_parse_json_array_orders(object, &graph->EO, "edgeOrders"));
	TRY(_parse_json_array2_indices(object, &graph->FV, "faces_vertices", false));
	TRY(_parse_json_array2_indices(object, &graph->FE, "faces_edges", false));
	TRY(_parse_json_array2_indices(object, &graph->FF, "faces_faces", true));
	TRY(_parse_json_array_orders(object, &graph->FO, "faceOrders"));

if (FOLD_COMPATIBILITY_EFA_EL && graph->EFA.size == 0) {
	TRY(_parse_json_array_floats(object, &graph->EFA, "edges_foldAngles")); }
if (FOLD_COMPATIBILITY_EFA_EL && graph->EL.size == 0) {
	TRY(_parse_json_array_floats(object, &graph->EL, "edges_lengths")); }

	TRY(fold_graph_extensions_from_json(&graph->extensions, JSON, object));
	return OK;
}

/* ========================================================================= */
/* FOLD Graph Serialization                                                  */
/* ========================================================================= */

static inline
Error _encode_json_array_coords(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	usize dimensions = (array->element_size
		== sizeof(Vector2)) ? 2 : 3;

	ARRAY_FOR_EACH(array, __, real*, vector) {
		double components[3];
		components[0] = (double)vector[0];
		components[1] = (double)vector[1];
		components[2] = (dimensions == 3)
			? (double)vector[2] : 0.0;

		yyjson_mut_val* inner_array =
			yyjson_mut_arr_with_double(json, components, dimensions);
		TRY_NEW_MEMORY(inner_array);

		TRY(!yyjson_mut_arr_add_val(json_array, inner_array));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array2_indices(yyjson_mut_doc* json, yyjson_mut_val* object, const Array2* array2, const char* name, bool has_null) {
	if (array2->size == 0 || array2->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY2_ITERATE(array2, array) {
		yyjson_mut_val* inner_array;
		if (has_null) {
			inner_array = yyjson_mut_arr(json);
			TRY_NEW_MEMORY(inner_array);
			ARRAY_FOR_EACH_IN_RANGE(&array2->data,
				_, usize*, number, array.start, array.end) {
				if (*number == FOLD_GRAPH_NULL) {
					TRY(!yyjson_mut_arr_add_null(json, inner_array));
				} else {
					TRY(!yyjson_mut_arr_add_uint(json, inner_array,
						(u64)*number));
				}
			}
		} else {
			ARRAY_CREATE(numbers, u64);
			TRY(array_resize(&numbers, array.size));
			ARRAY_FOR_EACH_IN_RANGE(&array2->data,
				i, usize*, number, array.start, array.end) {
				ARRAY_SET(&numbers, i - array.start, u64, *number);
			}

			inner_array =
				yyjson_mut_arr_with_uint64(json, numbers.data, array.size);
			array_destroy(&numbers);
			TRY_NEW_MEMORY(inner_array);
		}

		TRY(!yyjson_mut_arr_add_val(json_array, inner_array));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_edges(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, __, FoldGraphEdge*, edge) {
		u64 components[2] = {
			(u64)edge->components[0],
			(u64)edge->components[1]};

		yyjson_mut_val* inner_array =
			yyjson_mut_arr_with_uint64(json, components, 2);
		TRY_NEW_MEMORY(inner_array);

		TRY(!yyjson_mut_arr_add_val(json_array, inner_array));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_chars(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, _, char*, string) {
		if (array->element_size == sizeof(char)) {
			TRY(!yyjson_mut_arr_add_strn(json, json_array, string, sizeof(char)));
		} else {
			char* end = memchr(string, '\0', array->element_size);
			usize length = end ? (usize)(end - string) : array->element_size;
			TRY(!yyjson_mut_arr_add_strn(json, json_array, string, length));
		}
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_floats(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;

	ARRAY_CREATE(numbers, double);
	TRY(array_resize(&numbers, array->size));
	ARRAY_FOR_EACH(array, i, real*, number) {
		ARRAY_SET(&numbers, i, double, *number);
	}

	yyjson_mut_val* json_array =
		yyjson_mut_arr_with_double(json, numbers.data, array->size);
	array_destroy(&numbers);
	TRY_NEW_MEMORY(json_array);

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_orders(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, __, FoldGraphOrder*, order) {
		yyjson_mut_val* inner_array = yyjson_mut_arr(json);
		TRY_NEW_MEMORY(inner_array);

		TRY(!yyjson_mut_arr_add_uint(json, inner_array, (u64)order->a));
		TRY(!yyjson_mut_arr_add_uint(json, inner_array, (u64)order->b));

		if (order->o == FOLD_GRAPH_ORDER_POSITIVE) {
			TRY(!yyjson_mut_arr_add_int(json, inner_array, 1));
		} else if (order->o == FOLD_GRAPH_ORDER_NEGATIVE) {
			TRY(!yyjson_mut_arr_add_int(json, inner_array, -1));
		} else {
			TRY(!yyjson_mut_arr_add_int(json, inner_array, 0));
		}

		TRY(!yyjson_mut_arr_add_val(json_array, inner_array));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_colors(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, _, Color*, color) {
		u64 code = (u64)color_pack(*color);
		TRY(!yyjson_mut_arr_add_uint(json, json_array, code));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_coords2(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, __, Vector2*, vector) {
		double components[2] = {
			(double)vector->components[0],
			(double)vector->components[1]};

		yyjson_mut_val* inner_array =
			yyjson_mut_arr_with_double(json, components, 2);
		TRY_NEW_MEMORY(inner_array);

		TRY(!yyjson_mut_arr_add_val(json_array, inner_array));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_coords3(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, __, Vector3*, vector) {
		double components[3] = {
			(double)vector->components[0],
			(double)vector->components[1],
			(double)vector->components[2]};

		yyjson_mut_val* inner_array =
			yyjson_mut_arr_with_double(json, components, 3);
		TRY_NEW_MEMORY(inner_array);

		TRY(!yyjson_mut_arr_add_val(json_array, inner_array));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_indices(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name, bool has_null) {
	if (array->size == 0 || array->is_view) return OK;
	if (has_null) {
		yyjson_mut_val* json_array;
		json_array = yyjson_mut_arr(json);
		TRY_NEW_MEMORY(json_array);

		ARRAY_FOR_EACH(array, i, usize*, number) {
			if (*number == FOLD_GRAPH_NULL) {
				TRY(!yyjson_mut_arr_add_null(json, json_array));
			} else {
				TRY(!yyjson_mut_arr_add_uint(json, json_array,
					(u64)*number));
			}
		}

		TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	} else {
		ARRAY_CREATE(numbers, u64);
		TRY(array_resize(&numbers, array->size));
		ARRAY_FOR_EACH(array, i, usize*, number) {
			ARRAY_SET(&numbers, i, u64, *number);
		}

		yyjson_mut_val* json_array =
			yyjson_mut_arr_with_uint64(json, numbers.data, array->size);
		array_destroy(&numbers);
		TRY_NEW_MEMORY(json_array);

		TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	}
	return OK;
}

static
Error fold_graph_extensions_to_json(const FoldGraphExtensions* extensions, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_mut_val* object = Object;
	TRY(_encode_json_array_colors(json, object, &extensions->PC, NS_"paints_color"));
	TRY(_encode_json_array_coords2(json, object, &extensions->TC, NS_"uvs_coords"));
	TRY(_encode_json_array_coords2(json, object, &extensions->T2C, NS_"uv2s_coords"));
	TRY(_encode_json_array_coords3(json, object, &extensions->NC, NS_"normals_coords"));
	TRY(_encode_json_array_chars(json, object, &extensions->MN, NS_"materials_name"));
	TRY(_encode_json_array_indices(json, object, &extensions->VP, "vertices_"NS_"paint", false));
	TRY(_encode_json_array2_indices(json, object, &extensions->FT, "faces_"NS_"uvs", true));
	TRY(_encode_json_array2_indices(json, object, &extensions->FT2, "faces_"NS_"uv2s", true));
	TRY(_encode_json_array2_indices(json, object, &extensions->FN, "faces_"NS_"normals", true));
	TRY(_encode_json_array_indices(json, object, &extensions->FM, "faces_"NS_"material", true));
	return OK;
}

Error fold_graph_to_json(const FoldGraph* graph, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_mut_val* object = Object;
	TRY(_encode_json_array_coords(json, object, &graph->VC, "vertices_coords"));
	TRY(_encode_json_array2_indices(json, object, &graph->VV, "vertices_vertices", false));
	TRY(_encode_json_array2_indices(json, object, &graph->VE, "vertices_edges", false));
	TRY(_encode_json_array2_indices(json, object, &graph->VF, "vertices_faces", true));
	TRY(_encode_json_array_edges(json, object, &graph->EV, "edges_vertices"));
	TRY(_encode_json_array2_indices(json, object, &graph->EF, "edges_faces", true));
	TRY(_encode_json_array_chars(json, object, &graph->EA, "edges_assignment"));
	TRY(_encode_json_array_floats(json, object, &graph->EFA, "edges_foldAngle"));
	TRY(_encode_json_array_floats(json, object, &graph->EL, "edges_length"));
	TRY(_encode_json_array_orders(json, object, &graph->EO, "edgeOrders"));
	TRY(_encode_json_array2_indices(json, object, &graph->FV, "faces_vertices", false));
	TRY(_encode_json_array2_indices(json, object, &graph->FE, "faces_edges", false));
	TRY(_encode_json_array2_indices(json, object, &graph->FF, "faces_faces", true));
	TRY(_encode_json_array_orders(json, object, &graph->FO, "faceOrders"));
	TRY(fold_graph_extensions_to_json(&graph->extensions, json, object));
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Deserialization                                                */
/* ========================================================================= */

static inline
Error _parse_json_string(yyjson_val* object, String* string, const char* name) {
	yyjson_val* json_string = yyjson_obj_get(object, name);
	if (json_string == NULL) return OK;
	if NOT(yyjson_is_str(json_string)) return ERROR;
	string_recreate(string);
	TRY(string_copy_raw(string, yyjson_get_str(json_string)));
	return OK;
}

static inline
Error _parse_json_array_strings(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize i, _;
	yyjson_val *element;
	yyjson_arr_foreach(json_array, i, _, element) {
		if NOT(yyjson_is_str(element)) return ERROR;
	}

	array_recreate(array);
	TRY(array_resize(array, size));
	yyjson_arr_foreach(json_array, i, _, element) {
		String* string = array_get(array, i);
		string_create(string);
		TRY_OR_ELSE(string_copy_raw(string, yyjson_get_str(element)),
			array->size = i);
	}

	return OK;
}

static inline
Error _parse_json_index(yyjson_val* object, usize* value, const char* name) {
	yyjson_val* json_int = yyjson_obj_get(object, name);
	if (json_int == NULL) return OK;
	if NOT(yyjson_is_uint(json_int)) return ERROR;
	if (yyjson_get_uint(json_int) > USIZE_MAX) return ERROR_OUT_OF_MEMORY;
	*value = (usize)yyjson_get_uint(json_int);
	return OK;
}

static inline
Error _parse_json_bool(yyjson_val* object, bool* value, const char* name) {
	yyjson_val* json_bool = yyjson_obj_get(object, name);
	if (json_bool == NULL) return OK;
	if NOT(yyjson_is_bool(json_bool)) return ERROR;
	*value = yyjson_get_bool(json_bool);
	return OK;
}

static
Error fold_frame_metadata_from_json(FoldFrameMetadata* metadata, void* JSON, void* Object) {
	(void)JSON; yyjson_val* object = Object;
	TRY(_parse_json_bool(object, &metadata->is_simulated, "frame_"NS_"simulated"));
	return OK;
}

Error fold_frame_from_json(FoldFrame* frame, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_val* object = Object;
	fold_frame_recreate(frame);
	TRY(_parse_json_string(object, &frame->author, "frame_author"));
	TRY(_parse_json_string(object, &frame->title, "frame_title"));
	TRY(_parse_json_string(object, &frame->description, "frame_description"));
	TRY(_parse_json_array_strings(object, &frame->classes, "frame_classes"));
	TRY(_parse_json_array_strings(object, &frame->attributes, "frame_attributes"));
	TRY(_parse_json_string(object, &frame->unit, "frame_unit"));
	TRY(fold_graph_from_json(&frame->graph, json, object));
	TRY(_parse_json_index(object, &frame->parent, "frame_parent"));
	TRY(_parse_json_bool(object, &frame->inherit, "frame_inherit"));
	TRY(fold_frame_metadata_from_json(&frame->metadata, json, object));
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Serialization                                                  */
/* ========================================================================= */

static inline
Error _encode_json_string(yyjson_mut_doc* json, yyjson_mut_val* object, const String* string, const char* name) {
	if (string->length == 0 || string->is_view) return OK;
	TRY(!yyjson_mut_obj_add_str(json, object, name, string->data));
	return OK;
}

static inline
Error _encode_json_array_strings(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, _, String*, string) {
		TRY(!yyjson_mut_arr_add_str(json, json_array, string->data));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_index(yyjson_mut_doc* json, yyjson_mut_val* object, const usize* value, const char* name) {
	if (*value == FOLD_FRAME_PARENT_NONE) return OK;
	TRY(!yyjson_mut_obj_add_uint(json, object, name, (u64)*value));
	return OK;
}

static inline
Error _encode_json_bool(yyjson_mut_doc* json, yyjson_mut_val* object, const bool* value, const char* name) {
	if (*value == false) return OK;
	TRY(!yyjson_mut_obj_add_bool(json, object, name, *value));
	return OK;
}

static
Error fold_frame_metadata_to_json(const FoldFrameMetadata* metadata, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_mut_val* object = Object;
	TRY(_encode_json_bool(json, object, &metadata->is_simulated, "frame"NS_"simulated"));
	return OK;
}

Error fold_frame_to_json(const FoldFrame* frame, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_mut_val* object = Object;
	TRY(_encode_json_string(json, object, &frame->author, "frame_author"));
	TRY(_encode_json_string(json, object, &frame->title, "frame_title"));
	TRY(_encode_json_string(json, object, &frame->description, "frame_description"));
	TRY(_encode_json_array_strings(json, object, &frame->classes, "frame_classes"));
	TRY(_encode_json_array_strings(json, object, &frame->attributes, "frame_attributes"));
	TRY(_encode_json_string(json, object, &frame->unit, "frame_unit"));
	TRY(fold_graph_to_json(&frame->graph, json, object));
	TRY(_encode_json_index(json, object, &frame->parent, "frame_parent"));
	TRY(_encode_json_bool(json, object, &frame->inherit, "frame_inherit"));
	TRY(fold_frame_metadata_to_json(&frame->metadata, json, object));
	return OK;
}

/* ========================================================================= */
/* FOLD File Deserialization                                                 */
/* ========================================================================= */

static inline
Error _parse_json_float(yyjson_val* object, double* value, const char* name) {
	yyjson_val* json_number = yyjson_obj_get(object, name);
	if (json_number == NULL) return OK;
	if NOT(yyjson_is_num(json_number)) return ERROR;
	*value = yyjson_get_num(json_number);
	return OK;
}

static inline
Error _parse_json_frames(yyjson_val* object, Array* array, const char* name) {
	yyjson_val* json_array = yyjson_obj_get(object, name);
	if (json_array == NULL) return OK;
	if NOT(yyjson_is_arr(json_array)) return ERROR;
	usize size = yyjson_arr_size(json_array);
	if (size == 0) return OK;

	usize i, _;
	yyjson_val *inner_object;
	yyjson_arr_foreach(json_array, i, _, inner_object) {
		if NOT(yyjson_is_obj(inner_object)) return ERROR;
	}

	TRY_ADD(size, 1);
	TRY(array_resize(array, size + 1));
	yyjson_arr_foreach(json_array, i, _, inner_object) {
		FoldFrame* frame = array_get(array, i + 1);
		fold_frame_create(frame);
		TRY_OR_ELSE(fold_frame_from_json(frame, NULL, inner_object),
			array->size = i + 1);
	}

	return OK;
}

Error fold_file_from_json(FoldFile* file, void* JSON, void* Object) {
	yyjson_doc* json = JSON; yyjson_val* object = Object;
	TRY(fold_file_recreate(file));
	TRY(_parse_json_float(object, &file->version, "file_version"));
	TRY(_parse_json_string(object, &file->creator, "file_creator"));
	TRY(_parse_json_string(object, &file->author, "file_author"));
	TRY(_parse_json_string(object, &file->title, "file_title"));
	TRY(_parse_json_string(object, &file->description, "file_description"));
	TRY(_parse_json_array_strings(object, &file->classes, "file_classes"));
	TRY(fold_frame_from_json(array_get(&file->frames, 0), json, object));
	TRY(_parse_json_frames(object, &file->frames, "file_frames"));
	return OK;
}

/* ========================================================================= */
/* FOLD File Serialization                                                   */
/* ========================================================================= */

static inline
Error _encode_json_float(yyjson_mut_doc* json, yyjson_mut_val* object, const double* value, const char* name) {
	TRY(!yyjson_mut_obj_add_real(json, object, name, (double)*value));
	return OK;
}

static inline
Error _encode_json_frames(yyjson_mut_doc* json, yyjson_mut_val* object, const Array* array, const char* name) {
	if (array->size <= 1) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH_IN_RANGE(array,
		_, FoldFrame*, frame, 1, array->size) {
		yyjson_mut_val* inner_object = yyjson_mut_obj(json);
		TRY_NEW_MEMORY(inner_object);

		TRY(fold_frame_to_json(frame, json, inner_object));

		TRY(!yyjson_mut_arr_add_val(json_array, inner_object));
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

Error fold_file_to_json(const FoldFile* file, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_mut_val* object = Object;
	TRY(_encode_json_float(json, object, &file->version, "file_version"));
	TRY(_encode_json_string(json, object, &file->creator, "file_creator"));
	TRY(_encode_json_string(json, object, &file->author, "file_author"));
	TRY(_encode_json_string(json, object, &file->title, "file_title"));
	TRY(_encode_json_string(json, object, &file->description, "file_description"));
	TRY(_encode_json_array_strings(json, object, &file->classes, "file_classes"));
	TRY(fold_frame_to_json(array_get(&file->frames, 0), json, object));
	TRY(_encode_json_frames(json, object, &file->frames, "file_frames"));
	return OK;
}