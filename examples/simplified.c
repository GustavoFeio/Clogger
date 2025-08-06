
#define CLOG_IMPLEMENTATION
#define CLOG_SUPPRESS_TIME
#define CLOG_SUPPRESS_NEWLINE
#include "clogger.h"

int main(void)
{
	clog_info("This is a log.\n");
	clog_debug("This is a debug.\n");
	clog_warn("This is a warning.\n");
	clog_error("This is an error.\n");
	clog_info("This is a %s log.\n", "formatted");
	clog_debug("This is a %s debug.\n", "formatted");
	clog_warn("This is a %s warning.\n", "formatted");
	clog_error("This is a %s error.\n", "formatted");
	clog_info("Note the time of the log has been suppressed.\n");
	clog_info("If you prefer the explicit '\\n' à la printf you can just suppress that behavior and add it yourself.\n");
	return 0;
}


