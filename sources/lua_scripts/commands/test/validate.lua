local command = { name = "test-validate", group = "Other Commands" }
local validate = require("commands.validate")

function command.register(parser)
	local test = parser:command(command.name)
		:description("run describe tests")

	return test
end

function command.execute(args)
	local assets = require("assets")
	for _, file in ipairs(assets.FOLD_FILES) do
		validate.execute({ input = file })
	end
end

return command
