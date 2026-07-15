#include "frame.h"

static void fold_frame_metadata_create(FoldFrameMetadata* metadata) {
	metadata->is_simulated = false;
}

static void fold_frame_metadata_destroy(FoldFrameMetadata* metadata) {
	metadata->is_simulated = false;
}

static Error fold_frame_metadata_copy(FoldFrameMetadata* metadata, const FoldFrameMetadata* source_metadata) {
	metadata->is_simulated = source_metadata->is_simulated;
	return OK;
}

static void fold_frame_metadata_inherit(FoldFrameMetadata* metadata, const FoldFrameMetadata* source_metadata) {
	metadata->is_simulated = source_metadata->is_simulated;
}

static bool fold_frame_metadata_is_inherited(const FoldFrameMetadata* metadata) {
	(void)metadata;
	return false;
}

void fold_frame_create(FoldFrame* frame) {
	string_create(&frame->author);
	string_create(&frame->title);
	string_create(&frame->description);
	array_create_managed(&frame->classes, sizeof(String),
		(ArrayDestroyFunction)string_destroy,
		(ArrayCopyFunction)string_copy);
	array_create_managed(&frame->attributes, sizeof(String),
		(ArrayDestroyFunction)string_destroy,
		(ArrayCopyFunction)string_copy);
	string_create(&frame->unit);
	fold_graph_create(&frame->graph);
	frame->parent = FOLD_FRAME_PARENT_NONE;
	frame->inherit = false;
	fold_frame_metadata_create(&frame->metadata);
}

void fold_frame_destroy(FoldFrame* frame) {
	string_destroy(&frame->author);
	string_destroy(&frame->title);
	string_destroy(&frame->description);
	array_destroy(&frame->classes);
	array_destroy(&frame->attributes);
	string_destroy(&frame->unit);
	fold_graph_destroy(&frame->graph);
	frame->parent = FOLD_FRAME_PARENT_NONE;
	frame->inherit = false;
	fold_frame_metadata_destroy(&frame->metadata);
}

void fold_frame_recreate(FoldFrame* frame) {
	fold_frame_destroy(frame);
	fold_frame_create(frame);
}

Error fold_frame_copy(FoldFrame* frame, const FoldFrame* source_frame) {
	TRY(string_copy(&frame->author, &source_frame->author));
	TRY(string_copy(&frame->title, &source_frame->title));
	TRY(string_copy(&frame->description, &source_frame->description));
	TRY(array_copy(&frame->classes, &source_frame->classes));
	TRY(array_copy(&frame->attributes, &source_frame->attributes));
	TRY(string_copy(&frame->unit, &source_frame->unit));
	TRY(fold_graph_copy(&frame->graph, &source_frame->graph));
	frame->parent = source_frame->parent;
	frame->inherit = source_frame->inherit;
	TRY(fold_frame_metadata_copy(&frame->metadata, &source_frame->metadata));
	return OK;
}

void fold_frame_inherit(FoldFrame* frame, const FoldFrame* source_frame) {
	if (frame->author.length == 0) string_view(&frame->author, &source_frame->author);
	if (frame->title.length == 0) string_view(&frame->title, &source_frame->title);
	if (frame->description.length == 0) string_view(&frame->description, &source_frame->description);
	if (frame->classes.size == 0) array_view(&frame->classes, &source_frame->classes);
	if (frame->attributes.size == 0) array_view(&frame->attributes, &source_frame->attributes);
	if (frame->unit.length == 0) string_view(&frame->unit, &source_frame->unit);
	fold_graph_inherit(&frame->graph, &source_frame->graph);
	fold_frame_metadata_inherit(&frame->metadata, &source_frame->metadata);
}

bool fold_frame_is_inherited(const FoldFrame* frame) {
	if (frame->author.is_view) return true;
	if (frame->title.is_view) return true;
	if (frame->description.is_view) return true;
	if (frame->classes.is_view) return true;
	if (frame->attributes.is_view) return true;
	if (frame->unit.is_view) return true;
	if (fold_graph_is_inherited(&frame->graph)) return true;
	if (fold_frame_metadata_is_inherited(&frame->metadata)) return true;
	return false;
}