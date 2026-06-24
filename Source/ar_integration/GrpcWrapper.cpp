#include "GrpcWrapper.h"

F_obb UGrpcWrapper::Make_obb(FVector translation, FRotator rotation, FVector extent)
{
	return F_obb{ FBox::BuildAABB(translation, extent), FQuat(rotation) };
}