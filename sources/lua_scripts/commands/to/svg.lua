local command = { name = "to-svg", group = "Conversion To" }
local from_fold = require("commands.from.fold")
local validate = require("commands.validate")
local describe = require("commands.describe")

local COLORS = require("colors")
local BOLD = COLORS.BOLD
local RED = COLORS.RED
local RESET = COLORS.RESET

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from FOLD file to SVG file")
		:help_description_margin(40)

	convert:argument("input"):args("?")
		:description("path to the FOLD source")

	convert:group("Options",
		convert:option("-o --output"):argname("<file>")
		:description("path to the SVG destination"),

		convert:option("-f --frame"):argname("<int>")
		:description("FOLD frame")
		:convert(tonumber)
		:default("0")
		:show_default(true),

		convert:option("-s --scale"):argname("<float>")
		:description("scale coordinates")
		:convert(tonumber)
		:default("1.0")
		:show_default(true),

		convert:option("-S --seed"):argname("<int>")
		:description("seed for abstract graphs")
		:convert(tonumber)
		:default("0")
		:show_default(true)
		:target("abstract_seed")
	)

	convert:group("Vertices Options",
		convert:flag("--enable_vertices")
		:description("draw vertices circles"),

		convert:flag("--enable_vertices_indices")
		:description("write vertices array indices")
		:target("enable_vertices_index"),

		convert:flag("--disable_vertices_colors")
		:description("omit custom vertices colors")
		:target("disable_vertices_color"),

		convert:option("--vertices_scale"):argname("<float>")
		:description("scale vertices")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
	)

	convert:group("Edges Options",
		convert:flag("--disable_edges")
		:description("omit edges creases"),

		convert:flag("--enable_edges_indices")
		:description("write edges array indices")
		:target("enable_edges_index"),

		convert:flag("--enable_edges_fold_angles")
		:description("write edges fold angles")
		:target("enable_edges_fold_angle"),

		convert:flag("--enable_edges_lengths")
		:description("write edges lengths")
		:target("enable_edges_length"),

		convert:option("--edges_scale"):argname("<float>")
		:description("scale edges annotations")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("edges_annotations_scale")
	)

	convert:group("Crease Colors",
		convert:option("--B"):argname("<html>")
		:description("boundary crease color")
		:default("#000000ff")
		:show_default(true)
		:target("B_color"),

		convert:option("--M"):argname("<html>")
		:description("mountain crease color")
		:default("#ff0000ff")
		:show_default(true)
		:target("M_color"),

		convert:option("--V"):argname("<html>")
		:description("valley crease color")
		:default("#0000ffff")
		:show_default(true)
		:target("V_color"),

		convert:option("--F"):argname("<html>")
		:description("flat crease color")
		:default("#808080ff")
		:show_default(true)
		:target("F_color"),

		convert:option("--U"):argname("<html>")
		:description("unknown crease color")
		:default("#ffff00ff")
		:show_default(true)
		:target("U_color"),

		convert:option("--C"):argname("<html>")
		:description("cut crease color")
		:default("#00ff00ff")
		:show_default(true)
		:target("C_color"),

		convert:option("--J"):argname("<html>")
		:description("join crease color")
		:default("#00000040")
		:show_default(true)
		:target("J_color")
	)

	convert:group("Crease Scaling",
		convert:option("--Bs"):argname("<float>")
		:description("boundary crease scale")
		:convert(tonumber)
		:default("1.25")
		:show_default(true)
		:target("B_scale"),

		convert:option("--Ms"):argname("<float>")
		:description("mountain crease scale")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("M_scale"),

		convert:option("--Vs"):argname("<float>")
		:description("valley crease scale")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("V_scale"),

		convert:option("--Fs"):argname("<float>")
		:description("flat crease scale")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("F_scale"),

		convert:option("--Us"):argname("<float>")
		:description("unknown crease scale")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("U_scale"),

		convert:option("--Cs"):argname("<float>")
		:description("cut crease scale")
		:convert(tonumber)
		:default("1.25")
		:show_default(true)
		:target("C_scale"),

		convert:option("--Js"):argname("<float>")
		:description("join crease scale")
		:convert(tonumber)
		:default("0.75")
		:show_default(true)
		:target("J_scale")
	)

	convert:group("Crease Ordering",
		convert:option("--Bo"):argname("<int>")
		:description("boundary crease display order")
		:convert(tonumber)
		:default("60")
		:show_default(true)
		:target("B_order"),

		convert:option("--Mo"):argname("<int>")
		:description("mountain crease display order")
		:convert(tonumber)
		:default("50")
		:show_default(true)
		:target("M_order"),

		convert:option("--Vo"):argname("<int>")
		:description("valley crease display order")
		:convert(tonumber)
		:default("40")
		:show_default(true)
		:target("V_order"),

		convert:option("--Fo"):argname("<int>")
		:description("flat crease display order")
		:convert(tonumber)
		:default("30")
		:show_default(true)
		:target("F_order"),

		convert:option("--Uo"):argname("<int>")
		:description("unknown crease display order")
		:convert(tonumber)
		:default("20")
		:show_default(true)
		:target("U_order"),

		convert:option("--Co"):argname("<int>")
		:description("cut crease display order")
		:convert(tonumber)
		:default("70")
		:show_default(true)
		:target("C_order"),

		convert:option("--Jo"):argname("<int>")
		:description("join crease display order")
		:convert(tonumber)
		:default("10")
		:show_default(true)
		:target("J_order")
	)

	convert:group("Faces Options",
		convert:flag("--disable_faces")
		:description("omit faces polygons"),

		convert:flag("--enable_faces_indices")
		:description("write faces array indices")
		:target("enable_faces_index"),

		convert:flag("--enable_faces_materials")
		:description("write custom faces materials (HTML)")
		:target("enable_faces_material"),

		convert:option("--faces_scale"):argname("<float>")
		:description("scale faces annotations")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("faces_annotations_scale")
	)

	convert:group("Image Options",
		convert:option("-m --min_size"):argname("<px>")
		:description("min SVG size in pixels")
		:convert(tonumber)
		:default("512.0")
		:show_default(true)
		:target("image_min_size"),

		convert:option("-g --grow_size"):argname("<px>")
		:description("SVG grow size in pixels")
		:convert(tonumber)
		:default("64.0")
		:show_default(true)
		:target("image_grow_size"),

		convert:option("-w --stroke_width"):argname("<px>")
		:description("SVG stroke width pixels")
		:convert(tonumber)
		:default("1.0")
		:show_default(true),

		convert:option("-d --dash_scale"):argname("<float>")
		:description("SVG dash-array scale")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("stroke_dash_scale"),

		convert:flag("-p --printable")
		:description("SVG dash-array enable")
		:target("enable_stroke_dash")
	)

	return convert
end

function command.print_fold_frame_range_error(index, size)
	return describe.print_fold_frame_range_error(index, size)
end

function command.fold_to_svg_file(fold, args)
	local svg, svg_error =
		fold:frame_to_svg_file(args.output, args.frame,
			args.scale,
			args.abstract_seed,

			args.B_color,
			args.M_color,
			args.V_color,
			args.F_color,
			args.U_color,
			args.C_color,
			args.J_color,

			args.B_scale,
			args.M_scale,
			args.V_scale,
			args.F_scale,
			args.U_scale,
			args.C_scale,
			args.J_scale,

			args.B_order,
			args.M_order,
			args.V_order,
			args.F_order,
			args.U_order,
			args.C_order,
			args.J_order,

			args.image_min_size,
			args.image_grow_size,
			args.stroke_width,

			args.enable_stroke_dash,
			args.stroke_dash_scale,

			args.enable_vertices,
			args.enable_vertices_index,
			args.disable_vertices_color,
			args.vertices_scale,

			args.disable_edges,
			args.enable_edges_index,
			args.enable_edges_fold_angle,
			args.enable_edges_length,
			args.edges_annotations_scale,

			args.disable_faces,
			args.enable_faces_index,
			args.enable_faces_material,
			args.faces_annotations_scale)

	if svg == false then command.ERROR(svg_error) end
	return svg, svg_error
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)

	if type(args.frame) == "number" then
		local fold_frames_size = fold:get_frames_size()
		if args.frame < 0 or args.frame >= fold_frames_size then
			command.print_fold_frame_range_error(
				args.frame, fold_frames_size)
			return fold, fold_error
		end
	end

	local is_valid, _ =
		validate.validate_fold_file(fold, true)

	if not is_valid then
		validate.print_fold_validation_failure(args.input)
		return fold, fold_error
	end

	command.fold_to_svg_file(fold, args)
	return fold, fold_error
end

return command
