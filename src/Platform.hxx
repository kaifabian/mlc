#pragma once

#include "mlc-config.h"

#include <hwloc.h>
//#include <hwloc/bitmap.h>
//#include <hwloc/cpuset.h>

#include "CompilerHelpers.hxx"
#include "Timer.hxx"

using namespace std;

class Platform {
public:
	static Platform *g_instance;

	static Platform *instance() {
		/*if(g_instance == nullptr) {
			g_instance = new MLCLIB_PLATFORM_CLASS();
		}*/
		return g_instance;
	};

	virtual ~Platform() DEF_DESTR;

	/* Virtual methods / interface declaration */
	virtual void set_thread_highest_priority(void) const = 0;
	virtual void pin_thread_to_cpu(const hwloc_topology_t &topology, const hwloc_cpuset_t &cpuset) const;

	virtual void setup(void) {};
	virtual void revert(void) {};

	virtual void *init_timer(void) {
		return (void*) new HighResolutionTimer();
	};
	virtual void start_timer(void *tim_p) {
		HighResolutionTimer *tim = (HighResolutionTimer *) tim_p;
		tim->start();
	};
	virtual double end_timer(void *tim_p) {
		HighResolutionTimer *tim = (HighResolutionTimer *) tim_p;
		double duration = tim->elapsed();
		delete tim;
		return duration;
	};
};

class Platform_Generic : public Platform {
	virtual void set_thread_highest_priority(void) const {};
};
