#include "MLCMain.hxx"

int MLCMain::main(int argc, char* argv[], char* envp[]) {
	hwloc_topology_t topo;
	hwloc_topology_init(&topo);
	hwloc_topology_load(topo);
	bool is_numa_machine = true;
	if(hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NODE) == 0) {
		is_numa_machine = false;
		cerr << "Warning: Non-NUMA mode!" << endl << endl;
	}

	EnsureLayoutersAlive();

	string default_layouter = "strided";

	if(argc < 2) {
		cerr << "Error: must provide at least 1 argument." << endl << endl;
		cerr << "Usage: " << argv[0] << " [-b] -a" << endl;
		if(is_numa_machine)
			cerr << "       " << argv[0] << " [-b] -m <node1> [<node2> [<node3> ...]]" << endl;
		return 1;
	}

	//size_t mem_size = 0x1000000;
	size_t mem_size = 0;
	char* mem_size_str = getenv("MLC_MEM_SIZE");
	if(mem_size_str) {
		size_t mem_size_cand = atoi(mem_size_str);
		if(mem_size_cand >= 0 && mem_size_cand < 0x100000000000)
			mem_size = mem_size_cand;
		else
			cerr << "Warning: Requested memory size of " << mem_size_str << " (" << mem_size_cand << ") is not valid." << endl;
	}

	if(!mem_size)
		mem_size = LatencyTestMemory::default_memory_size(topo);

	set< NodeID > mem_nodes;
	vector< tuple< NodeID, NodeID, unsigned int > > mem_tests;

	bool bandwidth = false;

	if(!strcmp(argv[1], "-b")) {
		bandwidth = true;
		default_layouter = "linear";

		/* dirty hack */
		argv[1] = argv[0];
		argv++;
		argc--;
	}

	if(!strcmp(argv[1], "-m")) {
		if(!is_numa_machine) {
			cerr << "Manual mode is not supported on non-NUMA machines!" << endl;
			return -42;
		}

		unsigned int num_cores = argc - 2;
		for(unsigned int i = 0; i < num_cores; i++) {
			NodeID node_id = atoi(argv[i+2]);
			mem_nodes.insert(node_id);
		}

	} else if(!strcmp(argv[1], "-a")) {
		if(is_numa_machine) {
			size_t num_nodes = hwloc_get_nbobjs_by_type(topo, HWLOC_OBJ_NODE);
			for(int i = 0; i < num_nodes; i++) {
				hwloc_obj_t node = hwloc_get_obj_by_type(topo, HWLOC_OBJ_NODE, i);
				mem_nodes.insert(node->logical_index);
			}
		} else {
			/* Non-NUMA machine */
			mem_tests.push_back(tuple< NodeID, NodeID, unsigned int >(0, 0, 1));
			mem_nodes.insert(0);
		}
	} else {
		cerr << "Must supply -m or -a!" << endl;
		return 2;
	}

	if(mem_tests.size() == 0) {
		/* Some automatic mode. Generate tests... */
		for(NodeID node_id_x : mem_nodes) {
			mem_tests.push_back(tuple< NodeID, NodeID, unsigned int >(node_id_x, node_id_x, 1));
		}

		for(NodeID node_id_x : mem_nodes) {
			for(NodeID node_id_y : mem_nodes) {
				if(node_id_x != node_id_y) {
					mem_tests.push_back(tuple< NodeID, NodeID, unsigned int >(node_id_x, node_id_y, 1));
				}
			}
		}
	}


	/* Generate tests */
	// generate local tests first (power8-workaround) //

	char* layouter_cstr = getenv("MLC_LAYOUTER");
	string layouter_str = default_layouter;
	if(layouter_cstr) {
		layouter_str = string(layouter_cstr);
	}
	LatencyTestMemoryLayouter *layouter = LatencyTestMemoryLayouter::getLayouter(layouter_str);
	if(!layouter) {
		cerr << "Error: Layouter identified by \"" << layouter_str << "\" not found. Exiting." << endl;
		return 4;
	}
	cerr << "- Using layouter \"" << layouter_str << "\"" << endl;

	/* Get number of runs */
	unsigned long long runs = 10; /* Default: 10 runs per NUMA node pair */
	char *runs_cstr = getenv("MLC_RUNS");
	if(runs_cstr) {
		runs = atoi(runs_cstr);
	}
	cerr << "- Doing " << runs << " runs per NUMA node pair" << endl;

	/* Sanity check */
	if(is_numa_machine) {
		for(NodeID node_id : mem_nodes) {
			hwloc_obj_t node = hwloc_get_obj_by_type(topo, HWLOC_OBJ_NODE, node_id);
			if(!node) {
				cerr << "Error: Node #" << node_id << " is invalid!" << endl;
				return 3;
			}
		}
	} else {
		cerr << "- UMA machine" << endl;
	}

	cerr << endl;

	/* Prepare environment */
	Platform::instance()->setup();

	/* Prepare memory */

	map< NodeID, LatencyTestMemory* > test_mems;
	vector< thread* > prepare_threads;

	for(NodeID node_id : mem_nodes) {
		cerr << "Preparing memory on node #" << node_id << "..." << endl;
		LatencyTestMemory* mem = new LatencyTestMemory(topo, mem_size, node_id, layouter);
		test_mems[node_id] = mem;
		thread* prepare_thread = new thread(&LatencyTestMemory::prepare, mem);
		prepare_threads.push_back(prepare_thread);
	}

	for(thread *t : prepare_threads) {
		t->join();
	}

	cerr << "Memory preparation done." << endl;

	bool failure = false;
	for(NodeID node_id : mem_nodes) {
		LatencyTestMemory *mem = test_mems[node_id];
		if(!mem->prepared()) {
			cerr << "Preparing node #" << node_id << " failed!" << endl;
			failure = true;
		}
	}
	if(failure) return 1;

	cerr << endl;

	/* Execute tests and generate output */

	cout << "exec_node\tmem_node\tmem_size\trun_time\tlatency\tbandwidth" << endl;

	for(tuple< NodeID, NodeID, unsigned int > mem_test : mem_tests) {
		NodeID run_node_id = get<0>(mem_test);
		NodeID mem_node_id = get<1>(mem_test);
		unsigned int num_threads = get<2>(mem_test); /* ignored for now */

		LatencyTestMemory *test_mem = test_mems[mem_node_id];
		LatencyTest test(topo, run_node_id, *test_mem, runs);

		if(test.start()) {
			test.join();

			LatencyTestResult res = test.get_result();

			cout << res.run_node << "\t" << res.mem_node << "\t" << res.num_elems << "\t" << res.duration << "\t" << (res.latency() * 1.0e9) << "\t" << (res.bandwidth() / (1024.0 * 1024.0 * 1024.0)) << endl;
		} else {
			cout << run_node_id << "\t" << mem_node_id << endl;
		}
	}

	/* Teardown */

	Platform::instance()->revert();

	hwloc_topology_destroy(topo);

	return 0;
}
