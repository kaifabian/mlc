#include "ReverseStrided.hxx"

LatencyTestMemoryLayouter::Registrar<LatencyTestReverseStridedMemoryLayouter> LatencyTestReverseStridedMemoryLayouter::registrar("rstrided");

size_t LatencyTestReverseStridedMemoryLayouter::get_len(size_t m_requested_len) const {
	return m_requested_len;
}

void LatencyTestReverseStridedMemoryLayouter::initialize(LinkedListPtr mem, size_t len) const {
	size_t step_size = (-0x133742) % len;
	size_t enp0, enp1;
	enp0 = 0;

	for(size_t i = 0; i < (len - 1); i++) {
		enp1 = (enp0 + step_size) % len;
		mem[enp0].next = &mem[enp1];
		enp0 = enp1;
	}
	mem[enp0].next = nullptr;
}

