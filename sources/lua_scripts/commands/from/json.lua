local command = { name = "from-json", group = "Conversion Commands" }
local FoldFile = require("fold.file_C")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from JSON file to FOLD file")

	convert:argument("input"):args("?")
		:description("path to the JSON source")
	convert:option("-o --output")
		:description("path to the FOLD destination")

	convert:flag("-p --pretty")
		:description("beautify the FOLD output")

	return convert
end

function command.fold_from_json_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end
	fold, fold_error = fold:from_json_file(args.input)
	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function command.fold_from_json_string(json, args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end
	fold, fold_error = fold:from_json_string(json)
	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function command.fold_from_input_args(args)
	local fold, fold_error
	if args.input == nil then
		local io_json, io_error = io.read("*a")
		if io_json == nil then error(io_error, 0) end
		fold, fold_error =
			command.fold_from_json_string(io_json, args)
	else
		fold, fold_error =
			command.fold_from_json_file(args)
	end
	return fold, fold_error
end

function command._fold_to_json_file(fold, args)
	local json, json_error
	if args.output == nil then
		json, json_error = fold:to_json_string(args.pretty)
		if json == nil then error(json_error, 0) end
		print(json)
	else
		json, json_error = fold:to_json_file(args.output, args.pretty)
		if json == false then error(json_error, 0) end
	end
end

function command.execute(args)
	local fold, fold_error =
		command.fold_from_input_args(args)
	command._fold_to_json_file(fold, args)
	return fold, fold_error
end

return command
