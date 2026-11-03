#ifndef FOLD_FRAME_VALIDATION
#define FOLD_FRAME_VALIDATION

#include "fold_specification/frame.h"

Error fold_frame_validate(const FoldFrame* frame, bool* is,
	String* NULLABLE(errors));

Error fold_frame_validate_inherited(FoldFrame* frame, bool* is,
	String* NULLABLE(errors));

#endif /* FOLD_FRAME_VALIDATION */