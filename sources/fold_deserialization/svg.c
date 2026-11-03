#include "svg.h"

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#include "math/vector2.h"
#include "math/bezier2.h"
#include "math/color.h"

#define SVG_COLOR_EPSILON 0.1

/* ========================================================================= */
/* FOLD Graph                                                                */
/* ========================================================================= */

static inline
Error fold_graph_from_svg(FoldGraph* graph, void* SVG,
	const struct FoldFileFrameFromSVGParameters* parameters)
{
	NSVGimage* image = nsvgParseFromFile((const char*)SVG, "px", 96.0f);
	TRY_NEW_MEMORY_OR_ERROR(image, return ERROR);

	/* ABGR format */
	const Color B = color_from_rgba8(255, 0, 0, 0);
	const Color M = color_from_rgba8(255, 0, 0, 255);
	const Color V = color_from_rgba8(255, 255, 0, 0);
	const Color F = color_from_rgba8(255, 128, 128, 128);
	const Color U = color_from_rgba8(255, 0, 255, 255);
	const Color C = color_from_rgba8(255, 0, 255, 0);
	const Color J = color_from_rgba8(255, 0, 255, 255);

	for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next) {
		Color color = color_unpack(shape->stroke.color);
		real fold_angle = color.r * shape->opacity;
		color.r = 1.0;

		FoldGraphEdgeAssignment assignment =
			FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;

		if (color_is_equal(color, B, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY;
		} else if (color_is_equal(color, M, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN;
		} else if (color_is_equal(color, V, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY;
		} else if (color_is_equal(color, F, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_FLAT;
		} else if (color_is_equal(color, U, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;
		} else if (color_is_equal(color, C, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_CUT;
		} else if (color_is_equal(color, J, SVG_COLOR_EPSILON)) {
			assignment = FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN;
		}

		fold_angle *=
			(assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) ? -180.0
			: (assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) ? 180.0
			: 0.0;

		for (NSVGpath* path = shape->paths; path != NULL; path = path->next) {
			for (int i = 0; i < (path->npts - 3) * 2; i += 6) {
				Bezier2 curve = bezier2_from_components(
					vector2_from_components(
						path->pts[i + 0],
						path->pts[i + 1]),
					vector2_from_components(
						path->pts[i + 2],
						path->pts[i + 3]),
					vector2_from_components(
						path->pts[i + 4],
						path->pts[i + 5]),
					vector2_from_components(
						path->pts[i + 6],
						path->pts[i + 7]));

				Bezier2Vertices vertices =
					bezier2_tessellate_adaptive(curve,
						CLAMP(parameters->flatness, 0.0, REAL_INFINITY),
						CLAMP(parameters->min_segment_length, 0.0, REAL_INFINITY));

				if (vertices.count <= 1) continue;
				FOR_EACH(index, vertices.count - 1) {
					Vector2 A = vertices.points[index + 0];
					Vector2 B = vertices.points[index + 1];
					FoldGraphEdge edge = {
						.a = graph->VC.size + 0,
						.b = graph->VC.size + 1
					};

					TRY_OR_ELSE(
						array_append(&graph->VC, &A),
							nsvgDelete(image));

					TRY_OR_ELSE(
						array_append(&graph->VC, &B),
							nsvgDelete(image));

					TRY_OR_ELSE(
						array_append(&graph->EV, &edge),
							nsvgDelete(image));

					TRY_OR_ELSE(
						array_append(&graph->EA, &assignment),
							nsvgDelete(image));

					TRY_OR_ELSE(
						array_append(&graph->EFA, &fold_angle),
							nsvgDelete(image));
				}
			}
		}
	}

	nsvgDelete(image);
	return OK;
}

/* ========================================================================= */
/* FOLD Frame                                                                */
/* ========================================================================= */

static inline
Error fold_frame_from_svg(FoldFrame* frame, void* SVG,
	const struct FoldFileFrameFromSVGParameters* parameters)
{
	fold_frame_recreate(frame);
	frame->graph.VC.element_size = sizeof(Vector2);
	TRY(fold_graph_from_svg(&frame->graph, SVG, parameters));
	return OK;
}

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

Error fold_file_frame_from_svg(FoldFile* file, usize index, void* SVG,
	const struct FoldFileFrameFromSVGParameters* parameters)
{
	if NOT(index < file->frames.size) return ERROR;
	FoldFrame* frame = array_get(&file->frames, index);
	TRY(fold_frame_from_svg(frame, SVG, parameters));
	return OK;
}