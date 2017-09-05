#include "Platform_Linux.hxx"

Platform *Platform::g_instance = new Platform_Linux();

const string cpuPath(string cpu = "", string suffix = "") {
	ostringstream path;
	path << "/sys/devices/system/cpu";
	if(cpu.size() > 0)
		path << "/" << cpu;
	if(suffix.size() > 0)
		path << "/" << suffix;
	return path.str();
}

const vector< string > getCpus(void) {
	DIR *dir = opendir(cpuPath().c_str());
	struct dirent *ent;
	vector< string > cpus;
	regex cpu_filter("^(cpu[0-9]+)$");

	while(ent = readdir(dir)) {
		string filename(ent->d_name);
		if(ent->d_type == DT_DIR && regex_match(filename, cpu_filter))
			cpus.push_back(filename);
	}

	return cpus;
}

void Platform_Linux::set_thread_highest_priority(void) const {
#if !defined(_POSIX_PRIORITY_SCHEDULING)
  #error System does not support posix priority scheduling
#else
	if(this->disable_priority) return;

	if(!this->disable_realtime) {
		int sched_policy = SCHED_FIFO;
		struct sched_param sched_param = { .sched_priority = sched_get_priority_max(sched_policy) };

		/*if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched_param)) {
			perror("pthread_setschedparam");
		}*/

		sched_setscheduler(0, sched_policy, &sched_param);

		//sched_yield();
	} else {
		int new_nice = nice(-20);
	}
#endif
}

void Platform_Linux::setup(void) {
	char* realtime = getenv("MLC_DISABLE_REALTIME");
	if(realtime) {
		int realtime_i = atoi(realtime);
		if(realtime_i == 1)
			this->disable_realtime = true;
	}

	char* priority = getenv("MLC_DISABLE_PRIORITY");
	if(priority) {
		int priority_i = atoi(priority);
		if(priority_i == 1)
			this->disable_priority = true;
	}

	for(const string &cpu : getCpus()) {
		this->set_governor(cpu, "performance", true);
	}
}

void Platform_Linux::revert(void) {
	for(const pair< const string, string > &def : this->default_governors) {
		this->set_governor(def.first, def.second, false);
	}
}

bool Platform_Linux::set_governor(string cpu, string governor, bool store_default = false) {
	string path_av = cpuPath(cpu, "cpufreq/scaling_available_governors");
	string path_gov = cpuPath(cpu, "cpufreq/scaling_governor");

	struct stat stat_tmp;
	if(stat(path_av.c_str(), &stat_tmp) || stat(path_gov.c_str(), &stat_tmp)) {
		return false;
	}

	ifstream governors_av_f;
	governors_av_f.open(path_av, ios::in);
	if(!governors_av_f.is_open())
		return false;

	vector< string > governors;

	copy(istream_iterator<string>(governors_av_f), istream_iterator<string>(), back_inserter(governors));

	governors_av_f.close();

	bool governor_available = false;
	for(string &gov : governors)
		if(governor == gov)
			governor_available = true;

	if(!governor_available)
		return false;

	fstream governor_f;
	string previous_governor;
	governor_f.open(path_gov, ios::in | ios::out | ios::binary);
	if(!governor_f.is_open())
		return false;

	governor_f >> previous_governor;
	governor_f.seekg(0, governor_f.beg);
	governor_f << governor << endl;

	governor_f.close();

	if(store_default) {
		default_governors[cpu] = previous_governor;
	}

	return true;
}

#if USE_NATIVE_TIMERS
void *Platform_Linux::init_timer(void) {
	return (void *) calloc(sizeof(struct rusage), 1);
}

void Platform_Linux::start_timer(void *tim_p) {
	struct rusage *tim = (struct rusage *) tim_p;
	getrusage(RUSAGE_THREAD, tim);
}

double Platform_Linux::end_timer(void *tim_p) {
	struct rusage *tim = (struct rusage *) tim_p;
	struct rusage tim2;
	getrusage(RUSAGE_THREAD, &tim2);

	double elapsed_sec = tim2.ru_utime.tv_sec - tim->ru_utime.tv_sec;
	double elapsed_usec = tim2.ru_utime.tv_usec - tim->ru_utime.tv_usec;
	double elapsed = elapsed_sec + (elapsed_usec * 0.000001);

	delete tim;
	return elapsed;
}
#endif
