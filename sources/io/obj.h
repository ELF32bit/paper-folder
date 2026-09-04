#ifndef IO_OBJ_H
#define IO_OBJ_H

#include "fold/file.h"

Error fold_file_from_obj(FoldFile* file, void* OBJ);
Error fold_file_to_obj(FoldFile* file, void* OBJ);

#endif /* IO_OBJ_H */