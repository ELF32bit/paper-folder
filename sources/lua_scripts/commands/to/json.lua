local command = { name = "to-json", group = "Conversion Commands" }
local from_json = require("commands.from.json")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from FOLD file to JSON file")

	convert:argument("input"):args("?")
		:description("path to the FOLD source")
	convert:option("-o --output")
		:description("path to the JSON destination")

	convert:flag("-p --pretty")
		:description("beautify the JSON output")

	return convert
end

function command.fold_to_json_file(fold, args)
	return from_json._fold_to_json_file(fold, args)
end

function command.execute(args)
	return from_json.execute(args)
end

return command
