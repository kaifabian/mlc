#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <random>

#include <hwloc.h>
//#include <hwloc/bitmap.h>
//#include <hwloc/cpuset.h>

#include "Types.hxx"
#include "LinkedList.hxx"
#include "Timer.hxx"
#include "Platform.hxx"
#include "HwlocHelpers.hxx"

#include "LatencyTestMemory.hxx"

#include "mlc-config.h"

using namespace std;

class LatencyTestResult {
public:
	bool success;
	NodeID run_node;
	NodeID mem_node;
	size_t num_runs;
	size_t num_elems;
	double duration;
	double latency() const;
	double bandwidth() const;
	size_t num_reads() const;
};

class LatencyTest {
private:
	const hwloc_topology_t &m_topology;
	const NodeID m_run_node;
	const LatencyTestMemory &m_test_mem;
	LatencyTestResult m_result;
	thread m_test_thread;
	thread *m_complete_thread;
	bool m_done;
	condition_variable m_done_var;
	mutex m_done_mutex;

	double m_duration;
	unsigned long long m_runs;
	unsigned long long m_cycles;

	void wait_complete(void);
public:
	LatencyTest(const hwloc_topology_t &topology, const NodeID run_node, const LatencyTestMemory &test_mem, const unsigned long long runs) : m_topology(topology), m_run_node(run_node), m_test_mem(test_mem), m_runs(runs), m_cycles(0), m_done(false) {};
	bool run(void);
	bool start(void);
	void join(void);
	const LatencyTestResult get_result(void) const;
};

