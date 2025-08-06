
#define CLOG_DEBUG_COLOR "\x1b[1;34m"
#define CLOG_INFO_COLOR "\x1b[1;32m"
#define CLOG_INFO_OUT stdout
#define CLOG_DEBUG_OUT stdout
#define CLOG_IMPLEMENTATION
#include "clogger.h"

int main(void)
{
	clog_info("This is a green log.");
	clog_debug("This is a blue debug.");
	clog_warn("This is a warning.");
	clog_error("This is an error.");
	clog_info("This is a green %s log.", "formatted");
	clog_debug("This is a blue %s debug.", "formatted");
	clog_warn("This is a %s warning.", "formatted");
	clog_error("This is a %s error.", "formatted");
	clog_info("Note we only redefined the colors for INFO and DEBUG level logs. The rest remain in their default color.");
	clog_debug("We also redefined the output stream for those same levels. Maybe you'd want to split them up.");
	return 0;
}

