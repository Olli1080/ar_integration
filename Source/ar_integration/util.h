#pragma once

#include "Math/Vector.h"
#include "Math/TransformVectorized.h"
#include "HeadMountedDisplayTypes.h"

#include "grpc_wrapper.h"
#include "camera.h"

#include "grpc_include_begin.h"
#include "vertex.pb.h"
#include "object_prototype.pb.h"
#include "depth_image.pb.h"
#include "object.pb.h"
#include "hand_tracking.pb.h"
#include "robot.pb.h"
#include "meta_data.pb.h"
#include "grpc_include_end.h"

//#include <draco/point_cloud/point_cloud_builder.h>
//#include <draco/compression/point_cloud/point_cloud_kd_tree_encoder.h>

//#include "Math/UnitConversion.h"

#include "TransformHelper.h"

/**
 * This header allows hiding details of generated types
 * and modularity of conversion implementation
 * thereby allowing full utilization of the unreal system
 */

class TF_Conv_Wrapper
{
public:

	TF_Conv_Wrapper() = default;
	void set_source(const Transformation::TransformationMeta& meta);
	const Transformation::TransformationConverter& converter() const;
	bool has_converter() const;

private:

	std::unique_ptr<Transformation::TransformationConverter> m_converter;
};

/**
 * template for conversion between unreal usable types and generated types
 */
template<typename out, typename in>
out convert(const in&);

template<typename out, typename in>
out convert_meta(const in&, TF_Conv_Wrapper& cv);

template<typename out, typename in>
out convert_meta(const in&, const Transformation::TransformationConverter* cv = nullptr);

generated::Transformation_Meta generate_meta();


template<typename inner_out, typename inner_in>
TArray<inner_out> convert_array(const google::protobuf::RepeatedPtrField<inner_in>& in)
{
	TArray<inner_out> out;
	out.Reserve(in.size());

	for (const auto& it : in)
		out.Add(convert<inner_out, inner_in>(it));

	return out;
}

template<typename inner_out, typename inner_in>
TArray<inner_out> convert_array_meta(const google::protobuf::RepeatedPtrField<inner_in>& in, const Transformation::TransformationConverter* cv = nullptr)
{
	TArray<inner_out> out;
	out.Reserve(in.size());

	for (const auto& it : in)
		out.Add(convert_meta<inner_out, inner_in>(it, cv));

	return out;
}

template<typename inner_out, typename inner_in>
std::vector<inner_out> convert_std_array(const TArray<inner_in>& in)
{
	std::vector<inner_out> out;
	out.reserve(in.Num());

	for (const auto& it : in)
		out.emplace_back(convert<inner_out, inner_in>(it));

	return out;
}

template<typename inner_out, bool filter_nan>
std::vector<inner_out> convert_std_array(const TArray<FVector>& in)
{
	std::vector<inner_out> out;
	if constexpr (filter_nan)
	{
		TSet<size_t> not_nan;
		for (size_t i = 0; i < in.Num(); ++i)
		{
			if (!in[i].ContainsNaN())
				not_nan.Add(i);
		}
		out.reserve(not_nan.Num());

		for (const auto& index : not_nan)
			out.emplace_back(convert<inner_out, FVector>(in[index]));
	}
	else
	{
		out.Reserve(in.Num());

		for (const auto& it : in)
			out.emplace_back(convert<inner_out, FVector>(it));
	}
	return out;
}


template<typename inner_out, typename inner_in>
google::protobuf::RepeatedPtrField<inner_out> convert_array(const TArray<inner_in>& in)
{
	google::protobuf::RepeatedPtrField<inner_out> out;
	out.Reserve(in.Num());

	for (const auto& it : in)
		out.Add(convert<inner_out, inner_in>(it));

	return out;
}

template<typename inner_out, bool filter_nan>
google::protobuf::RepeatedPtrField<inner_out> convert_array(const TArray<FVector>& in)
{
	google::protobuf::RepeatedPtrField<inner_out> out;

	if constexpr (filter_nan)
	{
		TSet<size_t> not_nan;
		for (size_t i = 0; i < in.Num(); ++i)
		{
			if (!in[i].ContainsNaN())
				not_nan.Add(i);
		}
		out.Reserve(not_nan.Num());

		for (const auto& index : not_nan)
			out.Add(convert<inner_out, FVector>(in[index]));
	}
	else
	{
		out.Reserve(in.Num());

		for (const auto& it : in)
			out.Add(convert<inner_out, FVector>(it));
	}
	return out;
}

template<>
Transformation::AxisAlignment convert(const generated::Axis_Alignment& in);

template<>
Transformation::Ratio convert(const generated::Ratio& in);

template<>
Transformation::TransformationMeta convert(const generated::Transformation_Meta& in);

template<>
FVector convert_meta(const generated::vertex_3d& in, const Transformation::TransformationConverter* cv);

template<>
FVector convert_meta(const generated::index_3d& in, const Transformation::TransformationConverter* cv);

template<>
FVector convert_meta(const generated::size_3d& in, const Transformation::TransformationConverter* cv);

template<>
FQuat convert_meta(const generated::quaternion& in, const Transformation::TransformationConverter* cv);

template<>
generated::quaternion convert(const FQuat& in);

template<>
FQuat convert_meta(const generated::Rotation_3d& in, const Transformation::TransformationConverter* cv);

template<>
FColor convert(const generated::color& in);

template<>
TArray<int32> convert(const google::protobuf::RepeatedField<unsigned>& in);

template<>
FString convert(const std::string& in);

template<>
std::string convert(const FString& in);

template<>
F_mesh_data convert_meta(const generated::Mesh_Data& in, const Transformation::TransformationConverter* cv);

template<>
F_mesh_data convert_meta(const generated::Mesh_Data_TF_Meta& in, TF_Conv_Wrapper& cv);

template<>
FBox convert_meta(const generated::aabb& in, const Transformation::TransformationConverter* cv);

template<>
F_object_prototype convert_meta(const generated::Object_Prototype& in, const Transformation::TransformationConverter* cv);

template<>
F_object_prototype convert_meta(const generated::Object_Prototype_TF_Meta& in, TF_Conv_Wrapper& cv);

template<>
F_obb convert_meta(const generated::Obb& in, const Transformation::TransformationConverter* cv);

template<>
generated::size_3d convert(const FVector& in);

template<>
generated::vertex_3d convert(const FVector& in);

//TODO:: is this in use?
template<>
std::array<float, 3> convert(const FVector& in);

template<>
generated::Matrix convert(const FMatrix& in);

template<>
FMatrix convert(const generated::Matrix& in);

template<>
FTransform convert_meta(const generated::Matrix& in, const Transformation::TransformationConverter* cv);

template<>
generated::Matrix convert(const FTransform& in);
/*
template<>
std::unique_ptr<draco::PointCloud> convert(const TArray<FVector>& in);

template<>
generated::draco_data convert(const F_point_cloud& pcl);
*/
template<>
generated::Pcl_Data convert(const F_point_cloud& pcl);

template<>
F_object_data convert_meta(const generated::Object_Data& in, const Transformation::TransformationConverter* cv);

template<>
F_colored_box convert_meta(const generated::Colored_Box& in, const Transformation::TransformationConverter* cv);

/*
 * @attend precondition object_instance.has_obj() == true
 */
template<>
F_object_instance_data convert_meta(const generated::Object_Instance& in, const Transformation::TransformationConverter* cv);

template<>
F_object_instance_data convert_meta(const generated::Object_Instance_TF_Meta& in, TF_Conv_Wrapper& cv);

/*
 * @attend precondition object_instance.has_box() == true
 */
template<>
F_object_instance_colored_box convert_meta(const generated::Object_Instance& in, const Transformation::TransformationConverter* cv);

template<>
F_object_instance_colored_box convert_meta(const generated::Object_Instance_TF_Meta& in, TF_Conv_Wrapper& cv);

template<>
generated::Rotation_3d convert(const FQuat& in);

template<>
generated::aabb convert(const FBox& in);

template<>
generated::Obb convert(const F_obb& in);

template<>
generated::Hand_Data convert(const std::pair<FXRMotionControllerData, FDateTime>& in);

template<>
F_voxel_data convert_meta(const generated::Voxels& in, const Transformation::TransformationConverter* cv);

template<>
F_voxel_data convert_meta(const generated::Voxel_TF_Meta& in, TF_Conv_Wrapper& cv);

template<>
TArray<FVector> convert_meta(const generated::Tcps& in, const Transformation::TransformationConverter* cv);

template<>
TArray<FVector> convert_meta(const generated::Tcps_TF_Meta& in, TF_Conv_Wrapper& cv);

template<>
TOptional<FTransform> convert(const generated::ICP_Result& in);