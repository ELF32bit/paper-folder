local command = { name = "convert-obj", group = "Conversion Commands" }
local convert_from = require("commands.convert.from")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from any supported file to OBJ file")

	convert:usage_max_width(80)

	convert:argument("input_file")
		:description("path to the convertible source")
	convert:argument("output_file")
		:description("optional path to the destination")
		:args("?")

	return convert
end

function command.convert_fold_to_obj(fold, args)
	local obj, obj_error = fold:to_obj_file(args.output_file)
	if obj == false then error(obj_error, 0) end
end

function command.execute(args)
	local fold, _ = convert_from.execute(args)
	command.convert_fold_to_obj(fold, args)
end

return command
