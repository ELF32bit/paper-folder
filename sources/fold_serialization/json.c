#include "json.h"

#include <yyjson.h>

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"

#define GE_ ""
#define ME_ FOLD_EXTENSIONS_NAMESPACE

/* ========================================================================= */
/* FOLD Graph                                                                */
/* ========================================================================= */

static inline
Error _encode_json_array_coords(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
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
Error _encode_json_array2_indices(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array2* array2, const char* name, bool has_null)
{
	if (array2->size == 0 || array2->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY2_ITERATE(array2, array) {
		yyjson_mut_val* inner_array;
		if (has_null) {
			inner_array = yyjson_mut_arr(json);
			TRY_NEW_MEMORY(inner_array);
			ARRAY_FOR_EACH_IN_RANGE(&array2->data,
				_, usize*, number, array.start, array.end)
			{
				if (*number == FOLD_GRAPH_NULL) {
					TRY(!yyjson_mut_arr_add_null(json, inner_array));
				} else {
					TRY(!yyjson_mut_arr_add_uint(json, inner_array,
						(u64)*number));
				}
			}
		} else {
			ARRAY_CREATE(numbers, u64);
			TRY_OR_ELSE(array_resize(&numbers, array.size),
				array_destroy(&numbers));

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
Error _encode_json_array_edges(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
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
Error _encode_json_array_strings(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
	if (array->size == 0 || array->is_view) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH(array, _, char*, buffer) {
		if (array->element_size == sizeof(char)) {
			TRY(!yyjson_mut_arr_add_strn(json, json_array, buffer,
				sizeof(char)));
		} else {
			TRY(!yyjson_mut_arr_add_strn(json, json_array, buffer,
				string_buffer_length(buffer, array->element_size)));
		}
	}

	TRY(!yyjson_mut_obj_add_val(json, object, name, json_array));
	return OK;
}

static inline
Error _encode_json_array_floats(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
	if (array->size == 0 || array->is_view) return OK;

	ARRAY_CREATE(numbers, double);
	TRY_OR_ELSE(array_resize(&numbers, array->size),
		array_destroy(&numbers));

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
Error _encode_json_array_orders(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
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
Error _encode_json_array_colors(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
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
Error _encode_json_array_coords2(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
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
Error _encode_json_array_coords3(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
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
Error _encode_json_array_indices(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name, bool has_null)
{
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
		TRY_OR_ELSE(array_resize(&numbers, array->size),
			array_destroy(&numbers));

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
	TRY(_encode_json_array_colors(json, object, &extensions->PC, GE_"paints_color"));
	TRY(_encode_json_array_coords2(json, object, &extensions->TC, GE_"uvs_coords"));
	TRY(_encode_json_array_coords2(json, object, &extensions->T2C, GE_"uv2s_coords"));
	TRY(_encode_json_array_coords3(json, object, &extensions->NC, GE_"normals_coords"));
	TRY(_encode_json_array_strings(json, object, &extensions->MN, GE_"materials_name"));
	TRY(_encode_json_array_indices(json, object, &extensions->VP, "vertices_"GE_"paint", false));
	TRY(_encode_json_array2_indices(json, object, &extensions->FT, "faces_"GE_"uvs", true));
	TRY(_encode_json_array2_indices(json, object, &extensions->FT2, "faces_"GE_"uv2s", true));
	TRY(_encode_json_array2_indices(json, object, &extensions->FN, "faces_"GE_"normals", true));
	TRY(_encode_json_array_indices(json, object, &extensions->FM, "faces_"GE_"material", true));
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
	TRY(_encode_json_array_strings(json, object, &graph->EA, "edges_assignment"));
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
/* FOLD Frame                                                                */
/* ========================================================================= */

static inline
Error _encode_json_string(yyjson_mut_doc* json, yyjson_mut_val* object,
	const String* string, const char* name)
{
	if (string->length == 0 || string->is_view) return OK;
	TRY(!yyjson_mut_obj_add_str(json, object, name, string->data));
	return OK;
}

static inline
Error _encode_json_index(yyjson_mut_doc* json, yyjson_mut_val* object,
	const usize* value, const char* name)
{
	if (*value == FOLD_FRAME_PARENT_NONE) return OK;
	TRY(!yyjson_mut_obj_add_uint(json, object, name, (u64)*value));
	return OK;
}

static inline
Error _encode_json_bool(yyjson_mut_doc* json, yyjson_mut_val* object,
	const bool* value, const char* name)
{
	if (*value == false) return OK;
	TRY(!yyjson_mut_obj_add_bool(json, object, name, *value));
	return OK;
}

static
Error fold_frame_metadata_to_json(const FoldFrameMetadata* metadata, void* JSON, void* Object) {
	yyjson_mut_doc* json = JSON; yyjson_mut_val* object = Object;
	TRY(_encode_json_bool(json, object, &metadata->is_simulated, "frame"ME_"simulated"));
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
/* FOLD File                                                                 */
/* ========================================================================= */

static inline
Error _encode_json_float(yyjson_mut_doc* json, yyjson_mut_val* object,
	const double* value, const char* name)
{
	TRY(!yyjson_mut_obj_add_real(json, object, name, (double)*value));
	return OK;
}

static inline
Error _encode_json_frames(yyjson_mut_doc* json, yyjson_mut_val* object,
	const Array* array, const char* name)
{
	if (array->size <= 1) return OK;
	yyjson_mut_val* json_array = yyjson_mut_arr(json);
	TRY_NEW_MEMORY(json_array);

	ARRAY_FOR_EACH_IN_RANGE(array,
		_, FoldFrame*, frame, 1, array->size)
	{
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