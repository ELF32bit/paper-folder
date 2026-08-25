#include "svg.h"

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#include <stdio.h>

/* ========================================================================= */
/* FOLD Graph Deserialization                                                */
/* ========================================================================= */

Error fold_graph_from_svg(FoldGraph* graph, void* SVG) {
	NSVGimage* image = SVG;
	(void)graph; (void)image;
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Deserialization                                                */
/* ========================================================================= */

Error fold_frame_from_svg(FoldFrame* frame, void* SVG) {
	NSVGimage* image = SVG;
	(void)frame; (void)image;
	return OK;
}

/* ========================================================================= */
/* FOLD File Deserialization                                                 */
/* ========================================================================= */

Error fold_file_from_svg(FoldFile* file, void* SVG) {
	NSVGimage* image = SVG;
	(void)file; (void)image;
	return OK;
}

/* ========================================================================= */
/* FOLD Graph Serialization                                                  */
/* ========================================================================= */

Error fold_graph_to_svg(const FoldGraph* graph, void* SVG) {
	FILE* output_file = SVG;
	(void)graph;
	return OK;
}

/* ========================================================================= */
/* FOLD Frame Serialization                                                  */
/* ========================================================================= */

Error fold_frame_to_svg(const FoldFrame* frame, void* SVG) {
	FILE* output_file = SVG;
	fprintf(output_file,
		"<svg "
		"width=\"500\" "
		"height=\"500\" "
		"xmlns=\"http://w3.org\" "
		"style=\"background-color: #f0f0f0;\""
		">\n");
	TRY(fold_graph_to_svg(&frame->graph, SVG));
	return OK;
}

/* ========================================================================= */
/* FOLD File Serialization                                                   */
/* ========================================================================= */

Error fold_file_to_svg(const FoldFile* file, void* SVG) {
	FILE* output_file = fopen((const char*)SVG, "w");
	if (output_file == NULL) return ERROR;

	FoldFrame* key_frame = array_get(&file->frames, 0);
	TRY_OR_ELSE(fold_frame_to_svg(key_frame, output_file),
		fclose(output_file));

	if (ferror(output_file) != 0) {
		fclose(output_file);
		return ERROR;
	}

	return (fclose(output_file) != 0)
		? ERROR
		: OK;
}