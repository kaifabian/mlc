#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <random>

//#include <sys/mman.h>

#include <hwloc.h>
//#include <hwloc/bitmap.h>
//#include <hwloc/cpuset.h>

#include "Types.hxx"
#include "LinkedList.hxx"
#include "Platform.hxx"
#include "HwlocHelpers.hxx"
#include "LatencyTestMemoryLayouter.hxx"

using namespace std;

class LatencyTestMemory {
private:
	const hwloc_topology_t &m_topology;
	size_t m_list_len;
	NodeID m_mem_node;
	bool m_prepared;
	LinkedListPtr m_list;
	const LatencyTestMemoryLayouter *m_layouter;
public:
	LatencyTestMemory(const hwloc_topology_t &topology, size_t list_len, NodeID mem_node, LatencyTestMemoryLayouter *layouter) : m_topology(topology), m_mem_node(mem_node), m_list(nullptr), m_prepared(false), m_layouter(layouter) {
		m_list_len = layouter->get_len(list_len);
	};

	static size_t default_memory_size(const hwloc_topology_t &topology);
	static size_t default_list_len(const hwloc_topology_t &topology);
	static constexpr size_t pointer_size(void);

	bool prepare(void);

	size_t num_elements(void) const;
	size_t memory_size(void) const;
	bool prepared(void) const;
	NodeID node_id(void) const;
	LinkedListPtr entry_point(void) const;
};

