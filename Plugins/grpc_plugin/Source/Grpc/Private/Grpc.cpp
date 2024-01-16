#include "Grpc.h"

DEFINE_LOG_CATEGORY(LogGrpc);

#define LOCTEXT_NAMESPACE "FGrpcModule"

void FGrpcModule::StartupModule()
{}

void FGrpcModule::ShutdownModule()
{}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGrpcModule, Grpc)
