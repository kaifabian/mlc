#if __GNUG__
#if __amd64__ or __x86_64__ or _M_X64 or _M_AMD64

#ifdef HAS_NATIVE_LATENCYTEST
 #error "Multiple native implementations"
#endif

#include "../LatencyTest.hxx"

#define HAS_NATIVE_LATENCYTEST 1
#define NATIVE_LATENCYTEST amd64

bool LatencyTest_ThreadNative(ll *list, unsigned long long runs, unsigned long long *cycles_p) {
	unsigned long long duration = 0;

	asm volatile (
		/* asm source */
			"xor	%%rcx,		%%rcx	\n"
			"xor	%%r12,		%%r12	\n"
			"mov	%[runs],	%%r10	\n"
			"mov	%[list],	%%r11	\n"

			"rdtsc				\n"
			"shl	$0x20,		%%rdx	\n"
			"or	%%rax,		%%rdx	\n"
			"mov	%%rdx,		%%r8	\n"

			"_run:				\n"
			"cmp	%%r12,		%%r10	\n"
			"jz	_runs_done		\n"
			"inc	%%r12			\n"

			"mov	%%r11,		%%rbx	\n"

			"_run_inner:			\n"
//			"clflush 0(%%rbx)		\n"
//			"mfence				\n"
			"mov	0(%%rbx),	%%rbx	\n"
			"test	%%rbx,		%%rbx	\n"
			"jz	_run			\n"
			"jmp	_run_inner		\n"

			"_runs_done:			\n"
			"rdtsc				\n"
			"shl	$0x20,		%%rdx	\n"
			"or	%%rax,		%%rdx	\n"
			"mov	%%rdx,		%%r9	\n"

			"sub	%%r8,		%%r9	\n"
			"mov	%%r9,		%[cycl]	\n"
		: /* outputs */
			[cycl]	"=r"	(duration)
		: /* inputs */
			[runs]	"r"	(runs),
			[list]	"m"	(list)
		: /* clobbered registers */
			"rax", "rbx", "rcx", "rdx", "r8", "r9", "r10", "r11", "r12"
	);

	if(cycles_p)
		*cycles_p = duration;

	return true;
}

#endif
#endif
