#include "obj.h"

#include "fold_graph/attributes.h"
#include "fold_graph/vertices.h"

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

/* ========================================================================= */
/* FOLD Graph                                                                */
/* ========================================================================= */

static inline
Error fold_graph_to_obj(const FoldGraph* graph, void* OBJ, void* Object) {
	FILE* output_file = OBJ;
	struct FoldFrameObject* object = Object;

	usize abstract_size = 0;
	if (fold_graph_is_abstract(graph)) {
		abstract_size = fold_graph_get_abstract_size(graph);
		FOR_EACH(_, abstract_size) {
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

	TRY_SAFE(object->vertex_offset += abstract_size);
	TRY_SAFE(object->vertex_offset += graph->VC.size);
	TRY_SAFE(object->texture_offset += graph->TC.size);
	TRY_SAFE(object->normal_offset += graph->NC.size);
	return OK;
}

/* ========================================================================= */
/* FOLD Frame                                                                */
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
/* FOLD File                                                                 */
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