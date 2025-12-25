
# Clogger

Clogger is a logging library for C with flexible user customizability.

# Usage
Simply paste the `clogger.h` file located in `./lib` in your project, include it wherever you need and define the `CLOG_IMPLEMENTATION` macro.

## Example
```c
#define CLOG_IMPLEMENTATION
#include "clogger.h"

int main(void)
{
	clog_info("This is a log.");
	clog_debug("This is a debug.");
	clog_warn("This is a warning.");
	clog_error("This is an error.");
	return 0;
}
```
You can find more examples in the `./examples` directory.

