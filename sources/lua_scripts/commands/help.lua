local command = {}

command.name = "help"
command.aliases = { "-h", "--help" }
command.description = "show this help message"
command.help_message = [[
usage: paper-folder <command> [options]
Available commands are:]]

function command.execute(arg, modules, unknown)
	if type(unknown) == "string" then
		print(("paper-folder: unrecognized command '%s'")
			:format(unknown))
	end
	print(command.help_message)
	for _, module in ipairs(modules) do
		print(("  %-10s%s"):format(
			module.name, module.description))
	end
end

return command
