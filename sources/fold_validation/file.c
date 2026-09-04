#include "file.h"
#include "frame.h"

#include "collections/set.h"
#include <stdio.h>

#define MAX_FRAME_PARENTS 64;

static inline
Error _validate_frame_parents(const FoldFile* file, usize index, bool* is) {
	*is = false;
	FoldFrame* frame = array_get(&file->frames, index);
	if (frame->parent == FOLD_FRAME_PARENT_NONE ||
		frame->parent == index ||
		NOT(frame->inherit)) {
		*is = true;
		return OK;
	} else if (frame->parent >= file->frames.size) {
		return OK;
	}

	usize parent_index = frame->parent;
	if (parent_index >= file->frames.size) return OK;
	FoldFrame* parent = array_get(&file->frames, parent_index);
	if (parent->parent == FOLD_FRAME_PARENT_NONE ||
		parent->parent == parent_index ||
		NOT(parent->inherit)) {
		*is = true;
		return OK;
	} else if (parent->parent == index) {
		*is = false;
		return OK;
	}

	SET_CREATE(parents, usize);
	TRY_OR_ELSE(set_add(&parents, &index, NULL),
		set_destroy(&parents));
	TRY_OR_ELSE(set_add(&parents, &parent_index, NULL),
		set_destroy(&parents));

	usize max_parents = MAX_FRAME_PARENTS;
	while (max_parents > 1) {
		parent_index = parent->parent;
		if (parent_index >= file->frames.size) {
			set_destroy(&parents);
			return OK;
		}

		parent = array_get(&file->frames, parent_index);
		if (parent->parent == FOLD_FRAME_PARENT_NONE ||
			parent->parent == parent_index ||
			NOT(parent->inherit)) {
			set_destroy(&parents);
			*is = true;
			return OK;
		} else if (set_has(&parents, &parent_index)) {
			set_destroy(&parents);
			return OK;
		}

		TRY_OR_ELSE(set_add(&parents, &parent_index, NULL),
			set_destroy(&parents));
		max_parents--;
	}

	set_destroy(&parents);
	return OK;
}

Error fold_file_validate(FoldFile* file, bool* is, String* errors) {
	*is = false;
	bool are_frames = true;
	FOR_EACH(index, file->frames.size) {
		bool frame_parent_is = true;
		TRY(_validate_frame_parents(file, index,
			&frame_parent_is));

		if NOT(frame_parent_is) {
			if (errors != NULL) {
				char buffer[64];
				snprintf(buffer, sizeof(buffer),
					"ERROR: frame %zu has bad parents\n", index);
				TRY(string_append_raw(errors, buffer));
			}
			are_frames = false;
		}
	}
	if NOT(are_frames) return OK;

	fold_file_frames_inherit(file);
	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		STRING_CREATE(frame_errors);
		bool is_frame = true;

		TRY_OR_ELSE(fold_frame_validate(frame,
			&is_frame, &frame_errors),
			string_destroy(&frame_errors));

		if NOT(is_frame) {
			if (errors != NULL) {
				char buffer[32];
				snprintf(buffer, sizeof(buffer),
					"FRAME: %zu\n", index);

				TRY_OR_ELSE(string_append_raw(errors, buffer),
					string_destroy(&frame_errors));

				TRY_OR_ELSE(string_append(errors, &frame_errors),
					string_destroy(&frame_errors));
			}
			are_frames = false;
		}

		string_destroy(&frame_errors);
	}

	*is = are_frames;
	return OK;
}