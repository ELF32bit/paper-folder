local command = { name = "test", group = "Other Commands" }

function command.register(parser)
	local test = parser:command(command.name)
		:description("run tests")

	return test
end

local function validate_fold_files(args)
	local assets = require("assets")
	local validate = require("commands.validate")
	for _, file in ipairs(assets.FOLD_FILES) do
		validate.print_fold_validation_errors(file,
			validate.validate_fold_file(file))
	end
end

function command.execute(args)
	validate_fold_files(args)
end

return command
