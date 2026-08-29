#include "svg.h"

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#include <stdio.h>

#define _F "%g"
#define _FF float

#define SVG_GROW_SIZE 64.0
#define SVG_MIN_SIZE (512.0 - SVG_GROW_SIZE)
#define SVG_LINE_WIDTH 1.0

#define SVG_XMLNS "http://www.w3.org/2000/svg"

static const FoldGraphEdgeAssignment EA_display_priority[] = {
	FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN,
	FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN,
	FOLD_GRAPH_EDGE_ASSIGNMENT_FLAT,
	FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY,
	FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN,
	FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY,
	FOLD_GRAPH_EDGE_ASSIGNMENT_CUT,
};

static const char EA_display_colors[][10] = {
	"#00000040",
	"#ffff00ff",
	"#808080ff",
	"#0000ffff",
	"#ff0000ff",
	"#000000ff",
	"#00ff00ff",
};

/* ========================================================================= */
/* FOLD Graph Deserialization                                                */
/* ========================================================================= */

static inline
Error fold_graph_from_svg(FoldGraph* graph, void* SVG) {
	NSVGimage* image = nsvgParseFromFile((const char*)SVG, "px", 96.0f);
	if (image == NULL) {
		return ERROR;
	}

	for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next) {
		for (NSVGpath* path = shape->paths; path != NULL; path = path->next) {
			for (int i = 0; i < (path->npts - 3) * 2; i += 6) {
				Vector2 p0 = {
					.x = path->pts[i + 0],
					.y = path->pts[i + 1]};
				Vector2 p1 = {
					.x = path->pts[i + 2],
					.y = path->pts[i + 3]};
				Vector2 p2 = {
					.x = path->pts[i + 4],
					.y = path->pts[i + 5]};
				Vector2 p3 = {
					.x = path->pts[i + 6],
					.y = path->pts[i + 7]};

				FoldGraphEdge edge = {
					.a = graph->VC.size + 0,
					.b = graph->VC.size + 1};

				array_append(&graph->VC, &p0);
				array_append(&graph->VC, &p3);
				array_append(&graph->EV, &edge);
			}
		}
	}

	nsvgDelete(image);
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Deserialization                                                */
/* ========================================================================= */

static inline
Error fold_frame_from_svg(FoldFrame* frame, void* SVG) {
	fold_frame_recreate(frame);
	frame->graph.VC.element_size = sizeof(Vector2);
	TRY(fold_graph_from_svg(&frame->graph, SVG));
	return OK;
}

/* ========================================================================= */
/* FOLD File Deserialization                                                 */
/* ========================================================================= */

Error fold_file_frame_from_svg(FoldFile* file, usize index, void* SVG) {
	if NOT(index < file->frames.size) return ERROR;
	FoldFrame* frame = array_get(&file->frames, index);
	TRY(fold_frame_from_svg(frame, SVG));
	return OK;
}

/* ========================================================================= */
/* FOLD Graph Serialization                                                  */
/* ========================================================================= */

static inline
Error fold_graph_to_svg(FoldGraph* graph, void* SVG) {
	FILE* output_file = SVG;
	real scale = 1.0;

	AABB2 aabb2 = fold_graph_get_aabb2(graph);
	real min_size = aabb2_axis_size(aabb2,
		aabb2_shortest_axis_index(aabb2));
	if (min_size < SVG_MIN_SIZE) {
		scale = (min_size > 0.01)
			? SVG_MIN_SIZE / min_size
			: scale;
	}

	Vector2 min = vector2_scale(aabb2.min, scale);
	min.x -= (SVG_GROW_SIZE / 2.0f);
	min.y -= (SVG_GROW_SIZE / 2.0f);

	Vector2 size = aabb2_size(aabb2);
	size = vector2_scale(size, scale);
	size.x = ROUND(size.x + SVG_GROW_SIZE);
	size.y = ROUND(size.y + SVG_GROW_SIZE);

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
			(_FF)min.x, (_FF)min.y,
			(_FF)size.x, (_FF)size.y,
			(_FF)size.x, (_FF)size.y);

	if (fold_graph_is_abstract(graph)) {
		goto svg_finish;
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
				Vector2* p = array_get(&graph->VC, *fvi);
				x = p->x * scale; y = p->y * scale;
				if (i == fv.start) { x0 = x; y0 = y; }
			} else if (fold_graph_is_3D(graph)) {
				Vector3* p = array_get(&graph->VC, *fvi);
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
				SVG_LINE_WIDTH * 0.5,
				1.0);
	}

	fprintf(output_file, "</g>\n");

	fprintf(output_file,
		"<g "
		"id=\"edges\""
		">\n");

	FOR_EACH(i, BUFFER_SIZE(EA_display_priority)) {
		FoldGraphEdgeAssignment assignment = EA_display_priority[i];
		const char* color = EA_display_colors[i];

		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			if (graph->EA.size != 0) {
				FoldGraphEdgeAssignment* ea = array_get(&graph->EA, ei);
				if (*ea != assignment &&
					NOT(assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN &&
					NOT(FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(*ea)))) {
					continue;
				}
			} else if (assignment != FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN) {
				continue;
			}

			real x1 = 0.0, y1 = 0.0;
			real x2 = 0.0, y2 = 0.0;
			if (fold_graph_is_2D(graph)) {
				Vector2* a = array_get(&graph->VC, ev->a);
				Vector2* b = array_get(&graph->VC, ev->b);
				x1 = a->x * scale; y1 = a->y * scale;
				x2 = b->x * scale; y2 = b->y * scale;
			} else if (fold_graph_is_3D(graph)) {
				Vector3* a = array_get(&graph->VC, ev->a);
				Vector3* b = array_get(&graph->VC, ev->b);
				x1 = a->x * scale; y1 = a->y * scale;
				x2 = b->x * scale; y2 = b->y * scale;
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
					SVG_LINE_WIDTH * (1.0 + (assignment ==
						FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY)),
					(_FF)opacity);
		}
	}

	fprintf(output_file, "</g>\n");

svg_finish:
	fprintf(output_file, "</svg>\n");

	return OK;
}

/* ========================================================================= */
/* FOLD Frame Serialization                                                  */
/* ========================================================================= */

static inline
Error fold_frame_to_svg(FoldFrame* frame, void* SVG) {
	FILE* output_file = (SVG != NULL)
		? fopen((const char*)SVG, "w")
		: stdout;

	TRY_NEW_MEMORY_OR_ERROR(output_file,
		return ERROR);

	TRY_OR_ELSE(fold_graph_to_svg(&frame->graph, output_file),
		fclose(output_file));

	TRY_OR_ERROR(ferror(output_file),
		fclose(output_file);
		return ERROR);

	return IS_ERROR(fclose(output_file))
		? ERROR : OK;
}

/* ========================================================================= */
/* FOLD File Serialization                                                   */
/* ========================================================================= */

Error fold_file_frame_to_svg(FoldFile* file, usize index, void* SVG) {
	if NOT(index < file->frames.size) return ERROR;
	FoldFrame* frame = fold_file_frame_inherit(file, index);
	TRY(fold_frame_to_svg(frame, SVG));
	return OK;
}