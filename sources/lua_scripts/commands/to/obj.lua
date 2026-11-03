local command = { name = "to-obj", group = "Conversion Commands" }
local from_fold = require("commands.from.fold")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from FOLD file to OBJ file")

	convert:argument("input"):args("?")
		:description("path to the FOLD source")
	convert:option("-o --output")
		:description("path to the OBJ destination")

	convert:flag("-t --triangulate")
		:description("triangulate FOLD file")

	convert:flag("-c --colors")
		:description("store vertex colors")

	return convert
end

function command.fold_to_obj_file(fold, args)
	local obj, obj_error =
		fold:to_obj_file(args.output,
			args.triangulate, args.colors)
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
