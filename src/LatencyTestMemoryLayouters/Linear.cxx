#include "Linear.hxx"

LatencyTestMemoryLayouter::Registrar<LatencyTestLinearMemoryLayouter> LatencyTestLinearMemoryLayouter::registrar("linear");

size_t LatencyTestLinearMemoryLayouter::get_len(size_t m_requested_len) const {
	return m_requested_len;
}

void LatencyTestLinearMemoryLayouter::initialize(LinkedListPtr mem, size_t len) const {
	for(size_t i = 0; i < (len - 1); i++) {
		mem[i].next = &mem[i+1];
	}

	mem[len-1].next = nullptr;
}

