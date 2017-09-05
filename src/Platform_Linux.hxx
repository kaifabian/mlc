#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include <iterator>
#include <map>
#include <regex>
#include <vector>

/* SCHEDULER */
#include <sched.h>

/* STAT */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

/* TIME */
#include <sys/time.h>
#include <sys/resource.h>

#include "CompilerHelpers.hxx"
#include "Platform.hxx"

#include "mlc-config.h"

class Platform_Linux : public Platform {
private:
	map< string, string > default_governors;

	bool disable_realtime;
	bool disable_priority;
	bool set_governor(string cpu, string governor, bool store_default);

public:
	Platform_Linux() :
		disable_realtime(false),
		disable_priority(false),
		Platform() {};
	virtual ~Platform_Linux() DEF_DESTR;

	virtual void set_thread_highest_priority(void) const;
	virtual void setup(void);
	virtual void revert(void);

#if USE_NATIVE_TIMERS
	virtual void *init_timer(void);
	virtual void start_timer(void*);
	virtual double end_timer(void*);
#endif
};
