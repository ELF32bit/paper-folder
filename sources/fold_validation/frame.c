#include "frame.h"
#include "graph.h"

static
Error fold_frame_metadata_validate(const FoldFrame* frame, bool* is, String* errors) {
	(void)frame; (void)errors;
	*is = true;
	return OK;
}

static
Error fold_frame_metadata_validate_inherited(FoldFrame* frame, bool* is, String* errors) {
	(void)frame; (void)errors;
	*is = true;
	return OK;
}

Error fold_frame_validate(const FoldFrame* frame, bool* is, String* errors) {
	*is = false;
	bool is_graph = true;
	bool is_metadata = true;
	TRY(fold_graph_validate(&frame->graph, &is_graph, errors));
	TRY(fold_frame_metadata_validate(frame, &is_metadata, errors));
	*is = (is_graph && is_metadata);
	return OK;
}

Error fold_frame_validate_inherited(FoldFrame* frame, bool* is, String* errors) {
	*is = false;
	bool is_graph = true;
	bool is_metadata = true;
	TRY(fold_graph_validate_inherited(&frame->graph, &is_graph, errors));
	TRY(fold_frame_metadata_validate_inherited(frame, &is_metadata, errors));
	*is = (is_graph && is_metadata);
	return OK;
}