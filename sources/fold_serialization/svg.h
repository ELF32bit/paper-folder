#ifndef FOLD_SERIALIZATION_SVG_H
#define FOLD_SERIALIZATION_SVG_H

#include "fold_specification/file.h"

struct FoldFileFrameToSVGParameters {
	// TODO
};

Error fold_file_frame_to_svg(FoldFile* file, usize index, void* NULLABLE(SVG),
	const struct FoldFileFrameToSVGParameters* parameters);

#endif /* FOLD_SERIALIZATION_SVG_H */
