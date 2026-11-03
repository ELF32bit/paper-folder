local command = { name = "validate", group = "Inspection Commands" }
local from_fold = require("commands.from.fold")

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
		:description("validate FOLD file")

	validate:argument("input"):args("?")
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

function command.validate_fold_file(fold, silent)
	return fold:validate(silent)
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)

	local is_valid, errors =
		command.validate_fold_file(fold, false)

	command.print_fold_validation_errors(args.input
		or "STDIN", is_valid, errors)

	return fold, fold_error
end

return command
