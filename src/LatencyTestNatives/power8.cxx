#if __GNUG__
#if __ppc__ and __ppc8__
 #define __POWER8__ 1
#endif

#if _M_PPC and _ARCH_8
 #define __POWER8__ 1
#endif

#if __POWER8__

#ifdef HAS_NATIVE_LATENCYTEST
 #error "Multiple native implementations"
#endif

#include "../LatencyTest.hxx"

// NOT YET IMPLEMENTED
// #define HAS_NATIVE_LATENCYTEST 1
// #define NATIVE_LATENCYTEST power8

// bool LatencyTest_ThreadNative(ll *list, unsigned long long runs, unsigned long long *duration_p, double *duration_time_p) {
// }

#endif
#endif
