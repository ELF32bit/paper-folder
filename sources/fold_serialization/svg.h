#ifndef FOLD_SERIALIZATION_SVG_H
#define FOLD_SERIALIZATION_SVG_H

#include "fold_specification/file.h"

#include "math/color.h"

struct FoldFileFrameToSVGParameters {
	real scale;
	usize abstract_seed;
	Color B_color, M_color, V_color, F_color, U_color, C_color, J_color;
	f32 B_scale, M_scale, V_scale, F_scale, U_scale, C_scale, J_scale;
	u8 B_order, M_order, V_order, F_order, U_order, C_order, J_order;

	f32 image_min_size;
	f32 image_grow_size;
	f32 stroke_width;

	bool enable_stroke_dash;
	real stroke_dash_scale;

	bool enable_vertices;
	bool enable_vertices_index;
	bool disable_vertices_color;
	real vertices_scale;

	bool disable_edges;
	bool enable_edges_index;
	bool enable_edges_fold_angle;
	bool enable_edges_length;
	real edges_annotations_scale;

	bool disable_faces;
	bool enable_faces_index;
	bool enable_faces_material;
	real faces_annotations_scale;
};

static inline
void fold_file_frame_to_svg_parameters_default(
	struct FoldFileFrameToSVGParameters* parameters)
{
	parameters->scale = 1.0;
	parameters->abstract_seed = 0;

	parameters->B_color = /* BLACK */
		color_from_html("#000000ff");

	parameters->M_color = /* RED */
		color_from_html("#ff0000ff");

	parameters->V_color = /* BLUE */
		color_from_html("#0000ffff");

	parameters->F_color = /* GRAY */
		color_from_html("#808080ff");

	parameters->U_color = /* YELLOW */
		color_from_html("#ffff00ff");

	parameters->C_color = /* GREEN */
		color_from_html("#00ff00ff");

	parameters->J_color = /* TRANSPARENT */
		color_from_html("#00000040");

	parameters->B_scale = 1.25;
	parameters->M_scale = 1.0;
	parameters->V_scale = 1.0;
	parameters->F_scale = 1.0;
	parameters->U_scale = 1.0;
	parameters->C_scale = 1.25;
	parameters->J_scale = 0.75;

	parameters->B_order = 60;
	parameters->M_order = 50;
	parameters->V_order = 40;
	parameters->F_order = 30;
	parameters->U_order = 20;
	parameters->C_order = 70;
	parameters->J_order = 10;

	parameters->image_min_size = 512.0;
	parameters->image_grow_size = 64.0;
	parameters->stroke_width = 1.0;

	parameters->enable_stroke_dash = false;
	parameters->stroke_dash_scale = 1.0;

	parameters->enable_vertices = false;
	parameters->enable_vertices_index = false;
	parameters->disable_vertices_color = false;
	parameters->vertices_scale = 1.0;

	parameters->disable_edges = false;
	parameters->enable_edges_index = false;
	parameters->enable_edges_fold_angle = false;
	parameters->enable_edges_length = false;
	parameters->edges_annotations_scale = 1.0;

	parameters->disable_faces = false;
	parameters->enable_faces_index = false;
	parameters->enable_faces_material = false;
	parameters->faces_annotations_scale = 1.0;
}

Error fold_file_frame_to_svg(FoldFile* file, usize index, void* NULLABLE(SVG),
	const struct FoldFileFrameToSVGParameters* parameters);

#endif /* FOLD_SERIALIZATION_SVG_H */
