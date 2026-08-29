#ifndef FOLD_FRAME_H
#define FOLD_FRAME_H

#include "graph.h"

#define FOLD_FRAME_PARENT_NONE USIZE_MAX

#define FOLD_FRAME_CLASS_CREASE_PATTERN "creasePattern"
#define FOLD_FRAME_CLASS_FOLDED_FORM "foldedForm"
#define FOLD_FRAME_CLASS_GRAPH "graph"
#define FOLD_FRAME_CLASS_LINKAGE "linkage"

#define FOLD_FRAME_ATTRIBUTE_ABSTRACT "abstract"
#define FOLD_FRAME_ATTRIBUTE_TWO_DIMENSIONAL "2D"
#define FOLD_FRAME_ATTRIBUTE_THREE_DIMENSIONAL "3D"
#define FOLD_FRAME_ATTRIBUTE_MANIFOLD "manifold"
#define FOLD_FRAME_ATTRIBUTE_NON_MANIFOLD "nonManifold"
#define FOLD_FRAME_ATTRIBUTE_ORIENTABLE "orientable"
#define FOLD_FRAME_ATTRIBUTE_NON_ORIENTABLE "nonOrientable"
#define FOLD_FRAME_ATTRIBUTE_SELF_TOUCHING "selfTouching"
#define FOLD_FRAME_ATTRIBUTE_NON_SELF_TOUCHING "nonSelfTouching"
#define FOLD_FRAME_ATTRIBUTE_SELF_INTERSECTING "selfIntersecting"
#define FOLD_FRAME_ATTRIBUTE_NON_SELF_INTERSECTING "nonSelfIntersecting"
#define FOLD_FRAME_ATTRIBUTE_CUTS "cuts"
#define FOLD_FRAME_ATTRIBUTE_NO_CUTS "noCuts"
#define FOLD_FRAME_ATTRIBUTE_JOINS "joins"
#define FOLD_FRAME_ATTRIBUTE_NO_JOINS "noJoins"
#define FOLD_FRAME_ATTRIBUTE_CONVEX_FACES "convexFaces"
#define FOLD_FRAME_ATTRIBUTE_NON_CONVEX_FACES "nonConvexFaces"

#define FOLD_FRAME_UNIT_NONE "unit"
#define FOLD_FRAME_UNIT_INCH "in"
#define FOLD_FRAME_UNIT_POST_SCRIPT_POINTS "pt"
#define FOLD_FRAME_UNIT_METERS "m"
#define FOLD_FRAME_UNIT_CENTIMETERS "cm"
#define FOLD_FRAME_UNIT_MILLIMETERS "mm"
#define FOLD_FRAME_UNIT_MICRONS "um"
#define FOLD_FRAME_UNIT_NANOMETERS "nm"

typedef struct FoldFrameMetadata {
	bool is_simulated;
} FoldFrameMetadata;

typedef struct FoldFrame {
	String author;
	String title;
	String description;
	Array classes;
	Array attributes;
	String unit;
	FoldGraph graph;
	usize parent;
	bool inherit;
	union {
		FoldFrameMetadata metadata;
		struct {
			bool is_simulated;
		};
	};
} FoldFrame;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void fold_frame_create(FoldFrame* frame);

#define FOLD_FRAME_CREATE(frame) \
	FoldFrame frame; \
	fold_frame_create(&frame)

#define FoldFrame_destroy fold_frame_destroy
void fold_frame_destroy(FoldFrame* frame);

void fold_frame_recreate(FoldFrame* frame);

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

Error fold_frame_from_json(FoldFrame* frame, void* JSON, void* Object);
Error fold_frame_to_json(const FoldFrame* frame, void* JSON, void* Object);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

#define FoldFrame_copy fold_frame_copy
Error fold_frame_copy(FoldFrame* frame, const FoldFrame* source);

void fold_frame_inherit(FoldFrame* frame, const FoldFrame* source);
bool fold_frame_is_inherited(const FoldFrame* frame);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

Error fold_frame_validate(const FoldFrame* frame, bool* is, String* errors);
Error fold_frame_validate_inherited(FoldFrame* frame, bool* is, String* errors);

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

Error fold_frame_attributes_assign(FoldFrame* frame);

#endif /* FOLD_FRAME_H */