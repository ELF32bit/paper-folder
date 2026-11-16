local command = { name = "to-obj", group = "Conversion To Commands" }
local from_fold = require("commands.from.fold")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from FOLD file to OBJ file")

	convert:argument("input"):args("?")
		:description("path to the FOLD source")
	convert:option("-o --output")
		:description("path to the OBJ destination")

	convert:flag("--enable_groups")
		:description("write frame groups")

	convert:flag("--disable_colors")
		:description("omit vertex R,G,B (72 bit)")

	convert:flag("--enable_weights")
		:description("write vertex RGB (24 bit)")

	convert:flag("--disable_textures")
		:description("omit texture coordinates")

	convert:flag("--disable_normals")
		:description("omit normals")

	convert:flag("--disable_materials")
		:description("omit materials")

	convert:flag("--disable_lines")
		:description("omit lines")

	convert:flag("--disable_faces")
		:description("omit faces")

	convert:option("-s"):argname("SEED")
		:target("abstract_seed")
		:description("seed for abstract graphs")
		:convert(tonumber)
		:default("0")
		:show_default(true)

	convert:option("-S"):argname("UNITS")
		:target("abstract_size")
		:description("max abstract graph size")
		:convert(tonumber)
		:default("100.0")
		:show_default(true)

	return convert
end

function command.fold_to_obj_file(fold, args)
	local obj, obj_error =
		fold:to_obj_file(args.output,
			args.enable_groups,
			args.disable_colors,
			args.enable_weights,
			args.disable_textures,
			args.disable_normals,
			args.disable_materials,
			args.disable_lines,
			args.disable_faces,
			args.abstract_seed,
			args.abstract_size)

	if obj == false then error(obj_error, 0) end
	return obj, obj_error
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)
	command.fold_to_obj_file(fold, args)
	return fold, fold_error
end

return command
