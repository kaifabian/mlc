#include "LatencyTest.hxx"

#include "LatencyTestNatives/_inc.cxx"

bool LatencyTest_ThreadGeneric(ll *list, unsigned long long runs, unsigned long long *cycles_p) {
	volatile ll* result;
	ll** list_p = &list;

	for(unsigned long long run = 0; run < runs; run++) {
		ll* cur = *list_p;
		for(void* cur = (void*) list; cur; cur = *((void**) cur));
		//for(ll* cur = list; cur; cur = cur->next);
		result = cur;
	}

	if(cycles_p)
		*cycles_p = -1;

	return true;
}

void LatencyTest_Thread(const hwloc_topology_t &topology, const hwloc_cpuset_t cpuset, ll *list, unsigned long long runs, unsigned long long &cycles, double &duration) {
	//HighResolutionTimer tim;
	Platform *p = Platform::instance();
	void *tim = p->init_timer();
	Platform::instance()->pin_thread_to_cpu(topology, cpuset);
	Platform::instance()->set_thread_highest_priority();

	//tim.start();
	p->start_timer(tim);

	//for(unsigned long long run = 0; run < runs; run++) {
#if USE_NATIVE_LATENCYTEST && HAS_NATIVE_LATENCYTEST
  #if __GNUG__
    #warning "Using native implementation"
  #else
    #pragma message ("warning: Using native implementation")
  #endif
		LatencyTest_ThreadNative(list, runs, &cycles);
#else
  #if __GNUG__
    #warning "Using generic implementation"
  #else
    #pragma message ("warning: Using generic implementation")
  #endif
		LatencyTest_ThreadGeneric(list, runs, &cycles);
#endif
	//}

	//duration = tim.elapsed();
	duration = p->end_timer(tim);
}

void LatencyTest::wait_complete(void) {
	unique_lock< mutex > lock(m_done_mutex);

	m_test_thread.join();

	//double single_read_ns;

	//single_read_ns = (m_duration * 1.0e9) / ((double) (m_test_mem.num_elements() * m_runs));

	m_result.success = true;
	//m_result.num_reads = m_test_mem.num_elements() * m_runs;
	m_result.duration = m_duration;
	//m_result.latency = single_read_ns;

	m_done = true;
	m_done_var.notify_all();
}


bool LatencyTest::run(void) {
	if(start())
		join();
	return m_result.success;
}

bool LatencyTest::start(void) {
	//hwloc_cpuset_t run_cs = hwloc_bitmap_alloc();
	//hwloc_bitmap_only(run_cs, m_run_cpu);
	ll* list = m_test_mem.entry_point();
	size_t list_len = m_test_mem.num_elements();

	/*ll* list = NULL;
	ll** list_p = &list;
	std::thread mem_prepare(LatencyTest_ListPreparationThread, std::ref(m_topology), std::ref(mem_cs), std::ref(list_p), list_len);
	mem_prepare.join();*/

	m_result.success = false;
	m_result.run_node = m_run_node;
	m_result.mem_node = m_test_mem.node_id();
	m_result.num_elems = m_test_mem.num_elements();
	m_result.num_runs = m_runs;
	m_result.duration = 0.0;

	if(list) {
		//hwloc_cpuset_t run_cs = Hwloc_GetCPUSetByCoreID(m_topology, m_run_cpu);
		hwloc_cpuset_t run_cs = Hwloc_GetCPUSetByNodeID(m_topology, m_run_node);
		m_test_thread = thread(LatencyTest_Thread, std::ref(m_topology), run_cs, list, m_runs, std::ref(m_cycles), std::ref(m_duration));
		m_complete_thread = new thread(&LatencyTest::wait_complete, this);
		//test.join();

		//std::cout << m_run_cpu << "\t" << m_mem_cpu << "\t" << (list_len * sizeof(ll)) << "\t" << duration << "\t" << single_read_ns << endl;
		return true;
	} else {
		//std::cout << m_run_cpu << "\t" << m_mem_cpu << "\t" << 0 << "\t" << "error" << endl;
		m_result.success = false;
		return false;
	}
}

void LatencyTest::join(void) {
	//m_test_thread.join();
	unique_lock< mutex > lock(m_done_mutex);
	while(!m_done)
		m_done_var.wait(lock);
}

const LatencyTestResult LatencyTest::get_result(void) const {
	return m_result;
}

size_t LatencyTestResult::num_reads() const {
	return this->num_runs * this->num_elems;
}

double LatencyTestResult::latency() const {
	double reads = (double) this->num_reads();
	return this->duration / reads;
}

double LatencyTestResult::bandwidth() const {
	double reads = (double) this->num_reads();
	return (reads * sizeof(LinkedListPtr)) / this->duration;
}
