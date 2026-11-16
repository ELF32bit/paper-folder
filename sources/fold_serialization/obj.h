#ifndef FOLD_SERIALIZATION_OBJ_H
#define FOLD_SERIALIZATION_OBJ_H

#include "fold_specification/file.h"

struct FoldFileToOBJParameters {
	bool enable_groups;
	bool disable_colors;
	bool enable_weights;
	bool disable_textures;
	bool disable_normals;
	bool disable_materials;
	bool disable_lines;
	bool disable_faces;
	usize abstract_seed;
	f32 abstract_size;
};

Error fold_file_to_obj(FoldFile* file, void* NULLABLE(OBJ),
	const struct FoldFileToOBJParameters* parameters);

#endif /* FOLD_SERIALIZATION_OBJ_H */