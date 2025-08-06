
#define CLOG_IMPLEMENTATION
#include "clogger.h"

int main(void)
{
	clog_info("This is a log.");
	clog_debug("This is a debug.");
	clog_warn("This is a warning.");
	clog_error("This is an error.");
	clog_info("This is a %s log.", "formatted");
	clog_debug("This is a %s debug.", "formatted");
	clog_warn("This is a %s warning.", "formatted");
	clog_error("This is a %s error.", "formatted");
	return 0;
}

