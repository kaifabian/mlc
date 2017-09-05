#include "Platform_Win32.hxx"

Platform *Platform::g_instance = new Platform_Win32();

void Platform_Win32::set_thread_highest_priority(void) const {
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    //volatile int y = 0;
    //int x = 1/y;
	/*
	struct sched_param sched_param = { .sched_priority = sched_get_priority_max(SCHED_FIFO) };

	if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched_param)) {
		perror("pthread_setschedparam");
	}

	//hwloc_set_cpubind(topology, cpuset, HWLOC_CPUBIND_THREAD  | HWLOC_CPUBIND_STRICT | HWLOC_CPUBIND_NOMEMBIND);

	sched_yield();
	*/
}

#if USE_NATIVE_TIMERS
void *Platform_Win32::init_timer(void) {
	return (void *) calloc(sizeof(FILETIME), 1);
}

void Platform_Win32::start_timer(void *tim_p) {
	LPFILETIME tim = (LPFILETIME) tim_p;
	FILETIME _t;
	GetThreadTimes(GetCurrentThread(), &_t, &_t, &_t, tim);
}

double Platform_Win32::end_timer(void *tim_p) {
	LPFILETIME tim = (LPFILETIME) tim_p;
	FILETIME tim2;
	FILETIME _t;
	GetThreadTimes(GetCurrentThread(), &_t, &_t, &_t, &tim2);

	unsigned long long time1 = (((unsigned long long) tim->dwHighDateTime) << 32) | ((unsigned long long) tim->dwLowDateTime);
	unsigned long long time2 = (((unsigned long long) tim2.dwHighDateTime) << 32) | ((unsigned long long) tim2.dwLowDateTime);

	double elapsed = ((double) (time2 - time1)) * 10.0 /* to usec */ * 1000000.0 /* to sec */;

	delete tim;
	return elapsed;
}
#endif
