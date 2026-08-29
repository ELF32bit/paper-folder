#include "frame.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

static
void fold_frame_metadata_create(FoldFrameMetadata* metadata) {
	metadata->is_simulated = false;
}

void fold_frame_create(FoldFrame* frame) {
	string_create(&frame->author);
	string_create(&frame->title);
	string_create(&frame->description);
	array_create(&frame->classes, SIZEOF(char, 32));
	array_create(&frame->attributes, SIZEOF(char, 32));
	string_create(&frame->unit);
	fold_graph_create(&frame->graph);
	frame->parent = FOLD_FRAME_PARENT_NONE;
	frame->inherit = false;
	fold_frame_metadata_create(&frame->metadata);
}

static
void fold_frame_metadata_destroy(FoldFrameMetadata* metadata) {
	metadata->is_simulated = false;
}

void fold_frame_destroy(FoldFrame* frame) {
	string_destroy(&frame->author);
	string_destroy(&frame->title);
	string_destroy(&frame->description);
	array_destroy(&frame->classes);
	array_destroy(&frame->attributes);
	string_destroy(&frame->unit);
	fold_graph_destroy(&frame->graph);
	fold_frame_metadata_destroy(&frame->metadata);
}

void fold_frame_recreate(FoldFrame* frame) {
	fold_frame_destroy(frame);
	fold_frame_create(frame);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static
Error fold_frame_metadata_copy(
	FoldFrameMetadata* metadata, const FoldFrameMetadata* source)
{
	metadata->is_simulated = source->is_simulated;
	return OK;
}

Error fold_frame_copy(FoldFrame* frame, const FoldFrame* source) {
	TRY(string_copy(&frame->author, &source->author));
	TRY(string_copy(&frame->title, &source->title));
	TRY(string_copy(&frame->description, &source->description));
	TRY(array_copy(&frame->classes, &source->classes));
	TRY(array_copy(&frame->attributes, &source->attributes));
	TRY(string_copy(&frame->unit, &source->unit));
	TRY(fold_graph_copy(&frame->graph, &source->graph));
	frame->parent = source->parent;
	frame->inherit = source->inherit;
	TRY(fold_frame_metadata_copy(&frame->metadata, &source->metadata));
	return OK;
}

static
void fold_frame_metadata_inherit(
	FoldFrameMetadata* metadata, const FoldFrameMetadata* source)
{
	metadata->is_simulated = source->is_simulated;
}

void fold_frame_inherit(FoldFrame* frame, const FoldFrame* source) {
	if (frame->author.is_view || frame->author.length == 0)
		string_view(&frame->author, &source->author);
	if (frame->title.is_view || frame->title.length == 0)
		string_view(&frame->title, &source->title);
	if (frame->description.is_view || frame->description.length == 0)
		string_view(&frame->description, &source->description);
	if (frame->classes.is_view || frame->classes.size == 0)
		array_view(&frame->classes, &source->classes);
	if (frame->attributes.is_view || frame->attributes.size == 0)
		array_view(&frame->attributes, &source->attributes);
	if (frame->unit.is_view || frame->unit.length == 0)
		string_view(&frame->unit, &source->unit);
	fold_graph_inherit(&frame->graph, &source->graph);
	fold_frame_metadata_inherit(&frame->metadata, &source->metadata);
}

static
bool fold_frame_metadata_is_inherited(const FoldFrameMetadata* metadata) {
	(void)metadata;
	return false;
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

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

Error fold_frame_attributes_assign(FoldFrame* frame) {
	if (frame->attributes.is_view) return OK;
	array_recreate(&frame->attributes);
	bool is;

	if NOT(frame->graph.VC.is_view) {
		if (fold_graph_is_abstract(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_ABSTRACT,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else if (fold_graph_is_2D(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_TWO_DIMENSIONAL,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else if (fold_graph_is_3D(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_THREE_DIMENSIONAL,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	if NOT(frame->graph.EF.is_view) {
		if (fold_graph_is_manifold(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_MANIFOLD,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NON_MANIFOLD,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	if NOT(frame->graph.FV.is_view) {
		TRY(fold_graph_is_orientable(&frame->graph, &is));
		if (is) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_ORIENTABLE,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NON_ORIENTABLE,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	if NOT(frame->graph.FV.is_view) {
		TRY(fold_graph_is_self_touching(&frame->graph, &is));
		if (is) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_SELF_TOUCHING,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NON_SELF_TOUCHING,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	if NOT(frame->graph.FV.is_view) {
		TRY(fold_graph_is_self_intersecting(&frame->graph, &is));
		if (is) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_SELF_INTERSECTING,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NON_SELF_INTERSECTING,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	if NOT(frame->graph.EA.is_view) {
		if (fold_graph_has_cuts(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_CUTS,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NO_CUTS,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
		if (fold_graph_has_joins(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_JOINS,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NO_JOINS,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	if NOT(frame->graph.FV.is_view) {
		if (fold_graph_has_concave_faces(&frame->graph)) {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_NON_CONVEX_FACES,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		} else {
			STRING_BUFFER_RAW(FOLD_FRAME_ATTRIBUTE_CONVEX_FACES,
				buffer, frame->attributes.element_size);
			TRY(array_append(&frame->attributes, &buffer));
		}
	}

	return OK;
}