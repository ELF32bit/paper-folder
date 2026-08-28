#include "svg.h"

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#include <stdio.h>

/* ========================================================================= */
/* FOLD Graph Deserialization                                                */
/* ========================================================================= */

static inline
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

static inline
Error fold_graph_to_svg(FoldGraph* graph, void* SVG) {
	FILE* output_file = SVG;

	AABB2 aabb2 = fold_graph_get_aabb2(graph);
	Vector2 size = aabb2_size(aabb2);
	fprintf(output_file,
		"<svg "
		"width=\"%f\" "
		"height=\"%f\" "
		"xmlns=\"http://w3.org\" "
		"style=\"background-color: #f0f0f0;\""
		">\n", (float)size.x, (float)size.y);

	return OK;
}

/* ========================================================================= */
/* FOLD Frame Serialization                                                  */
/* ========================================================================= */

Error fold_frame_to_svg(FoldFrame* frame, void* SVG) {
	FILE* output_file = (SVG != NULL)
		? fopen((const char*)SVG, "w")
		: stdout;

	TRY_NEW_MEMORY_OR_ERROR(output_file,
		return ERROR);

	TRY_OR_ELSE(fold_graph_to_svg(&frame->graph, output_file),
		fclose(output_file));

	TRY_OR_ERROR(ferror(output_file),
		fclose(output_file);
		return ERROR);

	return IS_ERROR(fclose(output_file))
		? ERROR : OK;
}

/* ========================================================================= */
/* FOLD File Serialization                                                   */
/* ========================================================================= */

Error fold_file_to_svg(FoldFile* file, void* SVG) {
	FoldFrame* key_frame = array_get(&file->frames, 0);
	TRY(fold_frame_to_svg(key_frame, SVG));
	return OK;
}