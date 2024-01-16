#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "grpc_wrapper.h"
#include "grpc_channel.h"
#include "base_client.h"

#include "grpc_include_begin.h"
#include "object_prototype.grpc.pb.h"
#include "vertex.grpc.pb.h"
#include "grpc_include_end.h"

#include "mesh_client.generated.h"

/**
 * @class U_mesh_client
 * client for receiving meshes and prototypes
 */
UCLASS(BlueprintType)
class U_mesh_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	/**
	 * requests meshes and receives them by name
	 *
	 * @param requests requested meshes by name
	 * @param meshes received meshes by name
	 *
	 * @attend only received meshes are present in meshes
	 *
	 * @return false if stream fails or empty requests
	 */
	UFUNCTION(BlueprintCallable)
	bool get_meshes(
		const TArray<FString>& requests, 
		TMap<FString, F_mesh_data>& meshes);

	/**
	 * requests prototypes and receives them by name
	 *
	 * @param requests requested prototypes by name
	 * @param meshes received prototypes by name
	 *
	 * @attend only received prototypes are present in prototypes
	 *
	 * @return false if stream fails or empty requests
	 */
	UFUNCTION(BlueprintCallable)
	bool get_object_prototypes(
		const TArray<FString>& requests, 
		TMap<FString, F_object_prototype>& prototypes);

	virtual void stop_Implementation() override {}
	virtual void state_change_Implementation(connection_state old_state, connection_state new_state) override {}

private:
	
	std::unique_ptr<generated::mesh_com::Stub> mesh_stub;
	std::unique_ptr<generated::object_prototype_com::Stub> obj_proto_stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			mesh_stub = generated::mesh_com::NewStub(ch);
			obj_proto_stub = generated::object_prototype_com::NewStub(ch);
		}
	)
};