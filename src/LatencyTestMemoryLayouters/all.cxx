#include "all.hxx"

void EnsureLayoutersAlive() {
	LatencyTestLinearMemoryLayouter::registrar.ensureAlive();
	LatencyTestProtoRandomMemoryLayouter::registrar.ensureAlive();
	LatencyTestStridedMemoryLayouter::registrar.ensureAlive();
	LatencyTestReverseStridedMemoryLayouter::registrar.ensureAlive();
}
