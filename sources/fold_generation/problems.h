#ifndef FOLD_GENERATION_H
#define FOLD_GENERATION_H

#include "fold_specification/file.h"
#include "definitions/random.h"

struct LFFParameters {
	usize min_boundary_vertices;
	usize max_boundary_vertices;
	usize min_interior_vertices;
	usize max_interior_vertices;
	real boundary_offset;
	real cell_size;
	real kawasaki_error;
	usize seed;
};

Error fold_generate_locally_flat_foldable(FoldFile* file,
	const struct LFFParameters* parameters);

#endif /* FOLD_GENERATION_H */