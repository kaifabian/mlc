#include "Platform.hxx"

void Platform::pin_thread_to_cpu(const hwloc_topology_t &topology, const hwloc_cpuset_t &cpuset) const {
	hwloc_set_cpubind(topology, cpuset, HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_STRICT | HWLOC_CPUBIND_NOMEMBIND);
}
