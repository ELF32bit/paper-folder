#include "obj.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "math/vector2.h"
#include "math/vector3.h"

#include <stdio.h>

struct TinyObjObject {
	usize* shape_offset;
	tinyobj_shape_t* shape;
	tinyobj_attrib_t* attributes;
	tinyobj_material_t* materials;
	usize num_materials;
};

static inline
void _tiny_obj_file_reader(void* ctx, const char* filename, int is_mtl,
	const char* obj_filename, char** buf, usize* len) {
	(void)ctx; (void)obj_filename; (void)is_mtl;
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		*buf = NULL;
		*len = 0;
		return;
	}
	if (fseek(file, 0, SEEK_END) != 0) {
		fclose(file);
		*buf = NULL;
		*len = 0;
		return;
	}
	long file_length = ftell(file);
	if (file_length < 0) {
		fclose(file);
		*buf = NULL;
		*len = 0;
		return;
	}
	*len = (usize)file_length;
	if (fseek(file, 0, SEEK_SET) != 0) {
		fclose(file);
		*buf = NULL;
		*len = 0;
		return;
	}
	*buf = (char*)malloc(*len + 1);
	if (*buf == NULL) {
		fclose(file);
		*len = 0;
		return;
	}
	usize bytes = fread(*buf, 1, *len, file);
	if (bytes < *len) {
		free(*buf);
		fclose(file);
		*buf = NULL;
		*len = 0;
		return;
	}
	(*buf)[*len] = '\0';
	fclose(file);
}

/* ========================================================================= */
/* FOLD Graph Deserialization                                                */
/* ========================================================================= */

static inline
Error fold_graph_from_obj(FoldGraph* graph, void* OBJ, void* Object) {
	struct TinyObjObject* object = Object; (void)OBJ;
	usize* shape_offset = object->shape_offset;
	tinyobj_shape_t* shape = object->shape;
	tinyobj_attrib_t* attributes = object->attributes;
	tinyobj_material_t* materials = object->materials;
	usize num_materials = object->num_materials;

	usize EV_size = 0, FV_size = 0;
	FOR_EACH(fi, shape->length) {
		usize face_index = shape->face_offset + fi;
		usize face_vertices = attributes->face_num_verts[face_index];
		if (face_vertices == 2) { EV_size++; }
		else { FV_size++; }
	}

	TRY(array_resize(&graph->EV, EV_size));
	TRY(array_resize(&graph->FV.offsets, FV_size));

	usize fv_offset = 0;
	FOR_EACH(fi, shape->length) {
		usize face_index = shape->face_offset + fi;
		usize face_vertices = attributes->face_num_verts[face_index];
		if (face_vertices == 2) continue;

		TRY_ADD(fv_offset, face_vertices);
		fv_offset += face_vertices;
		array_set(&graph->FV.offsets, fi, &fv_offset);
	}

	TRY(array_resize(&graph->FV.data, fv_offset));

	fv_offset = 0;
	usize offset = 0;
	usize ev_offset = 0;
	FOR_EACH(fi, shape->length) {
		usize face_index = shape->face_offset + fi;
		usize face_vertices = attributes->face_num_verts[face_index];

		if (face_vertices == 2) {
			tinyobj_vertex_index_t l0 =
				attributes->faces[*shape_offset + offset];
			tinyobj_vertex_index_t l1 =
				attributes->faces[*shape_offset + offset + 1];

			FoldGraphEdge edge;
			edge.a = (usize)l0.v_idx; edge.b = (usize)l1.v_idx;
			array_set(&graph->EV, ev_offset, &edge);
			ev_offset++;
		} else {
			FOR_EACH(vi, face_vertices) {
				tinyobj_vertex_index_t vertex =
					attributes->faces[*shape_offset + offset + vi];

				usize vertex_index = (usize)vertex.v_idx;
				array_set(&graph->FV.data, fv_offset + vi, &vertex_index);
			}
			fv_offset += face_vertices;
		}
		offset += face_vertices;
	}

	*shape_offset += offset;
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Deserialization                                                */
/* ========================================================================= */

static inline
Error fold_frame_from_obj(FoldFrame* frame, void* OBJ, void* Object) {
	struct TinyObjObject* object = Object; (void)OBJ;
	TRY(string_copy_raw(&frame->title, object->shape->name));
	TRY(fold_graph_from_obj(&frame->graph, NULL, object));
	return OK;
}

/* ========================================================================= */
/* FOLD File Deserialization                                                 */
/* ========================================================================= */

Error fold_file_from_obj(FoldFile* file, void* OBJ) {
	const char* file_name = OBJ;

	tinyobj_attrib_t attributes;
	tinyobj_shape_t* shapes = NULL;
	tinyobj_material_t* materials = NULL;
	usize num_shapes = 0, num_materials = 0;
	unsigned int flags = 0;

	int result = tinyobj_parse_obj(&attributes,
		&shapes, &num_shapes, &materials, &num_materials,
		file_name, _tiny_obj_file_reader, NULL, flags);
	if (result != TINYOBJ_SUCCESS) {
		return ERROR;
	}

	TRY_ADD(num_shapes, 1);
	TRY(fold_file_recreate(file));
	TRY(array_resize(&file->frames, num_shapes + 1));
	FoldFrame* key_frame = array_get(&file->frames, 0);

	TRY(array_resize(&key_frame->graph.VC, attributes.num_vertices));
	FOR_EACH(vi, attributes.num_vertices) {
		Vector3 vc;
		vc.x = attributes.vertices[3 * vi + 0];
		vc.y = attributes.vertices[3 * vi + 1];
		vc.z = attributes.vertices[3 * vi + 2];
		array_set(&key_frame->graph.VC, vi, &vc);
	}

	usize shape_offset = 0;
	ARRAY_FOR_EACH_IN_RANGE(&file->frames, shape,
		FoldFrame*, frame, 1, num_shapes + 1) {
		struct TinyObjObject object;
		object.shape_offset = &shape_offset;
		object.shape = &shapes[shape - 1];
		object.attributes = &attributes;
		object.materials = materials;
		object.num_materials = num_materials;

		fold_frame_create(frame);
		TRY(fold_frame_from_obj(frame, NULL, &object));
	}

	tinyobj_attrib_free(&attributes);
	tinyobj_shapes_free(shapes, num_shapes);
	tinyobj_materials_free(materials, num_materials);
	return OK;
}

/* ========================================================================= */
/* FOLD Graph Serialization                                                  */
/* ========================================================================= */

static inline
Error fold_graph_to_obj(const FoldGraph* graph, void* OBJ, void* Object) {
	FILE* output_file = OBJ; (void)(Object);

	if (fold_graph_is_abstract(graph)) {
		usize max_vi = 0;
		ARRAY_FOR_EACH(&graph->EV, _, FoldGraphEdge*, ev) {
			max_vi = MAX(max_vi, ev->a + 1);
			max_vi = MAX(max_vi, ev->b + 1);
		}
		ARRAY2_FOR_EACH(&graph->FV, __, _, usize*, fvi) {
			max_vi = MAX(max_vi, *fvi + 1);
		}
		FOR_EACH(_, max_vi) {
			fprintf(output_file, "v 0.0 0.0 0.0\n");
		}
	} else if (fold_graph_is_2D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, _, Vector2*, vc) {
			fprintf(output_file, "v %g %g 0.0\n",
				(double)vc->x, (double)vc->y);
		}
	} else if (fold_graph_is_3D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, _, Vector3*, vc) {
			fprintf(output_file, "v %g %g %g\n",
				(double)vc->x, (double)vc->y, (double)vc->z);
		}
	}

	if (graph->extensions.FM.size == 0) {
		fprintf(output_file, "usemtl\n");
	}

	ARRAY2_ITERATE(&graph->FV, fv) {
		if (graph->extensions.FM.size != 0) {
			char* material = array_get(&graph->extensions.FM, fv.index);
			fprintf(output_file, "usemtl %.*s\n",
				(int)graph->extensions.FM.element_size,
				material);
		}

		fprintf(output_file, "f");
		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data, _,
			usize*, fvi, fv.start, fv.end) {
			fprintf(output_file, " %zu", *fvi + 1);
		}
		fprintf(output_file, "\n");
	}

	if (graph->EA.size == 0 &&
		graph->extensions.FM.size != 0) {
		fprintf(output_file, "usemtl\n");
	}

	ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
		if (graph->EA.size != 0) {
			char* material = array_get(&graph->EA, ei);
			fprintf(output_file, "usemtl %.*s\n",
				(int)graph->EA.element_size,
				material);
		}
		fprintf(output_file, "l %zu %zu\n",
			ev->a + 1, ev->b + 1);
	}

	return OK;
}

/* ========================================================================= */
/* FOLD Frame Serialization                                                  */
/* ========================================================================= */

static inline
Error fold_frame_to_obj(const FoldFrame* frame, void* OBJ, void* Object) {
	FILE* output_file = OBJ;
	fprintf(output_file, "o frame_%zu\n", *(usize*)Object);
	TRY(fold_graph_to_obj(&frame->graph, output_file, Object));
	return OK;
}

/* ========================================================================= */
/* FOLD File Serialization                                                   */
/* ========================================================================= */

Error fold_file_to_obj(const FoldFile* file, void* OBJ) {
	FILE* output_file = fopen((const char*)OBJ, "w");
	if (output_file == NULL) return ERROR;

	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		TRY_OR_ELSE(fold_frame_to_obj(frame, output_file, &index),
			fclose(output_file));
	}

	if (ferror(output_file) != 0) {
		fclose(output_file);
		return ERROR;
	}

	return (fclose(output_file) != 0)
		? ERROR
		: OK;
}