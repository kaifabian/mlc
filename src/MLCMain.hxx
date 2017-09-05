#pragma once

#include <iostream>

#include <cinttypes>
#include <cstdio>

#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <thread>

#include <hwloc.h>
//#include <hwloc/bitmap.h>
//#include <hwloc/cpuset.h>

#include "Platform.hxx"
#include "LatencyTest.hxx"
#include "LatencyTestMemory.hxx"
#include "LatencyTestMemoryLayouter.hxx"
#include "LatencyTestMemoryLayouters/all.hxx"

using namespace std;

class MLCMain {
public:
	static int main(int argc, char* argv[], char* envp[]);
};
