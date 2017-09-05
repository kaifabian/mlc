#pragma once

#include "../LatencyTestMemoryLayouter.hxx"

class LatencyTestLinearMemoryLayouter : public LatencyTestMemoryLayouter {
public:
	static LatencyTestMemoryLayouter *create() { return new LatencyTestLinearMemoryLayouter(); };
	static LatencyTestMemoryLayouter::Registrar<LatencyTestLinearMemoryLayouter> registrar;

        virtual size_t get_len(size_t m_requested_len) const;
        virtual void initialize(LinkedListPtr mem, size_t len) const;
};
