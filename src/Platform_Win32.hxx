#pragma once

#include <windows.h>

#include "Platform.hxx"

#include "mlc-config.h"


class Platform_Win32 : public Platform {
public:
	virtual void set_thread_highest_priority(void) const;

#if USE_NATIVE_TIMERS
	virtual void *init_timer(void);
	virtual void start_timer(void*);
	virtual double end_timer(void*);
#endif
};
