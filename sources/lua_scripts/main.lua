local TEST_VALIDATE = require("tests.validate")

if #arg > 0 then
	TEST_VALIDATE.run_file(arg[1], arg[2] ~= nil)
else
	TEST_VALIDATE.run_all(false)
end
