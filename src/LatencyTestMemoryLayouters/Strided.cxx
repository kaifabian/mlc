#include "Strided.hxx"

LatencyTestMemoryLayouter::Registrar<LatencyTestStridedMemoryLayouter> LatencyTestStridedMemoryLayouter::registrar("strided");

static long getStrideSize(void) {
	size_t stride = 0;
	char* stride_str = getenv("MLC_STRIDE");
	if(stride_str)
		stride = atoi(stride_str);
	if(!stride)
		stride = 1024;
	return stride;
}

size_t LatencyTestStridedMemoryLayouter::get_len(size_t m_requested_len) const {
	long stride = getStrideSize();
	long offset = m_requested_len % stride;
	if(offset <= 0) offset += stride;
	return m_requested_len + (stride - offset); //(m_requested_len % getStrideSize());
}

void LatencyTestStridedMemoryLayouter::initialize(LinkedListPtr mem, size_t len) const {
	long stride = getStrideSize();

	//cerr << "Memory Layout = STRIDED@" << stride << endl;
	//cerr << (len % stride) << endl;

	if((len % stride) != 0 || len == stride || !len) {
		cerr << "ERROR: Memory not as expected. Skipping." << endl;
		cerr << "(Memory is " << len << ", stride is " << stride << endl;
		mem[0].next = nullptr;
		return;
	}

	for(size_t i = 0; i < (len - stride); i += stride) {
		for(size_t j = 0; j < stride; j++) {
			mem[i + j].next = &mem[i + j + stride];
		}
	}
	for(size_t j = len - stride; j < (len - 1); j++) {
		mem[j].next = &mem[(j % stride) + 1];
	}

	mem[len-1].next = nullptr;
}

