#ifndef FOLD_DESERIALIZATION_OBJ_H
#define FOLD_DESERIALIZATION_OBJ_H

#include "fold_specification/file.h"

struct FoldFileFromOBJParameters {
	real scale;
	bool sort_objects;
	bool disable_objects;
	bool disable_groups;
	bool disable_colors;
	bool enable_weights;
	bool disable_textures;
	bool disable_normals;
	bool disable_materials;
	bool disable_lines;
	bool disable_faces;
};

Error fold_file_from_obj(FoldFile* file, void* OBJ,
	const struct FoldFileFromOBJParameters* parameters);

#endif /* FOLD_DESERIALIZATION_OBJ_H */