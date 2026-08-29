local args = {}
local modules = {
	(require("commands.test")),
	(require("commands.convert")),
	(require("commands.validate")),
	(require("commands.help")),
}

local commands = {}
for _, module in ipairs(modules) do
	commands[module.name] = module
	for _, alias in ipairs(module.aliases) do
		commands[alias] = module
	end
end

function args.execute(arg)
	local name = arg[1]
	if type(name) ~= "string" then
		return commands.help.execute(arg,
			modules, nil)
	end
	local command = commands[name]
	if type(command) ~= "table" then
		return commands.help.execute(arg,
			modules, name)
	end
	command.execute(arg,
		modules, nil)
end

return args
