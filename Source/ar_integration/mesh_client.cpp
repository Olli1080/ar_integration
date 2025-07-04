#include "mesh_client.h"

#include "util.h"

bool U_mesh_client::get_meshes(
	const TArray<FString>& requests,
	TMap<FString, F_mesh_data>& meshes)
{
	if (!channel || !channel->channel || requests.IsEmpty()) return false;

	/**
	 * setup stream context and data
	 */
	grpc::ClientContext ctx;

	std::unique_lock lock(channel_mutex);
	auto stream = mesh_stub->transmit_mesh_data(&ctx);
	stream->WaitForInitialMetadata();
	
	for (const auto& request : requests)
	{
		generated::named_request req;
		req.set_name(convert<std::string>(request));

		/**
		 * return false if stream closed before
		 * all requests fullfiled
		 */
		if(!stream->Write(req)) return false;
	}
	stream->WritesDone();

	/**
	 * convert and emplace meshes after stream done
	 */
	TF_Conv_Wrapper wrapper;
	generated::Mesh_Data_TF_Meta mesh;
	while (stream->Read(&mesh))
		meshes.Emplace(convert<FString>(mesh.mesh_data().name()), convert_meta<F_mesh_data>(mesh, wrapper));
	
	return stream->Finish().ok();
}

bool U_mesh_client::get_object_prototypes(
	const TArray<FString>& requests,
	TMap<FString, F_object_prototype>& prototypes)
{
	if (!channel || !channel->channel || requests.IsEmpty()) return false;

	/**
	 * setup stream context and data
	 */
	grpc::ClientContext ctx;

	std::unique_lock lock(channel_mutex);
	auto stream = obj_proto_stub->transmit_object_prototype(&ctx);
	stream->WaitForInitialMetadata();

	for (const auto& request : requests)
	{
		generated::named_request req;
		req.set_name(convert<std::string>(request));

		/**
		 * return false if stream closed before
		 * all requests fullfiled
		 */
		if (!stream->Write(req)) return false;
	}
	stream->WritesDone();

	/**
	 * convert and emplace prototypes after stream done
	 */
	TF_Conv_Wrapper wrapper;
	generated::Object_Prototype_TF_Meta obj_proto;
	while (stream->Read(&obj_proto))
		prototypes.Emplace(
			convert<FString>(obj_proto.object_prototype().name()),
			convert_meta<F_object_prototype>(obj_proto, wrapper));

	return stream->Finish().ok();
}