#ifndef FOLD_DESERIALIZATION_OBJ_H
#define FOLD_DESERIALIZATION_OBJ_H

#include "fold_specification/file.h"

struct FoldFileFromOBJParameters {
	bool triangulate;
};

Error fold_file_from_obj(FoldFile* file, void* OBJ,
	const struct FoldFileFromOBJParameters* parameters);

#endif /* FOLD_DESERIALIZATION_OBJ_H */