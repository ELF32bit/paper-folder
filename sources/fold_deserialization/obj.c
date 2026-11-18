#include "obj.h"

#define TOBJ_USE_DOUBLE
#include "tiny_obj_c.h"

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "collections/map.h"

struct TinyObjectStreamingContext {
	const struct FoldFileFromOBJParameters* parameters;
	Map* unique_paint_colors;
	Map* unique_material_names;
	char active_edge_assignment;
	usize active_face_material;
	bool has_vertex_colors;
	bool has_texcoords;
	bool has_normals;
	FoldFile* file;
	Error error;
};

static
void _tiny_obj_object(void* context, const char* name) {
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_objects) return;
	if IS_ERROR(c->error) return;

	FOLD_FRAME_CREATE(frame);
	frame.inherit = true;
	frame.parent = 0;

	if (name != NULL) {
		c->error = string_copy_raw(&frame.title, name);
		if IS_ERROR(c->error) return fold_frame_destroy(&frame);
	}

	c->error = array_append(&c->file->frames, &frame);
	if IS_ERROR(c->error) return fold_frame_destroy(&frame);
}

static
void _tiny_obj_group(void* context,
	const char* const* names, int num_names)
{
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_groups) return;
	if IS_ERROR(c->error) return;
	if (num_names < 1 || names == NULL) return;
	_tiny_obj_object(context, names[0]);
}

static
void _tiny_obj_vertex(void* context,
	tobj_real x, tobj_real y, tobj_real z, tobj_real w)
{
	struct TinyObjectStreamingContext* c = context;
	if IS_ERROR(c->error) return;

	Vector3 vc;
	vc.x = REAL(x) * c->parameters->scale;
	vc.y = REAL(y) * c->parameters->scale;
	vc.z = REAL(z) * c->parameters->scale;

	FoldFrame* key_frame = fold_file_get_key_frame(c->file);
	c->error = array_append(&key_frame->graph.VC, &vc);
	if IS_ERROR(c->error) return;

	if (c->parameters->enable_weights) {
		f64 w24 = F64(CLAMP(w, 0.0, 1.0)) * F64(0xFFFFFF) + 0.5;
		Color pc24 = color_unpack((U32(w24) << 8) | 0xFF);
		c->has_vertex_colors = true;

		usize hash = USIZE(hash_color(pc24));
		if NOT(map_has(c->unique_paint_colors, &hash)) {
			c->error = map_add(c->unique_paint_colors, &hash,
				&c->unique_paint_colors->size);
			if IS_ERROR(c->error) return;

			c->error = array_append(&key_frame->graph.PC, &pc24);
			if IS_ERROR(c->error) return;
		}

		usize* vp = MAP_GET(c->unique_paint_colors, &hash);
		c->error = array_append(&key_frame->graph.VP, vp);
	}
}

static
void _tiny_obj_vertex_color(void* context,
	tobj_real x, tobj_real y, tobj_real z,
	tobj_real r, tobj_real g, tobj_real b, bool has_color)
{
	struct TinyObjectStreamingContext* c = context;
	if IS_ERROR(c->error) return;

	Vector3 vc;
	vc.x = REAL(x) * c->parameters->scale;
	vc.y = REAL(y) * c->parameters->scale;
	vc.z = REAL(z) * c->parameters->scale;

	if (has_color) c->has_vertex_colors = true;
	FoldFrame* key_frame = fold_file_get_key_frame(c->file);
	c->error = array_append(&key_frame->graph.VC, &vc);
	if IS_ERROR(c->error) return;

	Color pc72 = color_from_components(
		has_color ? F32(CLAMP(r, 0.0, 1.0)) : 1.0,
		has_color ? F32(CLAMP(g, 0.0, 1.0)) : 1.0,
		has_color ? F32(CLAMP(b, 0.0, 1.0)) : 1.0,
		1.0);

	usize hash = USIZE(hash_color(pc72));
	if NOT(map_has(c->unique_paint_colors, &hash)) {
		c->error = map_add(c->unique_paint_colors, &hash,
			&c->unique_paint_colors->size);
		if IS_ERROR(c->error) return;

		c->error = array_append(&key_frame->graph.PC, &pc72);
		if IS_ERROR(c->error) return;
	}

	usize* vp = MAP_GET(c->unique_paint_colors, &hash);
	c->error = array_append(&key_frame->graph.VP, vp);
}

static
void _tiny_obj_texcoord(void* context,
	tobj_real x, tobj_real y, tobj_real z)
{
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_textures) return;
	if IS_ERROR(c->error) return;
	c->has_texcoords = true;

	Vector2 vt;
	vt.x = REAL(x);
	vt.y = REAL(y);
	(void)z;

	FoldFrame* key_frame = fold_file_get_key_frame(c->file);
	c->error = array_append(&key_frame->graph.TC, &vt);
}

static
void _tiny_obj_normal(void* context,
	tobj_real x, tobj_real y, tobj_real z)
{
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_normals) return;
	if IS_ERROR(c->error) return;
	c->has_normals = true;

	Vector3 vn;
	vn.x = REAL(x);
	vn.y = REAL(y);
	vn.z = REAL(z);

	FoldFrame* key_frame = fold_file_get_key_frame(c->file);
	c->error = array_append(&key_frame->graph.NC, &vn);
}

static
void _tiny_obj_usemtl(void* context,
	const char* name, int material_id)
{
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_materials) return;
	if IS_ERROR(c->error) return;
	(void)material_id;

	if (name == NULL || name[0] == '\0') {
		c->active_edge_assignment = '\0';
		c->active_face_material = FOLD_GRAPH_NULL;
		return;
	}

	usize hash = hash_string_raw(name);
	if NOT(map_has(c->unique_material_names, &hash)) {
		c->error = map_add(c->unique_material_names, &hash,
			&c->unique_material_names->size);
		if IS_ERROR(c->error) return;

		FoldFrame* key_frame = fold_file_get_key_frame(c->file);
		STRING_BUFFER_RAW(name, buffer, key_frame->graph.MN.element_size);
		c->error = array_append(&key_frame->graph.MN, buffer);
		if IS_ERROR(c->error) return;
	}

	usize* index = MAP_GET(c->unique_material_names, &hash);
	c->active_edge_assignment = name[0];
	c->active_face_material = *index;
}

static
void _tiny_obj_line(void* context,
	const tobj_index* indices, int num_indices)
{
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_lines) return;
	if IS_ERROR(c->error) return;
	if (num_indices < 2) return;
	if (indices == NULL) return;
	FOR_EACH_IN_RANGE(i, 0, USIZE(num_indices)) {
		if (indices[i].vertex_index < 0) return;
	}

	FoldGraphEdgeAssignment assignment =
		FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(c->active_edge_assignment)
			? (FoldGraphEdgeAssignment)c->active_edge_assignment
			: FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN;

	FoldFrame* frame = array_get_end(&c->file->frames);
	FOR_EACH_IN_RANGE(i, 0, USIZE(num_indices) - 1) {
		FoldGraphEdge edge;
		edge.a = USIZE(indices[i + 0].vertex_index);
		edge.b = USIZE(indices[i + 1].vertex_index);

		c->error = array_append(&frame->graph.EV, &edge);
		if IS_ERROR(c->error) return;

		if NOT(c->parameters->disable_materials) {
			c->error = array_append(&frame->graph.EA, &assignment);
			if IS_ERROR(c->error) return;
		}
	}
}

static
void _tiny_obj_index(void* context,
	const tobj_index* indices, int num_indices)
{
	struct TinyObjectStreamingContext* c = context;
	if (c->parameters->disable_faces) return;
	if IS_ERROR(c->error) return;
	if (num_indices < 3) return;
	if (indices == NULL) return;
	FOR_EACH(i, USIZE(num_indices)) {
		if (indices[i].vertex_index < 0) return;
	}

	FoldFrame* frame = array_get_end(&c->file->frames);
	FoldGraph* graph = &frame->graph;

	FOR_EACH(i, USIZE(num_indices)) {
		usize fvi = USIZE(indices[i].vertex_index);
		c->error = array_append(&graph->FV.data, &fvi);
		if IS_ERROR(c->error) return;

		if NOT(c->parameters->disable_textures) {
			usize fvt = (indices[i].texcoord_index >= 0)
				? USIZE(indices[i].texcoord_index) : FOLD_GRAPH_NULL;
			c->error = array_append(&graph->FT.data, &fvt);
			if IS_ERROR(c->error) return;
		}

		if NOT(c->parameters->disable_normals) {
			usize fvn = (indices[i].normal_index >= 0)
				? USIZE(indices[i].normal_index) : FOLD_GRAPH_NULL;
			c->error = array_append(&graph->FN.data, &fvn);
			if IS_ERROR(c->error) return;
		}
	}

	usize offset = graph->FV.data.size;
	c->error = array_append(&graph->FV.offsets, &offset);
	if IS_ERROR(c->error) return;

	if NOT(c->parameters->disable_textures) {
		c->error = array_append(&graph->FT.offsets, &offset);
		if IS_ERROR(c->error) return;
	}

	if NOT(c->parameters->disable_normals) {
		c->error = array_append(&graph->FN.offsets, &offset);
		if IS_ERROR(c->error) return;
	}

	if NOT(c->parameters->disable_materials) {
		c->error = array_append(&graph->FM, &c->active_face_material);
	}
}

/* ========================================================================= */
/* FOLD File                                                                 */
/* ========================================================================= */

static inline
int _fold_frame_sort_ascending(const void* a, const void* b) {
	const String* s1 = &((const FoldFrame*)a)->title;
	const String* s2 = &((const FoldFrame*)b)->title;
	return string_sort_ascending(s1, s2);
}

Error fold_file_from_obj(FoldFile* file, void* OBJ,
	const struct FoldFileFromOBJParameters* parameters)
{
	String* input_string = OBJ;
	TRY(fold_file_recreate(file));

	MAP_CREATE(unique_paint_colors, usize, usize);
	unique_paint_colors.hash = usize_hash_identity;

	MAP_CREATE(unique_material_names, usize, usize);
	unique_material_names.hash = usize_hash_identity;

	struct TinyObjectStreamingContext context;
	context.parameters = parameters;
	context.unique_paint_colors = &unique_paint_colors;
	context.unique_material_names = &unique_material_names;
	context.active_edge_assignment = '\0';
	context.active_face_material = FOLD_GRAPH_NULL;
	context.has_vertex_colors = false;
	context.has_texcoords = false;
	context.has_normals = false;
	context.file = file;
	context.error = OK;

	tobj_load_config configuration = tobj_default_config();
	configuration.triangulate = false;

	tobj_callbacks callbacks;
	callbacks.user_data = &context;
	callbacks.object_cb = _tiny_obj_object;
	callbacks.group_cb = _tiny_obj_group;

	if (parameters->disable_colors || parameters->enable_weights) {
		callbacks.vertex_cb = _tiny_obj_vertex;
		callbacks.vertex_color_cb = NULL;
	} else {
		callbacks.vertex_cb = NULL;
		callbacks.vertex_color_cb = _tiny_obj_vertex_color;
	}

	callbacks.texcoord_cb = _tiny_obj_texcoord;
	callbacks.normal_cb = _tiny_obj_normal;
	callbacks.index_cb = _tiny_obj_index;
	callbacks.usemtl_cb = _tiny_obj_usemtl;
	callbacks.line_cb = _tiny_obj_line;
	callbacks.mtllib_cb = NULL;
	callbacks.point_cb = NULL;

	tobj_result result = tobj_load_obj_with_callbacks(
		(const u8*)input_string->data, input_string->length,
			&callbacks, &configuration, NULL);

	map_destroy(context.unique_paint_colors);
	map_destroy(context.unique_material_names);
	if IS_ERROR(context.error) return context.error;
	if (result != TOBJ_OK) return ERROR;

	FoldFrame* key_frame = fold_file_get_key_frame(file);
	if NOT(context.has_vertex_colors) {
		array_recreate(&key_frame->graph.PC);
		ARRAY_FOR_EACH(&file->frames, _, FoldFrame*, frame) {
			array_recreate(&frame->graph.VP);
		}
	}
	if NOT(context.has_texcoords) {
		array_recreate(&key_frame->graph.TC);
		ARRAY_FOR_EACH(&file->frames, _, FoldFrame*, frame) {
			array2_recreate(&frame->graph.FT);
		}
	}
	if NOT(context.has_normals) {
		array_recreate(&key_frame->graph.NC);
		ARRAY_FOR_EACH(&file->frames, _, FoldFrame*, frame) {
			array2_recreate(&frame->graph.FN);
		}
	}

	if (parameters->sort_objects) {
		if (file->frames.size > 1) {
			array_sort_range(&file->frames, 1, file->frames.size,
				_fold_frame_sort_ascending);
		}
	}

	FOR_EACH_IN_RANGE(index, 1, file->frames.size) {
		fold_file_frame_inherit(file, index);
	}

	return OK;
}