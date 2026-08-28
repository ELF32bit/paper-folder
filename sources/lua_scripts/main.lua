local TEST_FILES = require("tests.files")
local TEST_VALIDATE = require("tests.validate")

local HELP_MESSAGE = [[
usage: ./paper-folder [option] file
Available options are:
  -p, --print <file>     print a specific FOLD file
  -v, --validate <file>  validate a specific FOLD file
  -d, --dump <file>      dump a specific FOLD file as OBJ
  -h, --help             show this help message]]

if #arg > 0 then
	if arg[1] == "-h" or arg[1] == "--help" then
		print(HELP_MESSAGE)
	elseif arg[1] == "-p" or arg[1] == "--print" then
		TEST_FILES.print_fold_file(arg[2], true)
	elseif arg[1] == "-v" or arg[1] == "--validate" then
		TEST_VALIDATE.run_file(arg[2])
	elseif arg[1] == "-d" or arg[1] == "--dump" then
		TEST_FILES.dump_fold_file(arg[2])
	else
		io.stderr:write(string.format(
			"paper-folder: unrecognized option '%s'\n", arg[1]))
		print(HELP_MESSAGE)
		os.exit(1)
	end
else
	TEST_VALIDATE.run_all()
end
