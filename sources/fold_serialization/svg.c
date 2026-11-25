#include "svg.h"

#include "fold_graph/attributes.h"
#include "fold_graph/properties.h"

#include "definitions/constants.h"
#include "definitions/random.h"
#include "definitions/math.h"

#include <stdlib.h>
#include <stdio.h>

#define _F "%g"
#define _FF float

#define SVG_FONT "sans-serif"
#define SVG_COLOR_WHITE "#ffffff"
#define SVG_COLOR_BLACK "#000000"

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

	real dash_size = vector2_max_axis(aabb2_size(aabb2)) / 100.0;
	dash_size /= MAX(parameters->stroke_dash_scale, REAL_EPSILON);

	real circle_shrink_factor = 2.5;
	real circle_radius = (8.0 * parameters->vertices_scale)
		* ((parameters->stroke_width * 0.5) / svg_scale);
	real circle_stroke_radius = circle_radius * 0.1;

	real circle2_edge_offset_factor = 1.5;
	real circle2_edge_offset_diagonal_factor = 1.5 / 6.0;
	real circle2_radius = (8.0 * parameters->edges_annotations_scale)
		* ((parameters->stroke_width * 0.5) / svg_scale);
	real circle2_stroke_radius = circle2_radius * 0.1;

	real circle3_radius = (8.0 * parameters->faces_annotations_scale)
		* ((parameters->stroke_width * 0.5) / svg_scale);
	real circle3_stroke_radius = circle3_radius * 0.1;

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
			(_FF)min.x, (_FF)min.y,
			(_FF)size.x, (_FF)size.y,
			(_FF)svg_size.x,
			(_FF)svg_size.y);

	if (fold_graph_is_abstract(graph)) {
		if (parameters->abstract_seed == 0) {
			goto svg_finish;
		}
	}

	if NOT(parameters->disable_faces) {
		fprintf(output_file,
			"<g id=\"faces\">\n");

		ARRAY2_ITERATE(&graph->FV, fv) {
			if (parameters->enable_faces_index ||
				parameters->enable_faces_material) {
				fprintf(output_file, "<g>\n");
			}

			fprintf(output_file,
				"<polygon points=\"");

			real cx = 0.0, cy = 0.0;
			ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
				i, usize*, fvi, fv.start, fv.end)
			{
				real x = 0.0, y = 0.0;
				if (fold_graph_is_2D(graph)) {
					Vector2* p = array_get(&VC, *fvi);
					x = p->x * scale; y = p->y * scale;
				} else if (fold_graph_is_3D(graph)) {
					Vector3* p = array_get(&VC, *fvi);
					x = p->x * scale; y = p->y * scale;
				}

				cx += x; cy += y;
				fprintf(output_file, ""_F","_F"%s",
					(_FF)x, (_FF)y, (i != (fv.end - 1))
						? " " : "");
			}
			cx /= fv.size; cy /= fv.size;
			fprintf(output_file, "\" ");

			char material[COLOR_HTML_RGBA_SIZE];
			color_to_html(color_white(), material, false);
			if (parameters->enable_faces_material) {
				if (graph->FM.size != 0) {
					usize* fmi = array_get(&graph->FM, fv.index);
					char* fmn = array_get(&graph->MN, *fmi);
					Color color = color_from_html(fmn);
					bool has_alpha = BOOL(color.a != 1.0);
					color_to_html(color, material, has_alpha);
				}
			}

			fprintf(output_file,
				"fill=\"%s\" "
				"stroke=\"none\" "
				"stroke-width=\""_F"\""
				"/>\n",
					material,
					(_FF)(__width / svg_scale));

			if (parameters->enable_faces_index) {
				fprintf(output_file, "<g>\n");

				fprintf(output_file,
					"<polygon points=\""
					_F","_F","_F","_F","_F","_F"\" "
					"fill=\"%s\" "
					"stroke=\"%s\" "
					"stroke-width=\""_F"\""
					"/>\n",
						(_FF)cx,
						(_FF)(cy - 2.0 * circle3_radius),
						(_FF)(cx - circle3_radius * REAL_SQRT3),
						(_FF)(cy + circle3_radius),
						(_FF)(cx + circle3_radius * REAL_SQRT3),
						(_FF)(cy + circle3_radius),
							SVG_COLOR_WHITE, SVG_COLOR_BLACK,
							(_FF)(circle3_stroke_radius));

				fprintf(output_file,
					"<text "
					"x=\""_F"\" y=\""_F"\" "
					"font-size=\""_F"\" "
					"font-family=\"%s\" "
					"font-weight=\"bold\" "
					"fill=\"%s\" "
					"text-anchor=\"middle\" "
					"dominant-baseline=\"central\""
					">%zu</text>\n",
						(_FF)cx, (_FF)cy,
						(_FF)((fv.index < 100) ? circle3_radius
							: (circle3_radius * circle_shrink_factor)
								/ F64(1 + U8(LOG10(F64(fv.index))))),
						SVG_FONT,
						SVG_COLOR_BLACK,
						fv.index);

				fprintf(output_file, "</g>\n");
			}

			if (parameters->enable_faces_index ||
				parameters->enable_faces_material) {
				fprintf(output_file, "</g>\n");
			}
		}

		fprintf(output_file, "</g>\n");
	}

	if NOT(parameters->disable_edges) {
		fprintf(output_file,
			"<g id=\"edges\">\n");

		FOR_EACH(index, BUFFER_SIZE(svg_edges)) {
			FoldGraphEdgeAssignment assignment = svg_edges[index].assignment;
			char* color = svg_edges[index].color;
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

				Vector2 v1 = vector2_from_components(x1, y1);
				Vector2 v2 = vector2_from_components(x2, y2);
				Vector2 v12 = vector2_subtract(v2, v1);

				real angle = REAL_DEGREES(ATAN2(v12.y, v12.x));
				angle = (angle > 90.0) ? (angle - 180.0)
					: (angle < -90.0) ? (angle + 180.0)
					: angle;

				Vector2 n12 = vector2_orthogonal(v12);
				n12 = vector2_normalize(n12);

				real d = MAX((ABS(n12.x) + ABS(n12.y)) - 1.0, 0.0);
				d = 1.0 + d * (circle2_edge_offset_diagonal_factor
					/ (REAL_SQRT2 - 1.0));

				real opacity = 1.0;
				real fold_angle = 0.0;
				if (graph->EFA.size != 0 &&
					(assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN ||
					assignment == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY)) {
						real* efa = array_get(&graph->EFA, ei);
						opacity = (ABS(*efa) / 180.0);
						fold_angle = *efa;
				}

				if (parameters->enable_edges_index ||
					parameters->enable_edges_length) {
					fprintf(output_file, "<g>\n");
				}

				fprintf(output_file,
					"<line "
					"x1=\""_F"\" y1=\""_F"\" "
					"x2=\""_F"\" y2=\""_F"\" "
					"stroke=\"%s\" "
					"stroke-width=\""_F"\" "
					"stroke-opacity=\""_F"\" "
					"stroke-linecap=\"round\"",
						(_FF)x1, (_FF)y1,
						(_FF)x2, (_FF)y2,
						color,
						(_FF)(width / svg_scale),
						(_FF)opacity);

				if (parameters->enable_stroke_dash) {
					switch (assignment) {
						case FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY:
						case FOLD_GRAPH_EDGE_ASSIGNMENT_FLAT:
							fprintf(output_file,
								" stroke-dasharray=\"none\"");
							break;
						case FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN:
							fprintf(output_file,
								" stroke-dasharray=\""_F","_F","_F","_F"\"",
									(_FF)(dash_size * 3.0),
									(_FF)(dash_size * 0.9),
									(_FF)(dash_size * 0.2),
									(_FF)(dash_size * 0.9));
							break;
						case FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY:
							fprintf(output_file,
								" stroke-dasharray=\""_F","_F"\"",
									(_FF)(dash_size * 3.0),
									(_FF)(dash_size * 2.0));
							break;
						case FOLD_GRAPH_EDGE_ASSIGNMENT_CUT:
							fprintf(output_file,
								" stroke-dasharray=\""_F","_F"\"",
									(_FF)(dash_size * 0.0),
									(_FF)(dash_size * 1.0));
							break;
						case FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN:
							fprintf(output_file,
								" stroke-dasharray=\""_F","_F"\"",
									(_FF)(dash_size * 0.0),
									(_FF)(dash_size * 2.5));
							break;
						case FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN:
						default:
							fprintf(output_file,
								" stroke-dasharray=\""_F","_F"\"",
									(_FF)(dash_size * 1.0),
									(_FF)(dash_size * 1.0));
							break;
					}
				}
				fprintf(output_file, "/>\n");

				if (parameters->enable_edges_index) {
					fprintf(output_file, "<g>\n");

					Vector2 o12 = vector2_add(
						vector2_add(v1, vector2_scale(v12, 0.5)),
						vector2_scale(n12, d * circle2_radius *
							circle2_edge_offset_factor));

					fprintf(output_file,
						"<rect "
						"x=\""_F"\" y=\""_F"\" "
						"width=\""_F"\" height=\""_F"\" "
						"fill=\"%s\" "
						"stroke=\"%s\" "
						"stroke-width=\""_F"\""
						"/>\n",
							(_FF)(o12.x - circle2_radius),
							(_FF)(o12.y - circle2_radius),
							(_FF)(2.0 * circle2_radius),
							(_FF)(2.0 * circle2_radius),
								SVG_COLOR_WHITE, SVG_COLOR_BLACK,
								(_FF)(circle2_stroke_radius));

					fprintf(output_file,
						"<text "
						"x=\""_F"\" y=\""_F"\" "
						"font-size=\""_F"\" "
						"font-family=\"%s\" "
						"font-weight=\"bold\" "
						"fill=\"%s\" "
						"text-anchor=\"middle\" "
						"dominant-baseline=\"central\""
						">%zu</text>\n",
							(_FF)o12.x, (_FF)o12.y,
							(_FF)((ei < 100) ? circle2_radius
								: (circle2_radius * circle_shrink_factor)
									/ F64(1 + U8(LOG10(F64(ei))))),
							SVG_FONT,
							SVG_COLOR_BLACK,
							ei);

					fprintf(output_file, "</g>\n");
				}

				if (parameters->enable_edges_fold_angle) {
					if (fold_angle != 0.0) {
						real _sign = (parameters->enable_edges_index
							&& (NOT(parameters->enable_edges_length)
							|| graph->EL.size == 0))
								? -1.0 : 1.0;

						Vector2 o12 = vector2_add(
							vector2_add(v1, vector2_scale(v12, 0.5)),
							vector2_scale(n12, _sign * circle2_radius *
								circle2_edge_offset_factor));

						fprintf(output_file,
							"<text "
							"x=\""_F"\" y=\""_F"\" "
							"font-size=\""_F"\" "
							"font-family=\"%s\" "
							"font-weight=\"bold\" "
							"fill=\"%s\" "
							"stroke=\"%s\" "
							"stroke-width=\""_F"\" "
							"stroke-linejoin=\"round\" "
							"paint-order=\"stroke\" "
							"text-anchor=\"middle\" "
							"dominant-baseline=\"central\" "
							"transform=\"rotate("_F" "_F" "_F")\""
							">"_F"</text>\n",
								(_FF)o12.x, (_FF)o12.y,
								(_FF)circle2_radius,
								SVG_FONT,
								SVG_COLOR_BLACK,
								SVG_COLOR_WHITE,
								(_FF)circle2_stroke_radius,
								(_FF)angle, (_FF)o12.x, (_FF)o12.y,
								(_FF)fold_angle);
					}
				}

				if (parameters->enable_edges_length) {
					if (graph->EL.size != 0) {
						real* el = array_get(&graph->EL, ei);

						Vector2 o21 = vector2_subtract(
							vector2_add(v1, vector2_scale(v12, 0.5)),
							vector2_scale(n12, circle2_radius *
								circle2_edge_offset_factor));

						fprintf(output_file,
							"<text "
							"x=\""_F"\" y=\""_F"\" "
							"font-size=\""_F"\" "
							"font-family=\"%s\" "
							"font-weight=\"bold\" "
							"fill=\"%s\" "
							"stroke=\"%s\" "
							"stroke-width=\""_F"\" "
							"stroke-linejoin=\"round\" "
							"paint-order=\"stroke\" "
							"text-anchor=\"middle\" "
							"dominant-baseline=\"central\" "
							"transform=\"rotate("_F" "_F" "_F")\""
							">"_F"</text>\n",
								(_FF)o21.x, (_FF)o21.y,
								(_FF)circle2_radius,
								SVG_FONT,
								SVG_COLOR_WHITE,
								SVG_COLOR_BLACK,
								(_FF)circle2_stroke_radius,
								(_FF)angle, (_FF)o21.x, (_FF)o21.y,
								(_FF)*el);
					}
				}

				if (parameters->enable_edges_index ||
					parameters->enable_edges_length) {
					fprintf(output_file, "</g>\n");
				}
			}
		}

		fprintf(output_file, "</g>\n");
	}

	if (parameters->enable_vertices) {
		fprintf(output_file,
			"<g id=\"vertices\">\n");

		FOR_EACH(vi, graph->VC.size) {
			real x = 0.0, y = 0.0;
			if (fold_graph_is_2D(graph)) {
				Vector2* vc = array_get(&VC, vi);
				x = vc->x * scale; y = vc->y * scale;
			} else if (fold_graph_is_3D(graph)) {
				Vector3* vc = array_get(&VC, vi);
				x = vc->x * scale; y = vc->y * scale;
			}

			Color vpc = color_white();
			if NOT(parameters->disable_vertices_color) {
				if (graph->VP.size != 0) {
					usize* vpi = array_get(&graph->VP, vi);
					vpc = *(Color*)array_get(&graph->PC, *vpi);
				}
			}

			Color ivpc = color_invert(vpc);
			bool has_alpha = BOOL(vpc.a != 1.0);

			char VPC[COLOR_HTML_RGBA_SIZE];
			color_to_html(vpc, VPC, has_alpha);

			char IVPC[COLOR_HTML_RGBA_SIZE];
			color_to_html(ivpc, IVPC, has_alpha);

			if (parameters->enable_vertices_index) {
				fprintf(output_file, "<g>\n");
			}

			fprintf(output_file,
				"<circle "
				"cx=\""_F"\" cy=\""_F"\" "
				"r=\""_F"\" "
				"stroke=\"%s\" "
				"stroke-width=\""_F"\" "
				"fill=\"%s\" "
				"/>\n",
					(_FF)x, (_FF)y,
					(_FF)circle_radius,
					IVPC,
					(_FF)(circle_stroke_radius),
					VPC);

			if (parameters->enable_vertices_index) {
				fprintf(output_file,
					"<text "
					"x=\""_F"\" y=\""_F"\" "
					"font-size=\""_F"\" "
					"font-family=\"%s\" "
					"font-weight=\"bold\" "
					"fill=\"%s\" "
					"text-anchor=\"middle\" "
					"dominant-baseline=\"central\""
					">%zu</text>\n",
						(_FF)x, (_FF)y,
						(_FF)((vi < 100) ? circle_radius
							: (circle_radius * circle_shrink_factor)
								/ F64(1 + U8(LOG10(F64(vi))))),
						SVG_FONT,
						IVPC,
						vi);

				fprintf(output_file, "</g>\n");
			}
		}

		fprintf(output_file, "</g>\n");
	}

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