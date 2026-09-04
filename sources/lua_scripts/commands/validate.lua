local command = { name = "validate", group = "Other Commands" }
local FoldFile = require("fold.file_C")

local COLORS = require("colors")
local BOLD = COLORS.BOLD
local RED = COLORS.RED
local GREEN = COLORS.GREEN
local YELLOW = COLORS.YELLOW
local WHITE = COLORS.WHITE
local GRAY = COLORS.GRAY
local RESET = COLORS.RESET

function command.register(parser)
	local validate = parser:command(command.name)
		:description("validate a specific FOLD file")

	validate:argument("file")
		:description("path to the FOLD source")

	return validate
end

function command.print_fold_validation_errors(file, is_valid, errors)
	if is_valid then
		print(('%s%sVALID%s: %s%s"%s"%s'):format(
			BOLD, GREEN, RESET, BOLD, WHITE, file, RESET))
		return
	end

	print(('%s%sINVALID%s: %s%s"%s"%s'):format(
		BOLD, RED, RESET, BOLD, WHITE, file, RESET))

	local frame = 0
	for line in errors:gmatch("[^\n]+") do
		if line:sub(1, 7) == "FRAME: " then
			frame = line:sub(8)
		elseif line:sub(1, 7) == "ERROR: " then
			print(("%s%sframe:%d%s: %s%s%s"):format(
				BOLD, GRAY, frame, RESET,
				RED, line:sub(8), RESET))
		elseif line:sub(1, 9) == "WARNING: " then
			print(("%s%sframe:%d%s: %s%s%s"):format(
				BOLD, GRAY, frame, RESET,
				YELLOW, line:sub(10), RESET))
		end
	end
end

function command.validate_fold_file(file)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end
	fold, fold_error = fold:from_json_file(file)
	if fold == nil then error(fold_error, 0) end
	return fold:validate()
end

function command.execute(args)
	local is_valid, errors = command.validate_fold_file(args.file)
	command.print_fold_validation_errors(args.file, is_valid, errors)
end

return command
