#include "grpc_wrapper.h"

F_obb U_grpc_wrapper::Make_obb(FVector translation, FRotator rotation, FVector extent)
{
	return F_obb{ FBox::BuildAABB(translation, extent), FQuat(rotation) };
}