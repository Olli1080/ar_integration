#ifdef GRPC_INCLUDE_BEGIN

/***
 * this header makes it possible to compile
 * files generated by gRPC with unreal engine
 *
 * THIS HEADER HAS TO BE INCLUDED AFTER THE LAST GENERATED HEADER
 */

#if (defined(PLATFORM_HOLOLENS) | defined(PLATFORM_WINDOWS))

//#pragma pop_macro("InterlockedCompareExchange64")
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#pragma warning( pop )

#undef GRPC_INCLUDE_BEGIN

#elif

#pragma message "Not matching begin header"

#endif