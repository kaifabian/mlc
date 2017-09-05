#include "ProtoRandom.hxx"

#include "ProtoRandom_ProtoBlock.inc"

LatencyTestMemoryLayouter::Registrar<LatencyTestProtoRandomMemoryLayouter> LatencyTestProtoRandomMemoryLayouter::registrar("proto,default");

size_t LatencyTestProtoRandomMemoryLayouter::get_len(size_t m_requested_len) const {
	size_t additional = 0;
	if (m_requested_len % protoblock_length) {
		additional = protoblock_length - (m_requested_len % protoblock_length);
	}

	return m_requested_len + additional;
}

void LatencyTestProtoRandomMemoryLayouter::initialize(LinkedListPtr mem, size_t len) const {
	size_t runs = len / protoblock_length;

	for(size_t index = 0; index < protoblock_length; index++) {
		size_t i = index * runs;
		unsigned int pointer = protoblock[index];
		for(size_t j = 0; j < runs; j++) {
			if(pointer) {
				mem[i + j].next = &mem[pointer * runs + j];
			} else {
				if(j != (runs - 1)) {
					mem[i + j].next = &mem[pointer * runs + j + 1];
				} else {
					mem[i + j].next = nullptr;
					//cerr << "Exit pointer is at " << &mem[i + j] << " (" << (i + j) << ")" << endl;
				}
			}
		}
	}
}

