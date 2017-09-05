#pragma once

#include "../LatencyTestMemoryLayouter.hxx"

class LatencyTestReverseStridedMemoryLayouter : public LatencyTestMemoryLayouter {
public:
	static LatencyTestMemoryLayouter *create() { new LatencyTestReverseStridedMemoryLayouter(); };
	static LatencyTestMemoryLayouter::Registrar<LatencyTestReverseStridedMemoryLayouter> registrar;

        virtual size_t get_len(size_t m_requested_len) const;
        virtual void initialize(LinkedListPtr mem, size_t len) const;
};
