#include "LatencyTestMemory.hxx"

bool LatencyTestMemory::prepare() {
	hwloc_nodeset_t nodeset = Hwloc_GetNodeSetByNodeID(m_topology, node_id());
	hwloc_cpuset_t cpuset = Hwloc_GetCPUSetByNodeID(m_topology, node_id());

	size_t offset = 0;
	char *offset_c = getenv("MLC_OFFSET");
	if(offset_c)
		offset = atoi(offset_c);

	m_list = (LinkedListPtr) Hwloc_AllocMemoryOnNode(m_topology, node_id(), memory_size() + offset);
	if(!m_list) return false;

	m_list = (LinkedListPtr) (((char*) m_list) + offset);

	Platform::instance()->pin_thread_to_cpu(m_topology, cpuset);
	Platform::instance()->set_thread_highest_priority();

	memset((void *) m_list, 0, memory_size());

	m_layouter->initialize(m_list, num_elements());

	m_prepared = true;
	return true;
}

size_t LatencyTestMemory::memory_size(void) const {
	return pointer_size() * num_elements();
}

size_t LatencyTestMemory::num_elements(void) const {
	return m_list_len;
}

bool LatencyTestMemory::prepared(void) const {
	return m_prepared;
}

NodeID LatencyTestMemory::node_id(void) const {
	return m_mem_node;
}

LinkedListPtr LatencyTestMemory::entry_point(void) const {
	return m_list;
}

/*static*/ constexpr size_t LatencyTestMemory::pointer_size(void) {
	return sizeof(LinkedListPtr);
}

/*static*/ size_t LatencyTestMemory::default_list_len(const hwloc_topology_t &topology) {
	return LatencyTestMemory::default_memory_size(topology) / LatencyTestMemory::pointer_size();
}

/*static*/ size_t LatencyTestMemory::default_memory_size(const hwloc_topology_t &topology) {
	/* Default: 100 times larger than largest cache, but at most half of smallest memory of any NUMA node */

	size_t largest_cache = Hwloc_GetLargestCache(topology);
	size_t smallest_memory = Hwloc_GetSmallestNodeMemory(topology);
	size_t memory_limit = smallest_memory / 2;

	size_t memory_recommendation = largest_cache * 100;
	if(memory_recommendation > memory_limit)
		memory_recommendation = memory_limit;

	return memory_recommendation;
}
