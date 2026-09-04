#ifndef FOLD_SERIALIZATION_OBJ_H
#define FOLD_SERIALIZATION_OBJ_H

#include "fold_specification/file.h"

/* TODO: Track unique vertices to output less data */
Error fold_file_to_obj(FoldFile* file, void* OBJ);

#endif /* FOLD_SERIALIZATION_OBJ_H */