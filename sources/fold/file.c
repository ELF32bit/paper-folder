#include "file.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

Error fold_file_create(FoldFile* file) {
	file->version = FOLD_VERSION;
	string_create(&file->creator);
	string_create(&file->author);
	string_create(&file->title);
	string_create(&file->description);
	array_create_managed(&file->classes, sizeof(String),
		(ArrayDestroyFunction)string_destroy,
		(ArrayCopyFunction)string_copy);
	array_create_managed(&file->frames, sizeof(FoldFrame),
		(ArrayDestroyFunction)fold_frame_destroy,
		(ArrayCopyFunction)fold_frame_copy);
	TRY(array_resize(&file->frames, 1));
	fold_frame_create(array_get(&file->frames, 0));
	return OK;
}

void fold_file_destroy(FoldFile* file) {
	string_destroy(&file->creator);
	string_destroy(&file->author);
	string_destroy(&file->title);
	string_destroy(&file->description);
	array_destroy(&file->classes);
	array_destroy(&file->frames);
}

Error fold_file_recreate(FoldFile* file) {
	fold_file_destroy(file);
	TRY(fold_file_create(file));
	return OK;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error fold_file_copy(FoldFile* file, const FoldFile* source) {
	file->version = source->version;
	TRY(string_copy(&file->creator, &source->creator));
	TRY(string_copy(&file->author, &source->author));
	TRY(string_copy(&file->title, &source->title));
	TRY(string_copy(&file->description, &source->description));
	TRY(array_copy(&file->classes, &source->classes));
	TRY(array_copy(&file->frames, &source->frames));
	return OK;
}

FoldFrame* fold_file_frame_inherit(FoldFile* file, usize index) {
	FoldFrame* frame = array_get(&file->frames, index);
	if NOT(frame->inherit) return frame;

	FoldFrame* parent = array_get(&file->frames, frame->parent);
	fold_frame_inherit(frame, parent);

	while (parent->inherit) {
		parent = array_get(&file->frames, parent->parent);
		fold_frame_inherit(frame, parent);
	}

	return frame;
}

void fold_file_frames_inherit(FoldFile* file) {
	FOR_EACH(i, file->frames.size) {
		fold_file_frame_inherit(file, i);
	}
}