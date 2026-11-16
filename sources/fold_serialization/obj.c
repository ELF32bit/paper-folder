#include "obj.h"

#include "fold_graph/attributes.h"
#include "fold_graph/properties.h"

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "collections/map.h"
#include "definitions/random.h"

#include <stdio.h>

#define _F "%.6f"
#define _FF float
#define _W "%.9g"
#define _WW float

struct FoldFrameObjectContext {
	const struct FoldFileToOBJParameters* parameters;
	bool VC_inherit, TC_inherit, NC_inherit;
	usize VC_offset, TC_offset, NC_offset;
	usize index;
	RNG* rng;
};

static inline
void _get_abstract_coordinates(const FoldGraph* graph, f32* x, f32* y, f32* z,
	const struct FoldFrameObjectContext* object)
{
	*x = 0.0; *y = 0.0; *z = 0.0;
	if (object->parameters->abstract_seed != 0) {
		f32 size = object->parameters->abstract_size;
		*x = random_f32_in_range(object->rng, 0.0, size);
		*y = random_f32_in_range(object->rng, 0.0, size);
		if (fold_graph_is_3D(graph)) {
			*z = random_f32_in_range(object->rng, 0.0, size);
		}
	}
}

/* ========================================================================= */
/* FOLD Graph                                                                */
/* ========================================================================= */

static inline
Error fold_graph_to_obj(const FoldGraph* graph, void* OBJ,
	const struct FoldFrameObjectContext* object)
{
	FILE* output_file = OBJ;
	f32 x, y, z;

	if NOT(object->VC_inherit) {
		if (fold_graph_is_abstract(graph)) {
			if (object->parameters->disable_colors &&
				NOT(object->parameters->enable_weights))
			{
				FOR_EACH(_, fold_graph_get_abstract_size(graph)) {
					_get_abstract_coordinates(graph, &x, &y, &z, object);
					fprintf(output_file, "v "_F" "_F" "_F"\n",
						(_FF)x, (_FF)y, (_FF)z);
				}
			} else if (object->parameters->enable_weights) {
				FOR_EACH(_, fold_graph_get_abstract_size(graph)) {
					_get_abstract_coordinates(graph, &x, &y, &z, object);
					fprintf(output_file, "v "_F" "_F" "_F" 1.0\n",
						(_FF)x, (_FF)y, (_FF)z);
				}
			} else {
				FOR_EACH(_, fold_graph_get_abstract_size(graph)) {
					_get_abstract_coordinates(graph, &x, &y, &z, object);
					fprintf(output_file, "v "_F" "_F" "_F" 1.0 1.0 1.0\n",
						(_FF)x, (_FF)y, (_FF)z);
				}
			}
		} else if (fold_graph_is_2D(graph)) {
			if ((graph->VP.size == 0 ||
				object->parameters->disable_colors) &&
				NOT(object->parameters->enable_weights))
			{
				ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
					fprintf(output_file, "v "_F" "_F" 0.0\n",
						(_FF)vc->x, (_FF)vc->y);
				}
			} else if (graph->VP.size == 0 &&
				object->parameters->enable_weights)
			{
				ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
					fprintf(output_file, "v "_F" "_F" 0.0 1.0\n",
						(_FF)vc->x, (_FF)vc->y);
				}
			} else if (graph->VP.size != 0 &&
				object->parameters->enable_weights)
			{
				ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
					usize* vpi = array_get(&graph->VP, vi);
					Color* vpc = array_get(&graph->PC, *vpi);
					Color rgb = color_premultiply_alpha(*vpc);

					f32 w = F32(
						(U32(color_get_r8(rgb)) << 16) |
						(U32(color_get_g8(rgb)) << 8) |
						U32(color_get_b8(rgb))) /
						F32(0xFFFFFF);

					fprintf(output_file,
						"v "_F" "_F" 0.0 "_W"\n",
						(_FF)vc->x, (_FF)vc->y,
						(_WW)w);
				}
			} else {
				ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
					usize* vpi = array_get(&graph->VP, vi);
					Color* vpc = array_get(&graph->PC, *vpi);
					Color rgb = color_premultiply_alpha(*vpc);
					fprintf(output_file,
						"v "_F" "_F" 0.0 "_W" "_W" "_W"\n",
						(_FF)vc->x, (_FF)vc->y,
						(_WW)rgb.r, (_WW)rgb.g, (_WW)rgb.b);
				}
			}
		} else if (fold_graph_is_3D(graph)) {
			if ((graph->VP.size == 0 ||
				object->parameters->disable_colors) &&
				NOT(object->parameters->enable_weights))
			{
				ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
					fprintf(output_file, "v "_F" "_F" "_F"\n",
						(_FF)vc->x, (_FF)vc->y, (_FF)vc->z);
				}
			} else if (graph->VP.size == 0 &&
				object->parameters->enable_weights)
			{
				ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
					fprintf(output_file, "v "_F" "_F" "_F" 1.0\n",
						(_FF)vc->x, (_FF)vc->y, (_FF)vc->z);
				}
			} else if (graph->VP.size != 0 &&
				object->parameters->enable_weights)
			{
				ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
					usize* vpi = array_get(&graph->VP, vi);
					Color* vpc = array_get(&graph->PC, *vpi);
					Color rgb = color_premultiply_alpha(*vpc);

					f32 w = F32(
						(U32(color_get_r8(rgb)) << 16) |
						(U32(color_get_g8(rgb)) << 8) |
						U32(color_get_b8(rgb))) /
						F32(0xFFFFFF);

					fprintf(output_file,
						"v "_F" "_F" "_F" "_W"\n",
						(_FF)vc->x, (_FF)vc->y, (_FF)vc->z,
						(_WW)w);
				}
			} else {
				ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
					usize* vpi = array_get(&graph->VP, vi);
					Color* vpc = array_get(&graph->PC, *vpi);
					Color rgb = color_premultiply_alpha(*vpc);
					fprintf(output_file,
						"v "_F" "_F" "_F" "_W" "_W" "_W"\n",
						(_FF)vc->x, (_FF)vc->y, (_FF)vc->z,
						(_WW)rgb.r, (_WW)rgb.g, (_WW)rgb.b);
				}
			}
		}
	}

	if NOT(object->parameters->disable_textures) {
		if NOT(object->TC_inherit) {
			ARRAY_FOR_EACH(&graph->TC, _, Vector2*, vt) {
				fprintf(output_file,
					"vt "_F" "_F"\n",
					(_FF)vt->x, (_FF)vt->y);
			}
		}
	}

	if NOT(object->parameters->disable_normals) {
		if NOT(object->NC_inherit) {
			ARRAY_FOR_EACH(&graph->NC, _, Vector3*, vn) {
				fprintf(output_file,
					"vn "_F" "_F" "_F"\n",
					(_FF)vn->x, (_FF)vn->y, (_FF)vn->z);
			}
		}
	}

	if NOT(object->parameters->disable_lines) {
		if NOT(object->parameters->disable_materials) {
			if (graph->EA.size == 0) {
				fprintf(output_file, "usemtl\n");
			}
		}

		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			if NOT(object->parameters->disable_materials) {
				if (graph->EA.size != 0) {
					char* material = array_get(&graph->EA, ei);
					fprintf(output_file, "usemtl %.*s\n",
						(int)graph->EA.element_size, material);
				}
			}
			TRY_ADD(object->VC_offset, ev->a);
			TRY_ADD(object->VC_offset, ev->b);
			fprintf(output_file, "l %zu %zu\n",
				TRY_SAFE(object->VC_offset + ev->a),
				TRY_SAFE(object->VC_offset + ev->b));
		}
	}

	if NOT(object->parameters->disable_faces) {
		if NOT(object->parameters->disable_materials) {
			if (graph->FM.size == 0) {
				if (object->parameters->disable_lines) {
					fprintf(output_file, "usemtl\n");
				} else if (graph->EA.size != 0) {
					fprintf(output_file, "usemtl\n");
				}
			}
		}

		ARRAY2_ITERATE(&graph->FV, fv) {
			if NOT(object->parameters->disable_materials) {
				if (graph->FM.size != 0) {
					usize* fmi = array_get(&graph->FM, fv.index);
					if (*fmi != FOLD_GRAPH_NULL) {
						char* material = array_get(&graph->MN, *fmi);
						fprintf(output_file, "usemtl %.*s\n",
							(int)graph->MN.element_size, material);
					} else {
						fprintf(output_file, "usemtl\n");
					}
				}
			}

			fprintf(output_file, "f");
			ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
				i, usize*, fvi, fv.start, fv.end)
			{
				TRY_ADD(object->VC_offset, *fvi);
				fprintf(output_file, " %zu",
					TRY_SAFE(object->VC_offset + *fvi));

				fprintf(output_file, "/");
				if NOT(object->parameters->disable_textures) {
					if (graph->FT.size != 0) {
						usize* fti = array_get(&graph->FT.data, i);
						if (*fti != FOLD_GRAPH_NULL) {
							TRY_ADD(object->TC_offset, *fti);
							fprintf(output_file, "%zu",
								TRY_SAFE(object->TC_offset + *fti));
						}
					}
				}

				fprintf(output_file, "/");
				if NOT(object->parameters->disable_normals) {
					if (graph->FN.size != 0) {
						usize* fni = array_get(&graph->FN.data, i);
						if (*fni != FOLD_GRAPH_NULL) {
							TRY_ADD(object->NC_offset, *fni);
							fprintf(output_file, "%zu",
								TRY_SAFE(object->NC_offset + *fni));
						}
					}
				}
			}
			fprintf(output_file, "\n");
		}
	}

	return OK;
}

/* ========================================================================= */
/* FOLD Frame                                                                */
/* ========================================================================= */

static inline
Error fold_frame_to_obj(const FoldFrame* frame, void* OBJ,
	const struct FoldFrameObjectContext* object)
{
	FILE* output_file = OBJ;
	if (object->parameters->enable_groups) {
		fprintf(output_file, "g frame_%zu\n", object->index);
	} else fprintf(output_file, "o frame_%zu\n", object->index);
	TRY(fold_graph_to_obj(&frame->graph, output_file, object));
	return OK;
}

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

Error fold_file_to_obj(FoldFile* file, void* NULLABLE(OBJ),
	const struct FoldFileToOBJParameters* parameters)
{
	FILE* output_file = (OBJ != NULL)
		? fopen((const char*)OBJ, "w")
		: stdout;

	TRY_NEW_MEMORY_OR_ERROR(output_file,
		return ERROR);

	usize VC_offset = 1;
	usize VC_parents_size = 0;
	ARRAY_CREATE(VC_offsets, usize);
	MAP_CREATE(VC_parents, usize, usize);
	VC_parents.hash = usize_hash_identity;

	usize TC_offset = 1;
	usize TC_parents_size = 0;
	ARRAY_CREATE(TC_offsets, usize);
	MAP_CREATE(TC_parents, usize, usize);
	TC_parents.hash = usize_hash_identity;

	usize NC_offset = 1;
	usize NC_parents_size = 0;
	ARRAY_CREATE(NC_offsets, usize);
	MAP_CREATE(NC_parents, usize, usize);
	NC_parents.hash = usize_hash_identity;

#define _CLEANUP \
	array_destroy(&VC_offsets); \
	map_destroy(&VC_parents); \
	array_destroy(&TC_offsets); \
	map_destroy(&TC_parents); \
	array_destroy(&NC_offsets); \
	map_destroy(&NC_parents); \
	if (output_file != stdout) \
		fclose(output_file)

	TRY_OR_ELSE(array_resize(&VC_offsets, file->frames.size), _CLEANUP);
	TRY_OR_ELSE(array_resize(&TC_offsets, file->frames.size), _CLEANUP);
	TRY_OR_ELSE(array_resize(&NC_offsets, file->frames.size), _CLEANUP);

	fold_file_frames_inherit(file);
	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		if NOT(frame->graph.VC.is_view) VC_parents_size++;
		if NOT(frame->graph.TC.is_view) TC_parents_size++;
		if NOT(frame->graph.NC.is_view) NC_parents_size++;
	}

	TRY_OR_ELSE(map_reserve(&VC_parents, VC_parents_size), _CLEANUP);
	TRY_OR_ELSE(map_reserve(&TC_parents, TC_parents_size), _CLEANUP);
	TRY_OR_ELSE(map_reserve(&NC_parents, NC_parents_size), _CLEANUP);

	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		if NOT(frame->graph.VC.is_view) {
			usize hash = hash_usize(USIZE(frame->graph.VC.data));
			TRY_OR_ELSE(map_add(&VC_parents, &hash, &index), _CLEANUP);

			usize VC_size = frame->graph.VC.size;
			if (fold_graph_is_abstract(&frame->graph)) {
				VC_size = fold_graph_get_abstract_size(&frame->graph);
			}

			TRY_ADD_OR_ELSE(VC_offset, VC_size, _CLEANUP);
			array_set(&VC_offsets, index, &VC_offset);
			VC_offset += VC_size;
		}

		if NOT(frame->graph.TC.is_view) {
			usize hash = hash_usize(USIZE(frame->graph.TC.data));
			TRY_OR_ELSE(map_add(&TC_parents, &hash, &index), _CLEANUP);

			TRY_ADD_OR_ELSE(TC_offset, frame->graph.TC.size, _CLEANUP);
			array_set(&TC_offsets, index, &TC_offset);
			TC_offset += frame->graph.TC.size;
		}

		if NOT(frame->graph.NC.is_view) {
			usize hash = hash_usize(USIZE(frame->graph.NC.data));
			TRY_OR_ELSE(map_add(&NC_parents, &hash, &index), _CLEANUP);

			TRY_ADD_OR_ELSE(NC_offset, frame->graph.NC.size, _CLEANUP);
			array_set(&NC_offsets, index, &NC_offset);
			NC_offset += frame->graph.NC.size;
		}
	}

	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		if (frame->graph.VC.is_view) {
			usize hash = hash_usize(USIZE(frame->graph.VC.data));
			usize* parent_index = map_get(&VC_parents, &hash, NULL);
			ASSERT(parent_index != NULL);
			array_set(&VC_offsets, index,
				array_get(&VC_offsets, *parent_index));
		}

		if (frame->graph.TC.is_view) {
			usize hash = hash_usize(USIZE(frame->graph.TC.data));
			usize* parent_index = map_get(&TC_parents, &hash, NULL);
			ASSERT(parent_index != NULL);
			array_set(&TC_offsets, index,
				array_get(&TC_offsets, *parent_index));
		}

		if (frame->graph.NC.is_view) {
			usize hash = hash_usize(USIZE(frame->graph.NC.data));
			usize* parent_index = map_get(&NC_parents, &hash, NULL);
			ASSERT(parent_index != NULL);
			array_set(&NC_offsets, index,
				array_get(&NC_offsets, *parent_index));
		}
	}

#undef _CLEANUP
	map_destroy(&VC_parents);
	map_destroy(&TC_parents);
	map_destroy(&NC_parents);

	RNG rng;
	random_set_seed(&rng, parameters->abstract_seed);
	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		usize* VC_offset = array_get(&VC_offsets, index);
		usize* TC_offset = array_get(&TC_offsets, index);
		usize* NC_offset = array_get(&NC_offsets, index);

		struct FoldFrameObjectContext object;
		object.parameters = parameters;
		object.VC_inherit = frame->graph.VC.is_view;
		object.TC_inherit = frame->graph.TC.is_view;
		object.NC_inherit = frame->graph.NC.is_view;
		object.VC_offset = *VC_offset;
		object.TC_offset = *TC_offset;
		object.NC_offset = *NC_offset;
		object.index = index;
		object.rng = &rng;

		TRY_OR_ELSE(fold_frame_to_obj(frame, output_file, &object),
			array_destroy(&VC_offsets);
			array_destroy(&TC_offsets);
			array_destroy(&NC_offsets);
			if (output_file != stdout)
				fclose(output_file));
	}

	array_destroy(&VC_offsets);
	array_destroy(&TC_offsets);
	array_destroy(&NC_offsets);

	TRY_OR_ERROR(ferror(output_file),
		if (output_file != stdout)
			fclose(output_file);
		return ERROR);

	if (output_file == stdout) return OK;
	return IS_ERROR(fclose(output_file))
		? ERROR : OK;
}