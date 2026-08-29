#include "obj.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"

#include <stdio.h>

#define _F "%.6f"
#define _FF float

struct FoldFrameObject {
	usize vertex_offset;
	usize texture_offset;
	usize normal_offset;
	usize frame_index;
};

struct TinyObjObject {
	tinyobj_shape_t* shape;
	tinyobj_attrib_t* attributes;
	tinyobj_material_t* materials;
	usize num_materials;
	usize shape_offset;
};

static inline
void _tiny_obj_file_reader(void* ctx, const char* filename, int is_mtl,
	const char* obj_filename, char** buf, usize* len)
{
	(void)ctx; (void)obj_filename; (void)is_mtl;
	FILE* file = fopen(filename, "rb");
	TRY_NEW_MEMORY_OR_ERROR(file,
		*buf = NULL; *len = 0;
		return);

	TRY_OR_ERROR(fseek(file, 0, SEEK_END),
		*buf = NULL; *len = 0;
		fclose(file);
		return);

	long file_length = ftell(file);
	if (file_length < 0) {
		*buf = NULL; *len = 0;
		fclose(file);
		return;
	}

	*len = (usize)file_length;
	TRY_OR_ERROR(fseek(file, 0, SEEK_SET),
		*buf = NULL; *len = 0;
		fclose(file);
		return);

	TRY_ADD_OR_ERROR(*len, 1,
		*buf = NULL; *len = 0;
		fclose(file);
		return);

	*buf = (char*)malloc(*len + 1);
	TRY_NEW_MEMORY_OR_ERROR(*buf,
		fclose(file); *len = 0;
		return);

	usize bytes = fread(*buf, 1, *len, file);
	if (bytes < *len) { free(*buf);
		*buf = NULL; *len = 0;
		fclose(file);
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
	(void)OBJ;
	struct TinyObjObject* object = Object;
	tinyobj_shape_t* shape = object->shape;
	tinyobj_attrib_t* attributes = object->attributes;
	tinyobj_material_t* materials = object->materials;
	usize num_materials = object->num_materials;
	usize shape_offset = object->shape_offset;

	usize EV_size = 0, FV_size = 0;
	FOR_EACH(fi, shape->length) {
		usize face_index = (usize)shape->face_offset + fi;
		usize face_vertices = attributes->face_num_verts[face_index];
		if (face_vertices == 2) { EV_size++; }
		else { FV_size++; }
	}

	TRY(array_resize(&graph->EV, EV_size));
	if (num_materials != 0) {
		TRY(array_resize(&graph->EA, EV_size)); }

	TRY(array_resize(&graph->FV.offsets, FV_size));
	if (attributes->num_texcoords != 0) {
		TRY(array_resize(&graph->FT.offsets, FV_size)); }
	if (attributes->num_normals != 0) {
		TRY(array_resize(&graph->FN.offsets, FV_size)); }
	if (num_materials != 0) {
		TRY(array_resize(&graph->FM, FV_size)); }

	usize fv_offset = 0;
	FOR_EACH(fi, shape->length) {
		usize face_index = (usize)shape->face_offset + fi;
		usize face_vertices = attributes->face_num_verts[face_index];
		if (face_vertices == 2) continue;
		TRY_ADD(fv_offset, face_vertices);
		fv_offset += face_vertices;

		array_set(&graph->FV.offsets, fi, &fv_offset);
		if (attributes->num_texcoords != 0) {
			array_set(&graph->FT.offsets, fi, &fv_offset); }
		if (attributes->num_normals != 0) {
			array_set(&graph->FN.offsets, fi, &fv_offset); }
	}

	TRY(array_resize(&graph->FV.data, fv_offset));
	if (attributes->num_texcoords != 0) {
		TRY(array_resize(&graph->FT.data, fv_offset)); }
	if (attributes->num_normals != 0) {
		TRY(array_resize(&graph->FN.data, fv_offset)); }

	fv_offset = 0;
	usize offset = 0;
	usize ev_index = 0, fv_index = 0;
	FOR_EACH(fi, shape->length) {
		usize face_index = (usize)shape->face_offset + fi;
		usize face_vertices = attributes->face_num_verts[face_index];

		if (face_vertices == 2) {
			tinyobj_vertex_index_t line_vertex0 =
				attributes->faces[shape_offset + offset];
			tinyobj_vertex_index_t line_vertex1 =
				attributes->faces[shape_offset + offset + 1];

			FoldGraphEdge edge;
			edge.a = (usize)line_vertex0.v_idx;
			edge.b = (usize)line_vertex1.v_idx;
			array_set(&graph->EV, ev_index, &edge);

			if (num_materials != 0) {
				usize material_index = attributes->material_ids[face_index];
				tinyobj_material_t material = materials[material_index];
				FoldGraphEdgeAssignment a = (material.name != NULL)
					? (FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(material.name[0])
						? (FoldGraphEdgeAssignment)material.name[0]
						: FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN)
					: FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;
				array_set(&graph->EA, ev_index, &a);
			}

			ev_index++;
		} else {
			FOR_EACH(vi, face_vertices) {
				tinyobj_vertex_index_t face_vertex =
					attributes->faces[shape_offset + offset + vi];

				usize fvi = (usize)face_vertex.v_idx;
				array_set(&graph->FV.data, fv_offset + vi, &fvi);

				if (attributes->num_texcoords != 0) {
					usize fvt = (face_vertex.vt_idx >= 0)
						? (usize)face_vertex.vt_idx : FOLD_GRAPH_NULL;
					array_set(&graph->FT.data, fv_offset + vi, &fvt);
				}

				if (attributes->num_normals != 0) {
					usize fvn = (face_vertex.vn_idx >= 0)
						? (usize)face_vertex.vn_idx : FOLD_GRAPH_NULL;
					array_set(&graph->FN.data, fv_offset + vi, &fvn);
				}
			}

			if (num_materials != 0) {
				usize material_index = attributes->material_ids[face_index];
				tinyobj_material_t material = materials[material_index];
				material_index = (material.name != NULL)
					? material_index : FOLD_GRAPH_NULL;
				array_set(&graph->FM, fv_index, &material_index);
			}

			fv_offset += face_vertices;
			fv_index++;
		}
		offset += face_vertices;
	}

	object->shape_offset += offset;
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Deserialization                                                */
/* ========================================================================= */

static inline
Error fold_frame_from_obj(FoldFrame* frame, void* OBJ, void* Object) {
	(void)OBJ;
	struct TinyObjObject* object = Object;
	TRY(string_copy_raw(&frame->title, object->shape->name));
	TRY(fold_graph_from_obj(&frame->graph, NULL, object));
	return OK;
}

/* ========================================================================= */
/* FOLD File Deserialization                                                 */
/* ========================================================================= */

Error fold_file_from_obj(FoldFile* file, void* OBJ) {
	const char* input_file_path = OBJ;

	tinyobj_attrib_t attributes;
	tinyobj_shape_t* shapes = NULL;
	tinyobj_material_t* materials = NULL;
	usize num_shapes = 0, num_materials = 0;
	unsigned int flags = NOT(TINYOBJ_FLAG_TRIANGULATE);

	int result = tinyobj_parse_obj(&attributes,
		&shapes, &num_shapes, &materials, &num_materials,
		input_file_path, _tiny_obj_file_reader, NULL, flags);
	if (result != TINYOBJ_SUCCESS) {
		return ERROR;
	}

	TRY_ADD(num_shapes, 1);
	TRY(fold_file_recreate(file));
	TRY(array_resize(&file->frames, num_shapes + 1));
	FoldFrame* key_frame = array_get_start(&file->frames);

	TRY(array_resize(&key_frame->graph.VC, attributes.num_vertices));
	FOR_EACH(index, attributes.num_vertices) {
		Vector3 vc;
		vc.x = attributes.vertices[3 * index + 0];
		vc.y = attributes.vertices[3 * index + 1];
		vc.z = attributes.vertices[3 * index + 2];
		array_set(&key_frame->graph.VC, index, &vc);
	}

	TRY(array_resize(&key_frame->graph.TC, attributes.num_texcoords));
	FOR_EACH(index, attributes.num_texcoords) {
		Vector2 vt;
		vt.x = attributes.texcoords[2 * index + 0];
		vt.y = attributes.texcoords[2 * index + 1];
		array_set(&key_frame->graph.TC, index, &vt);
	}

	TRY(array_resize(&key_frame->graph.NC, attributes.num_normals));
	FOR_EACH(index, attributes.num_normals) {
		Vector3 vn;
		vn.x = attributes.normals[3 * index + 0];
		vn.y = attributes.normals[3 * index + 1];
		vn.z = attributes.normals[3 * index + 2];
		array_set(&key_frame->graph.NC, index, &vn);
	}

	TRY(array_resize(&key_frame->graph.MN, num_materials));
	FOR_EACH(index, num_materials) {
		const char* string = (materials[index].name != NULL)
			? materials[index].name : "";
		STRING_BUFFER_RAW(string, buffer,
			key_frame->graph.MN.element_size);
		array_set(&key_frame->graph.MN, index, buffer);
	}

	struct TinyObjObject object;
	object.shape = NULL;
	object.attributes = &attributes;
	object.materials = materials;
	object.num_materials = num_materials;
	object.shape_offset = 0;

	ARRAY_FOR_EACH_IN_RANGE(&file->frames,
		shape, FoldFrame*, frame, 1, num_shapes + 1)
	{
		object.shape = &shapes[shape - 1];

		fold_frame_create(frame);
		frame->inherit = true;
		frame->parent = 0;

		TRY(fold_frame_from_obj(frame, NULL, &object));
		fold_file_frame_inherit(file, shape);
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
	FILE* output_file = OBJ;
	struct FoldFrameObject* object = Object;

	usize abstract_VC_size = 0;
	if (fold_graph_is_abstract(graph)) {
		ARRAY_FOR_EACH(&graph->EV, _, FoldGraphEdge*, ev) {
			abstract_VC_size = MAX(abstract_VC_size, TRY_SAFE(ev->a + 1));
			abstract_VC_size = MAX(abstract_VC_size, TRY_SAFE(ev->b + 1));
		}
		ARRAY2_FOR_EACH(&graph->FV, __, _, usize*, fvi) {
			abstract_VC_size = MAX(abstract_VC_size, TRY_SAFE(*fvi + 1));
		}
		FOR_EACH(_, abstract_VC_size) {
			fprintf(output_file, "v 0.0 0.0 0.0\n");
		}
	} else if (fold_graph_is_2D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
			if (graph->VP.size == 0) {
				fprintf(output_file, "v "_F" "_F" 0.0\n",
					(_FF)vc->x, (_FF)vc->y);
			} else {
				usize* vpi = array_get(&graph->VP, vi);
				Color* vpc = array_get(&graph->PC, *vpi);
				fprintf(output_file, "v "_F" "_F" 0.0 "_F" "_F" "_F"\n",
					(_FF)vc->x, (_FF)vc->y,
					(_FF)(vpc->r * vpc->a),
					(_FF)(vpc->g * vpc->a),
					(_FF)(vpc->b * vpc->a));
			}
		}
	} else if (fold_graph_is_3D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
			if (graph->VP.size == 0) {
				fprintf(output_file, "v "_F" "_F" "_F"\n",
					(_FF)vc->x, (_FF)vc->y, (_FF)vc->z);
			} else {
				usize* vpi = array_get(&graph->VP, vi);
				Color* vpc = array_get(&graph->PC, *vpi);
				fprintf(output_file, "v "_F" "_F" "_F" "_F" "_F" "_F"\n",
					(_FF)vc->x, (_FF)vc->y, (_FF)vc->z,
					(_FF)(vpc->r * vpc->a),
					(_FF)(vpc->g * vpc->a),
					(_FF)(vpc->b * vpc->a));
			}
		}
	}

	ARRAY_FOR_EACH(&graph->TC, _, Vector2*, vt) {
		fprintf(output_file, "vt "_F" "_F"\n",
			(_FF)vt->x, (_FF)vt->y);
	}

	ARRAY_FOR_EACH(&graph->NC, _, Vector3*, vn) {
		fprintf(output_file, "vn "_F" "_F" "_F"\n",
			(_FF)vn->x, (_FF)vn->y, (_FF)vn->z);
	}

	if (graph->FM.size == 0) {
		fprintf(output_file, "usemtl\n");
	}

	ARRAY2_ITERATE(&graph->FV, fv) {
		if (graph->FM.size != 0) {
			usize* fmi = array_get(&graph->FM, fv.index);
			if (*fmi != FOLD_GRAPH_NULL) {
				char* material = array_get(&graph->MN, *fmi);
				fprintf(output_file, "usemtl %.*s\n",
					(int)graph->MN.element_size, material);
			} else { fprintf(output_file, "usemtl\n"); }
		}

		fprintf(output_file, "f");
		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, fvi, fv.start, fv.end)
		{
			fprintf(output_file, " %zu",
				TRY_SAFE(object->vertex_offset + *fvi));

			fprintf(output_file, "/");
			if (graph->FT.size != 0) {
				usize* fti = array_get(&graph->FT.data, i);
				if (*fti != FOLD_GRAPH_NULL) {
					fprintf(output_file, "%zu",
						TRY_SAFE(object->texture_offset + *fti));
				}
			}

			fprintf(output_file, "/");
			if (graph->FN.size != 0) {
				usize* fni = array_get(&graph->FN.data, i);
				if (*fni != FOLD_GRAPH_NULL) {
					fprintf(output_file, "%zu",
						TRY_SAFE(object->normal_offset + *fni));
				}
			}
		}
		fprintf(output_file, "\n");
	}

	if (graph->EA.size == 0 &&
		graph->FM.size != 0) {
		fprintf(output_file, "usemtl\n");
	}

	ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
		if (graph->EA.size != 0) {
			char* material = array_get(&graph->EA, ei);
			fprintf(output_file, "usemtl %.*s\n",
				(int)graph->EA.element_size, material);
		}
		fprintf(output_file, "l %zu %zu\n",
			TRY_SAFE(object->vertex_offset + ev->a),
			TRY_SAFE(object->vertex_offset + ev->b));
	}

	TRY_SAFE(object->vertex_offset += abstract_VC_size);
	TRY_SAFE(object->vertex_offset += graph->VC.size);
	TRY_SAFE(object->texture_offset += graph->TC.size);
	TRY_SAFE(object->normal_offset += graph->NC.size);
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Serialization                                                  */
/* ========================================================================= */

static inline
Error fold_frame_to_obj(const FoldFrame* frame, void* OBJ, void* Object) {
	FILE* output_file = OBJ;
	struct FoldFrameObject* object = Object;
	fprintf(output_file, "o frame_%zu\n", object->frame_index);
	TRY(fold_graph_to_obj(&frame->graph, output_file, object));
	object->frame_index++;
	return OK;
}

/* ========================================================================= */
/* FOLD File Serialization                                                   */
/* ========================================================================= */

Error fold_file_to_obj(FoldFile* file, void* OBJ) {
	FILE* output_file = (OBJ != NULL)
		? fopen((const char*)OBJ, "w")
		: stdout;

	TRY_NEW_MEMORY_OR_ERROR(output_file,
		return ERROR);

	struct FoldFrameObject object;
	object.vertex_offset = 1;
	object.texture_offset = 1;
	object.normal_offset = 1;
	object.frame_index = 0;

	fold_file_frames_inherit(file);
	ARRAY_FOR_EACH(&file->frames, _, FoldFrame*, frame) {
		TRY_OR_ELSE(fold_frame_to_obj(frame, output_file, &object),
			fclose(output_file));
	}

	TRY_OR_ERROR(ferror(output_file),
		fclose(output_file);
		return ERROR);

	return IS_ERROR(fclose(output_file))
		? ERROR : OK;
}