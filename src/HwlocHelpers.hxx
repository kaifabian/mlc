#pragma once

#include <vector>

#include <hwloc.h>
//#include <hwloc/bitmap.h>
//#include <hwloc/cpuset.h>
//#include <hwloc/helper.h>

#include "Types.hxx"

using namespace std;

static inline bool Hwloc_IsNumaMachine(const hwloc_topology_t &topology) {
	unsigned num_nodes = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_NODE);
	return num_nodes > 1;
}

static hwloc_obj_t Hwloc_GetNodeById(const hwloc_topology_t &topology, NodeID node_id) {
	hwloc_obj_type_t hwloc_type = Hwloc_IsNumaMachine(topology) ? HWLOC_OBJ_NODE : HWLOC_OBJ_SOCKET;
	return hwloc_get_obj_by_type(topology, hwloc_type, node_id);
}

static size_t Hwloc_GetSmallestNodeMemory(const hwloc_topology_t &topology) {
	if(!Hwloc_IsNumaMachine(topology)) {
		hwloc_obj_t machine = hwloc_get_obj_by_type(topology, HWLOC_OBJ_MACHINE, 0);
		return machine->memory.local_memory;
	}

	size_t smallest_memory = (size_t) -1;
	for(unsigned i = 0; i < hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_NODE); i++) {
		hwloc_obj_t node = hwloc_get_obj_by_type(topology, HWLOC_OBJ_NODE, i);
		if(node->memory.local_memory < smallest_memory)
			smallest_memory = node->memory.local_memory;
	}

	return smallest_memory;
}

static size_t Hwloc_GetLargestCachePerPU(const hwloc_topology_t &topology, unsigned index) {
	size_t largest_cache = 0;

	hwloc_obj_t pu = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, index);

	if(!pu) return 0;

	for(hwloc_obj_t obj = pu; obj; obj = obj->parent) {
		if(obj->type == HWLOC_OBJ_CACHE) {
			size_t cache_size = obj->attr->cache.size;
			if(cache_size > largest_cache)
				largest_cache = cache_size;
		}
	}

	return largest_cache;
}

static size_t Hwloc_GetLargestCache(const hwloc_topology_t &topology) {
	unsigned num_cores = hwloc_get_nbobjs_by_type(topology, HWLOC_OBJ_PU);
	size_t largest_cache = 0;

	for(unsigned i = 0; i <= num_cores + 1; i++) {
		size_t cache_size = Hwloc_GetLargestCachePerPU(topology, i);
		if(cache_size > largest_cache)
			largest_cache = cache_size;
	}

	if(largest_cache == 0) {
		/* no caches? assume largest page size */
		hwloc_obj_t machine = hwloc_get_obj_by_type(topology, HWLOC_OBJ_MACHINE, 0);
		for(unsigned i = 0; i < machine->memory.page_types_len; i++) {
			size_t page_size = machine->memory.page_types[i].size;
			if(page_size > largest_cache)
				largest_cache = page_size;
		}
	}


	if(!largest_cache) {
		/* We could not find anything. Assume 1MB. */
		largest_cache = 1 * 1024 * 1024;
	}

	return largest_cache;
}

static inline CoreID Hwloc_GetCoreIDByCPU(const hwloc_topology_t &topology, unsigned int cpu_id) {
	return hwloc_get_pu_obj_by_os_index(topology, cpu_id)->logical_index;
}

static inline unsigned int Hwloc_GetCPUByCoreID(const hwloc_topology_t &topology, CoreID core_id) {
	hwloc_obj_t core = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, core_id);
	return core->os_index;
}

static inline NodeID Hwloc_GetNodeByCoreID(const hwloc_topology_t &topology, CoreID core_id) {
	hwloc_obj_t core = hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, core_id);
	hwloc_obj_t node = hwloc_get_ancestor_obj_by_type(topology, HWLOC_OBJ_NODE, core);
	return node->logical_index;
}

static inline hwloc_cpuset_t Hwloc_GetCPUSetByCoreID(const hwloc_topology_t &topology, CoreID core_id) {
	return hwloc_get_obj_by_type(topology, HWLOC_OBJ_PU, core_id)->allowed_cpuset;
}

static inline hwloc_nodeset_t Hwloc_GetNodeSetByNodeID(const hwloc_topology_t &topology, NodeID node_id) {
	hwloc_obj_t node = Hwloc_GetNodeById(topology, node_id);
	return node->allowed_nodeset;
}

static inline hwloc_cpuset_t Hwloc_GetCPUSetByNodeID(const hwloc_topology_t &topology, NodeID node_id) {
	hwloc_obj_t node = Hwloc_GetNodeById(topology, node_id);
	return node->allowed_cpuset;
}

static inline void* Hwloc_AllocMemoryOnNode(const hwloc_topology_t &topology, NodeID node_id, size_t size) {
	if(!Hwloc_IsNumaMachine(topology)) {
		return malloc(size);
	}

	hwloc_nodeset_t nodeset = Hwloc_GetNodeSetByNodeID(topology, node_id);
	return hwloc_alloc_membind_nodeset(topology, size, nodeset, HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_STRICT | HWLOC_MEMBIND_NOCPUBIND);
}
