#ifndef CODECS_OBJ_H
#define CODECS_OBJ_H

#include "fold/file.h"

Error fold_file_from_obj(FoldFile* file, void* OBJ);
Error fold_file_to_obj(FoldFile* file, void* OBJ);

#endif /* CODECS_OBJ_H */