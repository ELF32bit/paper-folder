#ifndef FOLD_DESERIALIZATION_SVG_H
#define FOLD_DESERIALIZATION_SVG_H

#include "fold_specification/file.h"

struct FoldFileFrameFromSVGParameters {
	real flatness, min_segment_length;
};

/* TODO: Need very complex, BVH2 accelerated, Bezier curve intersector */
Error fold_file_frame_from_svg(FoldFile* file, usize index, void* SVG,
	const struct FoldFileFrameFromSVGParameters* parameters);

#endif /* FOLD_DESERIALIZATION_SVG_H */
