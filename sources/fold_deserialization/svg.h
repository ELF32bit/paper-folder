#ifndef FOLD_DESERIALIZATION_SVG_H
#define FOLD_DESERIALIZATION_SVG_H

#include "fold_specification/file.h"

#include "math/color.h"

struct FoldFileFrameFromSVGParameters {
	char* image_units;
	f32 image_dpi;

	real scale;
	Color B_color;
	Color M_color;
	Color V_color;
	Color F_color;
	Color U_color;
	Color C_color;
	Color J_color;

	real flatness;
	real min_segment_length;
	bool triangulate;
};

static inline
void fold_file_frame_from_svg_parameters_default(
	struct FoldFileFrameFromSVGParameters* parameters)
{
	parameters->image_units = "px";
	parameters->image_dpi = 96.0;
	parameters->scale = 1.0;

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

	parameters->flatness = 0.5;
	parameters->min_segment_length = 1.0;
	parameters->triangulate = true;
}

/* TODO: Need very complex, BVH2 accelerated, Bezier curve intersector */
Error fold_file_frame_from_svg(FoldFile* file, usize index, void* SVG,
	const struct FoldFileFrameFromSVGParameters* parameters);

#endif /* FOLD_DESERIALIZATION_SVG_H */
