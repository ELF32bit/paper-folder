#ifndef FOLD_VALIDATION_FILE_H
#define FOLD_VALIDATION_FILE_H

#include "fold_specification/file.h"

Error fold_file_validate_inherit(FoldFile* file, bool* is,
	String* NULLABLE(errors));

Error fold_file_validate(FoldFile* file, bool* is,
	String* NULLABLE(errors));

#endif /* FOLD_VALIDATION_FILE_H */