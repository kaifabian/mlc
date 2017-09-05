#pragma once

#include "../LatencyTestMemoryLayouter.hxx"

class LatencyTestStridedMemoryLayouter : public LatencyTestMemoryLayouter {
public:
	static LatencyTestMemoryLayouter *create() { new LatencyTestStridedMemoryLayouter(); };
	static LatencyTestMemoryLayouter::Registrar<LatencyTestStridedMemoryLayouter> registrar;

        virtual size_t get_len(size_t m_requested_len) const;
        virtual void initialize(LinkedListPtr mem, size_t len) const;
};
