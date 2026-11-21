local command = { name = "from", group = "Conversion From Commands" }
local from_json = require("commands.from.json")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from FOLD file")

	convert:argument("input"):args("?")
		:description("path to the FOLD source")

	convert:group("Options",
		convert:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination"),

		convert:flag("-p --pretty")
		:description("beautify the FOLD output")
	)

	return convert
end

function command.fold_from_fold_file(args)
	return from_json.fold_from_json_file(args)
end

function command.fold_from_fold_string(json, args)
	return from_json.fold_from_json_string(json, args)
end

function command.read_stdin_stream()
	return from_json.read_stdin_stream()
end

function command.fold_from_input_args(args)
	return from_json.fold_from_input_args(args)
end

function command.execute(args)
	return from_json.execute(args)
end

return command
