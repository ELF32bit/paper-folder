local command = { name = "convert-fold", group = "Conversion Commands" }
local convert_from = require("commands.convert.from")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from any supported file to FOLD file")

	convert:usage_max_width(80)

	convert:argument("input_file")
		:description("path to the convertible source")
	convert:argument("output_file")
		:description("optional path to the destination")
		:args("?")

	convert:flag("-p --pretty")
		:description("beautify the FOLD output")

	return convert
end

function command.convert_fold_to_json(fold, args)
	local json, json_error
	if args.output_file == nil then
		json, json_error = fold:to_json_string(args.pretty)
		if json == nil then error(json_error, 0) end
		print(json)
	else
		json, json_error = fold:to_json_file(args.output_file, args.pretty)
		if json == false then error(json_error, 0) end
	end
end

function command.execute(args)
	local fold, _ = convert_from.execute(args)
	command.convert_fold_to_json(fold, args)
end

return command
