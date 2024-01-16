#ifndef GRPC_INCLUDE_BEGIN
#define GRPC_INCLUDE_BEGIN

/***
 * this header makes it possible to compile
 * files generated by gRPC with unreal engine
 *
 * THIS HEADER HAS TO BE INCLUDED PRIOR TO THE GENERATED HEADERS
 */
#pragma warning(push)
#pragma warning (disable : 4800 4125 4647 4668 4582 4583 4946 4996)

#if (defined(PLATFORM_HOLOLENS) | defined(PLATFORM_WINDOWS))
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/MinWindows.h"
#include "Windows/WindowsPlatformMisc.h"

#ifndef WORKAROUND_SYMBOL_MEMORY_BARRIER
#define WORKAROUND_SYMBOL_MEMORY_BARRIER
static void MemoryBarrier()
{
	FWindowsPlatformMisc::MemoryBarrier();
}
#endif
#endif
#endif