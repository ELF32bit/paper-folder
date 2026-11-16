local command = { name = "from-obj", group = "Conversion From Commands" }
local to_fold = require("commands.to.fold")
local FoldFile = require("fold.file_C")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from OBJ file to FOLD file")

	convert:argument("input"):args("?")
		:description("path to the OBJ source")
	convert:option("-o --output")
		:description("path to the FOLD destination")

	convert:flag("-s --sort_objects")
		:description("sort frames by title")

	convert:flag("--disable_objects")
		:description("omit frame objects")

	convert:flag("--disable_groups")
		:description("omit frame groups")

	convert:flag("--disable_colors")
		:description("omit vertex R,G,B (72 bit)")

	convert:flag("--enable_weights")
		:description("read vertex RGB (24 bit)")

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

	return convert
end

function command.fold_from_obj_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end

	fold, fold_error =
		fold:from_obj_file(args.input,
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

function command.__create_stdin_file(stdin)
	local io_path = os.tmpname()
	local io_file, io_error = io.open(io_path, "wb")
	if io_file == nil then error(io_error, 0) end

	local written, write_error = io_file:write(stdin)
	if not written then io_file:close() end
	if not written then error(write_error, 0) end

	local closed, close_error = io_file:close()
	if not closed then error(close_error, 0) end
	return io_path
end

function command.fold_from_input_args(args)
	local fold, fold_error
	if args.input == nil then
		local io_obj, io_error = io.read("*a")
		if io_obj == nil then error(io_error, 0) end
		local io_path = command.__create_stdin_file(io_obj)

		local success
		args.input = io_path
		success, fold, fold_error = pcall(
			command.fold_from_obj_file, args)
		os.remove(io_path)
		args.input = nil

		if success == false then error(fold, 0) end
	else
		fold, fold_error =
			command.fold_from_obj_file(args)
	end
	return fold, fold_error
end

function command.execute(args)
	local fold, fold_error =
		command.fold_from_input_args(args)
	to_fold.fold_to_fold_file(fold, args)
	return fold, fold_error
end

return command
