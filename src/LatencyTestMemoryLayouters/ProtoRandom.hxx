#pragma once

#include "../LatencyTestMemoryLayouter.hxx"

class LatencyTestProtoRandomMemoryLayouter : public LatencyTestMemoryLayouter {
public:
	static LatencyTestMemoryLayouter *create() { new LatencyTestProtoRandomMemoryLayouter(); };
	static LatencyTestMemoryLayouter::Registrar<LatencyTestProtoRandomMemoryLayouter> registrar;

        virtual size_t get_len(size_t m_requested_len) const;
        virtual void initialize(LinkedListPtr mem, size_t len) const;
};
