#if defined(__GNUG__)
  #define ATTR_USED __attribute__(( used ))
  #define DEF_DESTR = default
#elif defined(_MSVC_LANG)
  #if _MSVC_LANG >= 201100
    #define ATTR_USED
    #define DEF_DESTR = default
  #endif
#else
  #define DEF_DESTR
  #define ATTR_USED
#endif
