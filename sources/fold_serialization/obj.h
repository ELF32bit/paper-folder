#ifndef FOLD_SERIALIZATION_OBJ_H
#define FOLD_SERIALIZATION_OBJ_H

#include "fold_specification/file.h"

struct FoldFileToOBJParameters {
	bool triangulate;
};

/* TODO: Track unique vertices to output less data */
Error fold_file_to_obj(FoldFile* file, void* NULLABLE(OBJ),
	const struct FoldFileToOBJParameters* parameters);

#endif /* FOLD_SERIALIZATION_OBJ_H */