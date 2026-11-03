local command = { name = "test-validate", group = "Other Commands" }
local validate = require("commands.validate")

function command.register(parser)
	local test = parser:command(command.name)
		:description("run validation tests")

	return test
end

function command.execute(args)
	local assets = require("assets")
	for _, file in ipairs(assets.FOLD_FILES) do
		args.input = file
		validate.execute(args)
	end
	args.input = nil
end

return command
