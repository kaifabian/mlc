# declarations
cmake_minimum_required (VERSION 2.6)
project (KaisMLC)


# defaults
set(USE_NATIVE_TIMERS 0)
set(USE_NATIVE_LATENCYTEST 0)


# includes
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")



# Find pthread
find_package (Threads)


# Find libhwloc
if(NOT HWLOC_INCLUDE_DIRS OR NOT HWLOC_LIBRARY_DIRS)
	include(FindPkgConfig)
	pkg_search_module(HWLOC REQUIRED hwloc)
endif()

include_directories(${HWLOC_INCLUDE_DIRS})
link_directories(${HWLOC_LIBRARY_DIRS})

if(NOT HWLOC_LIBRARIES)
	set(HWLOC_LIBRARIES "libhwloc")
endif()


# OS detection
if(UNIX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(LINUX true)
else()
	set(LINUX false)
endif()

if(UNIX AND APPLE)
	set(MACOS true)
else()
	set(MACOS false)
endif()


# OS-dependant settings
set(MLCLIB_PLATFORM_CLASS Platform_Generic)
if(LINUX)
	message(STATUS "Linux!")
	set(mlclib_os_deps ${mlclib_os_deps} src/Platform_Linux.cxx)
	set(MLCLIB_PLATFORM_CLASS Platform_Linux)
	set(GCC true)
elseif(WIN32)
	message(STATUS "Windows!")
	set(mlclib_os_deps ${mlclib_os_deps} src/Platform_Win32.cxx)
	set(MLCLIB_PLATFORM_CLASS Platform_Win32)
else()
	message(SEND_ERROR "Operating system unsupported: ${CMAKE_SYSTEM_NAME}")
endif()


if(GCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2")

	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
	if (COMPILER_OPT_ARCH_NATIVE_SUPPORTED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native")
	endif()

	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_OPT_CPP11_SUPPORTED)
	if (COMPILER_OPT_CPP11_SUPPORTED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
		CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_OPT_CPP0X_SUPPORTED)
		if (COMPILER_OPT_CPP0X_SUPPORTED)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
		else()
			message(SEND_ERROR "Compiler does not support C++11")
		endif()
	endif()
endif()


message(STATUS "Platform class: ${MLCLIB_PLATFORM_CLASS}")


# build configuration file
configure_file (
	"${PROJECT_SOURCE_DIR}/src/mlc-config.h.in"
	"${PROJECT_BINARY_DIR}/mlc-config.h"
	@ONLY
)

include_directories("${PROJECT_BINARY_DIR}")

# Build mlc binary
# <mlclib> (
	#file(GLOB mlclib_natives src/LatencyTestNatives/*.cxx)
	file(GLOB mlclib_layouters src/LatencyTestMemoryLayouters/*.cxx)
	add_library(mlclib STATIC src/LatencyTest.cxx src/Platform.cxx src/LatencyTestMemory.cxx src/LatencyTestMemoryLayouter.cxx ${mlclib_layouters} ${mlclib_os_deps}) # ${mlclib_natives})
# )
# <mlc> (
	add_executable(mlc src/mlc.cxx src/MLCMain.cxx)
	target_link_libraries(mlc mlclib ${HWLOC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
# )
