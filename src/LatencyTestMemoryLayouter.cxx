#include "LatencyTestMemoryLayouter.hxx"

LatencyTestMemoryLayouter::type_registry &LatencyTestMemoryLayouter::registry(void) {
	static LatencyTestMemoryLayouter::type_registry reg;
	return reg;
}
