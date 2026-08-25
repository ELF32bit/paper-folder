#ifndef FOLD_FILE_H
#define FOLD_FILE_H

#include "frame.h"

#define FOLD_VERSION 1.2
#define FOLD_FILE_CREATOR "Paper Folder"
#define FOLD_EXTENSIONS_NAMESPACE "pf:"
#define FOLD_COMPATIBILITY_EFA_EL 1

#define FOLD_FILE_CLASS_SINGLE_MODEL "singleModel"
#define FOLD_FILE_CLASS_MULTI_MODEL "multiModel"
#define FOLD_FILE_CLASS_ANIMATION "animation"
#define FOLD_FILE_CLASS_DIAGRAMS "diagrams"

typedef struct FoldFile {
	double version;
	String creator;
	String author;
	String title;
	String description;
	Array classes;
	Array frames;
} FoldFile;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

Error fold_file_create(FoldFile* file);

#define FoldFile_destroy fold_file_destroy
void fold_file_destroy(FoldFile* file);

Error fold_file_recreate(FoldFile* file);

/* ========================================================================= */
/* Serialization & Deserialization                                           */
/* ========================================================================= */

Error fold_file_from_json(FoldFile* file, void* JSON, void* Object);
Error fold_file_to_json(const FoldFile* file, void* JSON, void* Object);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

#define FoldFile_copy fold_file_copy
Error fold_file_copy(FoldFile* file, const FoldFile* source_file);

FoldFrame* fold_file_frame_inherit(FoldFile* file, usize index);
void fold_file_frames_inherit(FoldFile* file);

/* ========================================================================= */
/* Validation                                                                */
/* ========================================================================= */

Error fold_file_validate(FoldFile* file, bool* is, String* errors);

#endif /* FOLD_FILE_H */