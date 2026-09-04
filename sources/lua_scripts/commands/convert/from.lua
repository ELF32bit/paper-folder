local module = {}
local FoldFile = require("fold.file_C")

function module.get_file_extension(file)
	local extension = file:match("%.([^.]+)$")
	return extension and extension:lower() or "a file with no extension"
end

function module.fold_from_json_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end
	fold, fold_error = fold:from_json_file(args.input_file)
	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function module.fold_from_fold_file(args)
	return module.fold_from_json_file(args)
end

function module.fold_from_svg_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end
	fold, fold_error = fold:frame_from_svg_file(args.input_file, args.frame)
	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function module.fold_from_obj_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end
	fold, fold_error = fold:from_obj_file(args.input_file)
	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function module.execute(args)
	local from = module.get_file_extension(args.input_file)
	local converter = module[("fold_from_%s_file"):format(from)]
	if converter ~= nil then return converter(args) end
	error(("converting from %s is not supported"):format(from), 0)
end

function module.execute_without_converting(args)
	local from = module.get_file_extension(args.input_file)
	local converter = module[("fold_from_%s_file"):format(from)]
	if converter ~= nil then return end
	error(("converting from %s is not supported"):format(from), 0)
end

return module
