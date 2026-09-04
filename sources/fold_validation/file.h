#ifndef FOLD_FILE_VALIDATION
#define FOLD_FILE_VALIDATION

#include "fold_specification/file.h"

Error fold_file_validate(FoldFile* file, bool* is, String* errors);

#endif /* FOLD_FILE_VALIDATION */