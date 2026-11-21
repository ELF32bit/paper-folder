local command = { name = "from-obj", group = "Conversion From Commands" }
local from_fold = require("commands.from.fold")
local to_fold = require("commands.to.fold")
local FoldFile = require("fold.file_C")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from OBJ file to FOLD file")

	convert:argument("input"):args("?")
		:description("path to the OBJ source")

	convert:group("Options",
		convert:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination"),

		convert:option("-s --scale"):argname("<float>")
		:description("scale coordinates")
		:convert(tonumber)
		:default("1.0")
		:show_default(true),

		convert:flag("-S --sort_objects")
		:description("sort frames by title")
	)

	convert:group("Compatibility Options",
		convert:flag("--disable_objects")
		:description("omit frame objects"),

		convert:flag("--disable_groups")
		:description("omit frame groups"),

		convert:flag("--disable_colors")
		:description("omit vertex R|G|B (72 bit)"),

		convert:flag("--enable_weights")
		:description("read vertex RGB (24 bit)"),

		convert:flag("--disable_textures")
		:description("omit texture coordinates"),

		convert:flag("--disable_normals")
		:description("omit normals"),

		convert:flag("--disable_materials")
		:description("omit materials"),

		convert:flag("--disable_lines")
		:description("omit lines"),

		convert:flag("--disable_faces")
		:description("omit faces")
	)

	return convert
end

function command.fold_from_obj_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end

	fold, fold_error =
		fold:from_obj_file(args.input,
			args.scale,
			args.sort_objects,
			args.disable_objects,
			args.disable_groups,
			args.disable_colors,
			args.enable_weights,
			args.disable_textures,
			args.disable_normals,
			args.disable_materials,
			args.disable_lines,
			args.disable_faces)

	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function command.fold_from_obj_string(obj, args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end

	fold, fold_error =
		fold:from_obj_string(obj,
			args.scale,
			args.sort_objects,
			args.disable_objects,
			args.disable_groups,
			args.disable_colors,
			args.enable_weights,
			args.disable_textures,
			args.disable_normals,
			args.disable_materials,
			args.disable_lines,
			args.disable_faces)

	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function command.read_stdin_stream()
	return from_fold.read_stdin_stream()
end

function command.__create_stdin_file(io_file)
	local tmp_path = os.tmpname()
	local tmp_file, tmp_error = io.open(tmp_path, "wb")
	if tmp_file == nil then error(tmp_error, 0) end

	local written, write_error = tmp_file:write(io_file)
	if not written then tmp_file:close() end
	if not written then error(write_error, 0) end

	local closed, close_error = tmp_file:close()
	if not closed then error(close_error, 0) end
	return tmp_path
end

function command.__fold_from_obj_stdin_file(io_file, args)
	args.input = command.__create_stdin_file(io_file)

	local success, fold, fold_error =
		pcall(command.fold_from_obj_file, args)
	os.remove(args.input)
	args.input = nil

	if success == false then error(fold, 0) end
	return fold, fold_error
end

function command.fold_from_input_args(args)
	if args.input == nil then
		local io_file, _ = command.read_stdin_stream()
		return command.fold_from_obj_string(io_file, args)
	else
		return command.fold_from_obj_file(args)
	end
end

function command.execute(args)
	local fold, fold_error =
		command.fold_from_input_args(args)
	to_fold.fold_to_fold_file(fold, args)
	return fold, fold_error
end

return command
