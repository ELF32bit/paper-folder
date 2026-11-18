#include "svg.h"

#include "fold_graph/attributes.h"
#include "fold_graph/properties.h"

#include "definitions/random.h"

#include <stdlib.h>
#include <stdio.h>

#define _F "%g"
#define _FF float

#define SVG_XMLNS "http://www.w3.org/2000/svg"

/* ========================================================================= */
/* FOLD Graph                                                                */
/* ========================================================================= */

struct SVGEdge {
	FoldGraphEdgeAssignment assignment;
	char* color; f32 width; u8 order;
};

static inline
int _sort_svg_edges_ascending(const void* a, const void* b) {
	u8 o1 = ((const struct SVGEdge*)a)->order;
	u8 o2 = ((const struct SVGEdge*)b)->order;
	if (o1 < o2) return -1;
	if (o1 > o2) return 1;
	return 0;
}

static inline
Error fold_graph_to_svg(FoldGraph* graph, void* SVG,
	const struct FoldFileFrameToSVGParameters* parameters)
{
	FILE* output_file = SVG;

	char B_color[COLOR_HTML_RGBA_SIZE];
	char M_color[COLOR_HTML_RGBA_SIZE];
	char V_color[COLOR_HTML_RGBA_SIZE];
	char F_color[COLOR_HTML_RGBA_SIZE];
	char U_color[COLOR_HTML_RGBA_SIZE];
	char C_color[COLOR_HTML_RGBA_SIZE];
	char J_color[COLOR_HTML_RGBA_SIZE];

	color_to_html(parameters->B_color, B_color, true);
	color_to_html(parameters->M_color, M_color, true);
	color_to_html(parameters->V_color, V_color, true);
	color_to_html(parameters->F_color, F_color, true);
	color_to_html(parameters->U_color, U_color, true);
	color_to_html(parameters->C_color, C_color, true);
	color_to_html(parameters->J_color, J_color, true);

	f32 __width = MAX(parameters->stroke_width * 0.5, 0.0);
	f32 B_width = MAX(parameters->stroke_width * parameters->B_scale, 0.0);
	f32 M_width = MAX(parameters->stroke_width * parameters->M_scale, 0.0);
	f32 V_width = MAX(parameters->stroke_width * parameters->V_scale, 0.0);
	f32 F_width = MAX(parameters->stroke_width * parameters->F_scale, 0.0);
	f32 U_width = MAX(parameters->stroke_width * parameters->U_scale, 0.0);
	f32 C_width = MAX(parameters->stroke_width * parameters->C_scale, 0.0);
	f32 J_width = MAX(parameters->stroke_width * parameters->J_scale, 0.0);

	RNG rng;
	random_set_seed(&rng, parameters->abstract_seed);
	struct SVGEdge svg_edges[FOLD_GRAPH_EDGE_ASSIGNMENTS_COUNT];

	svg_edges[0].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY;
	svg_edges[0].color = B_color; svg_edges[0].width = B_width;
	svg_edges[0].order = parameters->B_order;

	svg_edges[1].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN;
	svg_edges[1].color = M_color; svg_edges[1].width = M_width;
	svg_edges[1].order = parameters->M_order;

	svg_edges[2].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY;
	svg_edges[2].color = V_color; svg_edges[2].width = V_width;
	svg_edges[2].order = parameters->V_order;

	svg_edges[3].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_FLAT;
	svg_edges[3].color = F_color; svg_edges[3].width = F_width;
	svg_edges[3].order = parameters->F_order;

	svg_edges[4].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;
	svg_edges[4].color = U_color; svg_edges[4].width = U_width;
	svg_edges[4].order = parameters->U_order;

	svg_edges[5].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_CUT;
	svg_edges[5].color = C_color; svg_edges[5].width = C_width;
	svg_edges[5].order = parameters->C_order;

	svg_edges[6].assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN;
	svg_edges[6].color = J_color; svg_edges[6].width = J_width;
	svg_edges[6].order = parameters->J_order;

	qsort(svg_edges,
		BUFFER_SIZE(svg_edges), sizeof(svg_edges[0]),
			_sort_svg_edges_ascending);

	f32 svg_min_size = MAX(parameters->image_min_size, 0.0);
	f32 svg_grow_size = MAX(parameters->image_grow_size, 0.0);
	f32 svg_content_size = MAX(svg_min_size - svg_grow_size, 0.0);

	Array VC;
	array_view(&VC, &graph->VC);
	if (fold_graph_is_abstract(graph)) {
		if (parameters->abstract_seed != 0) {
			array_recreate(&VC);
			TRY_OR_ELSE(array_resize(&VC,
				fold_graph_get_abstract_size(graph)),
					array_destroy(&VC));

			if (fold_graph_is_2D(graph)) {
				FOR_EACH(index, VC.size) {
					ARRAY_SET(&VC, index, Vector2, {
						.x = random_real_in_range(&rng, 0.0, 1.0),
						.y = random_real_in_range(&rng, 0.0, 1.0)
					});
				}
			} else if (fold_graph_is_3D(graph)) {
				FOR_EACH(index, VC.size) {
					ARRAY_SET(&VC, index, Vector3, {
						.x = random_real_in_range(&rng, 0.0, 1.0),
						.y = random_real_in_range(&rng, 0.0, 1.0),
						.z = random_real_in_range(&rng, 0.0, 1.0)
					});
				}
			}
		}
	}

	FoldGraph abstract_graph;
	fold_graph_create(&abstract_graph);
	abstract_graph.VC = VC;

	real scale = parameters->scale;
	AABB2 aabb2 = fold_graph_get_aabb2(&abstract_graph);
	aabb2.min = vector2_scale(aabb2.min, scale);
	aabb2.max = vector2_scale(aabb2.max, scale);
	aabb2 = aabb2_abs(aabb2);

	real min_size =
		aabb2_axis_size(aabb2,
			aabb2_shortest_axis_index(aabb2));

	real svg_scale = MAX((min_size > REAL_EPSILON)
		? (svg_content_size / min_size)
		: 1.0, REAL_EPSILON);

	Vector2 min = aabb2.min;
	min.x -= (svg_grow_size / svg_scale) * 0.5;
	min.y -= (svg_grow_size / svg_scale) * 0.5;

	Vector2 size = aabb2_size(aabb2);
	size.x += svg_grow_size / svg_scale;
	size.y += svg_grow_size / svg_scale;

	Vector2 svg_size = vector2_scale(size, svg_scale);
	svg_size.x = MAX(ROUND(svg_size.x), 1.0);
	svg_size.y = MAX(ROUND(svg_size.y), 1.0);

	fprintf(output_file,
		"<?xml "
		"version=\"1.0\" "
		"encoding=\"utf-8\"?"
		">\n");

	fprintf(output_file,
		"<svg "
		"xmlns=\""SVG_XMLNS"\" "
		"viewBox=\""_F" "_F" "_F" "_F"\" "
		"width=\""_F"\" "
		"height=\""_F"\" "
		"fill=\"none\""
		">\n",
			(_FF)min.x, (_FF)min.y, (_FF)size.x, (_FF)size.y,
			(_FF)svg_size.x, (_FF)svg_size.y);

	if (fold_graph_is_abstract(graph)) {
		if (parameters->abstract_seed == 0) {
			goto svg_finish;
		}
	}

	fprintf(output_file,
		"<g "
		"id=\"faces\""
		">\n");

	ARRAY2_ITERATE(&graph->FV, fv) {
		fprintf(output_file,
			"<polyline points=\"");

		real x0 = 0.0, y0 = 0.0;
		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, fvi, fv.start, fv.end)
		{
			real x = 0.0, y = 0.0;
			if (fold_graph_is_2D(graph)) {
				Vector2* p = array_get(&VC, *fvi);
				x = p->x * scale; y = p->y * scale;
				if (i == fv.start) { x0 = x; y0 = y; }
			} else if (fold_graph_is_3D(graph)) {
				Vector3* p = array_get(&VC, *fvi);
				x = p->x * scale; y = p->y * scale;
				if (i == fv.start) { x0 = x; y0 = y; }
			}
			fprintf(output_file, ""_F","_F" ",
				(_FF)x, (_FF)y);
		}
		fprintf(output_file, ""_F","_F"\" ",
			(_FF)x0, (_FF)y0);

		fprintf(output_file,
			"fill=\"#ffffffff\" "
			"stroke=\"none\" "
			"stroke-width=\""_F"\" "
			"stroke-opacity=\""_F"\" "
			"stroke-linecap=\"round\""
			"/>\n",
				(_FF)(__width / svg_scale),
				(_FF)1.0);
	}

	fprintf(output_file, "</g>\n");

	fprintf(output_file,
		"<g "
		"id=\"edges\""
		">\n");

	FOR_EACH(index, BUFFER_SIZE(svg_edges)) {
		FoldGraphEdgeAssignment assignment = svg_edges[index].assignment;
		const char* color = svg_edges[index].color;
		f32 width = svg_edges[index].width;
		if (width <= 0.0) continue;

		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			FoldGraphEdgeAssignment ea = (graph->EA.size != 0)
				? *(FoldGraphEdgeAssignment*)array_get(&graph->EA, ei)
				: FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;

			if NOT(FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(ea)) {
				ea = FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;
			}

			if (ea != assignment) {
				continue;
			}

			real x1 = 0.0, y1 = 0.0;
			real x2 = 0.0, y2 = 0.0;
			if (fold_graph_is_2D(graph)) {
				Vector2* A = array_get(&VC, ev->a);
				Vector2* B = array_get(&VC, ev->b);
				x1 = A->x * scale; y1 = A->y * scale;
				x2 = B->x * scale; y2 = B->y * scale;
			} else if (fold_graph_is_3D(graph)) {
				Vector3* A = array_get(&VC, ev->a);
				Vector3* B = array_get(&VC, ev->b);
				x1 = A->x * scale; y1 = A->y * scale;
				x2 = B->x * scale; y2 = B->y * scale;
			}

			real opacity = 1.0;
			if (graph->EFA.size != 0 &&
				(assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN ||
				assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY)) {
				real* efa = array_get(&graph->EFA, ei);
				opacity = (ABS(*efa) / 180.0);
			}

			fprintf(output_file,
				"<line "
				"x1=\""_F"\" y1=\""_F"\" "
				"x2=\""_F"\" y2=\""_F"\" "
				"stroke=\"%s\" "
				"stroke-width=\""_F"\" "
				"stroke-opacity=\""_F"\" "
				"stroke-linecap=\"round\""
				"/>\n",
					(_FF)x1, (_FF)y1,
					(_FF)x2, (_FF)y2,
					color,
					(_FF)(width / svg_scale),
					(_FF)opacity);
		}
	}

	fprintf(output_file, "</g>\n");

svg_finish:
	fprintf(output_file, "</svg>\n");

	fold_graph_destroy(&abstract_graph);
	return OK;
}

/* ========================================================================= */
/* FOLD Frame                                                                */
/* ========================================================================= */

static inline
Error fold_frame_to_svg(FoldFrame* frame, void* NULLABLE(SVG),
	const struct FoldFileFrameToSVGParameters* parameters)
{
	FILE* output_file = (SVG != NULL)
		? fopen((const char*)SVG, "w")
		: stdout;

	TRY_NEW_MEMORY_OR_ERROR(output_file,
		return ERROR);

	TRY_OR_ELSE(
		fold_graph_to_svg(&frame->graph, output_file, parameters),
			if (output_file != stdout)
				fclose(output_file));

	TRY_OR_ERROR(ferror(output_file),
		if (output_file != stdout)
			fclose(output_file);
		return ERROR);

	if (output_file == stdout) return OK;
	return IS_ERROR(fclose(output_file))
		? ERROR : OK;
}

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

Error fold_file_frame_to_svg(FoldFile* file, usize index, void* NULLABLE(SVG),
	const struct FoldFileFrameToSVGParameters* parameters)
{
	if NOT(index < file->frames.size) return ERROR;
	FoldFrame* frame = fold_file_frame_inherit(file, index);
	TRY(fold_frame_to_svg(frame, SVG, parameters));
	return OK;
}