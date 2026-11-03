#ifndef FOLD_VALIDATION_FRAME_H
#define FOLD_VALIDATION_FRAME_H

#include "fold_specification/frame.h"

Error fold_frame_validate(const FoldFrame* frame, bool* is,
	String* NULLABLE(errors));

Error fold_frame_validate_inherited(FoldFrame* frame, bool* is,
	String* NULLABLE(errors));

#endif /* FOLD_VALIDATION_FRAME_H */