#include "util.h"

template<>
Transformation::AxisAlignment convert(const generated::Axis_Alignment& in)
{
	return {
		static_cast<Transformation::Axis>(in.axis()),
		static_cast<Transformation::AxisDirection>(in.direction())
	};
}

template<>
Transformation::TransformationMeta convert(const generated::Transformation_Meta& in)
{
	return {
		convert<Transformation::AxisAlignment>(in.right()),
		convert<Transformation::AxisAlignment>(in.forward()),
		convert<Transformation::AxisAlignment>(in.up())
	};
}

template<>
FVector convert(const generated::vertex_3d& in)
{
	return FVector(in.x(), in.y(), in.z());
}

template<>
FVector convert(const generated::vertex_3d_ui& in)
{
	return FVector(in.x(), in.y(), in.z());
}

template<>
FVector convert(const generated::size_3d& in)
{
	return FVector(in.x(), in.y(), in.z());
}

template<>
FQuat convert(const generated::quaternion& in)
{
	return FQuat(in.x(), in.y(), in.z(), in.w());
}

template<>
generated::quaternion convert(const FQuat& in)
{
	FQuat temp = in;
	//temp.X *= -1;
	//temp.W *= -1;

	temp.Normalize();

	generated::quaternion out;

	out.set_x(temp.X);
	out.set_y(temp.Y);
	out.set_z(temp.Z);
	out.set_w(temp.W);

	return out;
}

template<>
FQuat convert(const generated::vertex_3d& in)
{
	auto out = FQuat::MakeFromEuler(FVector(
		FMath::RadiansToDegrees(in.x()),
		FMath::RadiansToDegrees(in.y()),
		FMath::RadiansToDegrees(in.z())));

	//out.X *= -1;
	//out.W *= -1;

	return out;
}

template<>
FColor convert(const generated::color& in)
{
	return FColor(in.r(), in.g(), in.b(), in.a());
}

template<>
TArray<int32> convert(const google::protobuf::RepeatedField<unsigned>& in)
{
	TArray<int32> out;
	out.Reserve(in.size());

	for (const auto& it : in)
		out.Add(it);

	return out;
}

template<>
FString convert(const std::string& in)
{
	return FString(in.c_str());
}

template<>
std::string convert(const FString& in)
{
	return std::string(TCHAR_TO_UTF8(*in));
}

template<>
F_mesh_data convert(const generated::mesh_data& in)
{
	F_mesh_data out;
	out.vertices = convert_array<FVector>(in.vertices());
	out.indices = convert<TArray<int32>>(in.indices());
	out.name = convert<FString>(in.name());

	if (in.has_vertex_normals())
		out.normals = convert_array<FVector>(in.vertex_normals().vertices());
	if (in.has_vertex_colors())
		out.colors = convert_array<FColor>(in.vertex_colors().colors());

	return out;
}

template<>
FBox convert(const generated::aabb& in)
{
	return FBox::BuildAABB(
		convert<FVector>(in.translation()),
		convert<FVector>(in.diagonal()) / 2.f
	);
}

template<>
F_object_prototype convert(const generated::object_prototype& in)
{
	F_object_prototype out;
	out.name = convert<FString>(in.name());
	out.mean_color = convert<FColor>(in.mean_color());
	out.bounding_box = convert<FBox>(in.bounding_box());
	out.mesh_name = convert<FString>(in.mesh_name());
	out.type = convert<FString>(in.type());

	return out;
}

template<>
F_obb convert(const generated::obb& in)
{
	F_obb out;
	out.axis_box = convert<FBox>(in.axis_aligned());
	out.rotation = convert<FQuat>(in.rotation());

	return out;
}

template<>
generated::size_3d convert(const FVector& in)
{
	generated::size_3d out;
	out.set_x(in.X);
	out.set_y(in.Y);
	out.set_z(in.Z);

	return out;
}

template<>
generated::vertex_3d convert(const FVector& in)
{
	generated::vertex_3d out;
	out.set_x(in.X);
	out.set_y(in.Y);
	out.set_z(in.Z);

	return out;
}

template<>
generated::Matrix convert(const FMatrix& in)
{
	generated::Matrix out;
	out.set_rows(4);
	out.set_cols(4);
	const auto data = out.mutable_data();
	data->Reserve(16);
	for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
			*data->Add() = in.M[y][x];

	return out;
}

template<>
FMatrix convert(const generated::Matrix& in)
{
	FMatrix out;

	if (in.rows() != 4 ||
		in.cols() != 4 ||
		in.data_size() != 16)
		return out;

	for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
			out.M[y][x] = in.data()[y * 4 + x];

	return out;
}

template<>
FTransform convert(const generated::Matrix& in)
{
	FMatrix temp;

	if (in.rows() != 4 ||
		in.cols() != 4 ||
		in.data_size() != 16)
		return {};

	for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
			temp.M[y][x] = in.data()[y * 4 + x];

	//apply_mask(temp);

	return FTransform(std::move(temp));
}

template<>
generated::Matrix convert(const FTransform& in)
{
	generated::Matrix out;
	out.set_rows(4);
	out.set_cols(4);
	const auto data = out.mutable_data();
	data->Reserve(16);

	const auto temp = in.ToMatrixWithScale();
	//apply_mask(temp);

	for (size_t y = 0; y < 4; ++y)
		for (size_t x = 0; x < 4; ++x)
			data->Add(temp.M[y][x]);

	return out;
}

template<>
std::unique_ptr<draco::PointCloud> convert(const TArray<FVector>& in)
{
	TSet<size_t> not_nan;
	for (size_t i = 0; i < in.Num(); ++i)
	{
		if (!in[i].ContainsNaN())
			not_nan.Add(i);
	}

	draco::PointCloudBuilder builder;
	builder.Start(not_nan.Num());

	const int att_id = builder.AddAttribute(draco::GeometryAttribute::POSITION, 3, draco::DataType::DT_FLOAT32);
	const auto data = convert_std_array<std::array<float, 3>, true>(in);

	for (const auto& idx : not_nan)
		builder.SetAttributeValueForPoint(att_id, draco::PointIndex(idx), data.data() + 3 * idx);

	return builder.Finalize(false);
}

template<>
generated::draco_data convert(const F_point_cloud& pcl)
{
	generated::draco_data request;

	const auto draco_cloud = convert<std::unique_ptr<draco::PointCloud>>(pcl.data);

	draco::EncoderBuffer buffer;
	draco::PointCloudKdTreeEncoder encoder;
	draco::EncoderOptions options = draco::EncoderOptions::CreateDefaultOptions();
	options.SetSpeed(4, 1);
	encoder.SetPointCloud(*draco_cloud);
	encoder.Encode(options, &buffer);

	request.set_data(buffer.data(), buffer.size());
	request.set_timestamp(pcl.abs_timestamp);

	return request;
}

template<>
generated::pcl_data convert(const F_point_cloud& pcl)
{
	generated::pcl_data request;
	request.mutable_vertices()->CopyFrom(
		convert_array<generated::vertex_3d, true>(pcl.data));

	request.set_timestamp(pcl.abs_timestamp);

	return request;
}

template<>
F_object_data convert(const generated::object_data& in)
{
	F_object_data out;
	out.prototype_name = convert<FString>(in.prototype_name());
	out.transform = convert<FTransform>(in.transform());

	return out;
}

template<>
F_colored_box convert(const generated::colored_box& in)
{
	F_colored_box out;
	out.box = convert<F_obb>(in.obbox());
	out.color = convert<FColor>(in.box_color());

	return out;
}

template<>
F_object_instance_data convert(const generated::object_instance& in)
{
	F_object_instance_data out;

	out.id = convert<FString>(in.id());
	out.data = convert<F_object_data>(in.obj());

	return out;
}

template<>
F_object_instance_colored_box convert(const generated::object_instance& in)
{
	F_object_instance_colored_box out;

	out.id = convert<FString>(in.id());
	out.data = convert<F_colored_box>(in.box());

	return out;
}

template<>
generated::vertex_3d convert(const FQuat& in)
{
	generated::vertex_3d out;

	FQuat cpy = in;

	//cpy.X *= -1;
	//cpy.W *= -1;

	const auto& rot = cpy.Rotator().Euler();

	out.set_x(FMath::DegreesToRadians(rot.X));
	out.set_y(FMath::DegreesToRadians(rot.Y));
	out.set_z(FMath::DegreesToRadians(rot.Z));

	return out;
}

template<>
generated::aabb convert(const FBox& in)
{
	generated::aabb out;
	*out.mutable_diagonal() = convert<generated::size_3d>(in.GetExtent() * 2.f);
	*out.mutable_translation() = convert<generated::vertex_3d>(in.GetCenter());

	return out;
}

template<>
generated::obb convert(const F_obb& in)
{
	generated::obb out;

	*out.mutable_rotation() = convert<generated::quaternion>(in.rotation);
	*out.mutable_axis_aligned() = convert<generated::aabb>(in.axis_box);

	return out;
}

template<>
generated::hand_data convert(const std::pair<FXRMotionControllerData, FDateTime>& in)
{
	generated::hand_data out;

	const auto& hand_data = in.first;

	out.set_valid(hand_data.bValid);
	out.set_hand(static_cast<generated::hand_index>(hand_data.HandIndex));
	out.set_tracking_stat(static_cast<generated::tracking_status>(hand_data.TrackingStatus));

	if (hand_data.bValid)
	{
		*out.mutable_grip_position() = convert<generated::vertex_3d>(hand_data.GripPosition);
		*out.mutable_grip_rotation() = convert<generated::quaternion>(hand_data.GripRotation);
		*out.mutable_aim_position() = convert<generated::vertex_3d>(hand_data.AimPosition);
		*out.mutable_aim_rotation() = convert<generated::quaternion>(hand_data.AimRotation);
		*out.mutable_hand_key_positions() = convert_array<generated::vertex_3d>(hand_data.HandKeyPositions);
		*out.mutable_hand_key_rotations() = convert_array<generated::quaternion>(hand_data.HandKeyRotations);

		const auto mutable_radii = out.mutable_hand_key_radii();
		mutable_radii->Reserve(hand_data.HandKeyRadii.Num());
		for (const float& f : hand_data.HandKeyRadii)
			mutable_radii->Add(f);
	}
	out.set_is_grasped(hand_data.bIsGrasped);
	out.set_utc_timestamp(in.second.GetTicks());

	return out;
}

template<>
F_voxel_data convert(const generated::Voxels& in)
{
	F_voxel_data res;
	res.voxel_side_length = in.voxel_side_length();
	res.robot_origin = convert<FTransform>(in.robot_origin());
	res.indices = convert_array<FVector>(in.voxel_coords());

	return res;
}

template<>
F_voxel_data convert_meta(const generated::Voxel_TF_Meta& in, TF_Conv_Wrapper& cv)
{
	using namespace Transformation;
	if (in.has_transformation_meta())
		cv.set_source(convert<TransformationMeta>(in.transformation_meta()));

	const auto& voxels = in.voxels();

	if (!cv.has_converter())
		return convert<F_voxel_data>(voxels);

	F_voxel_data res;
	const auto& converter = cv.converter();

	res.voxel_side_length = converter.convert_scale(voxels.voxel_side_length());
	res.robot_origin = converter.convert_matrix_proto(voxels.robot_origin());

	res.indices.Reserve(voxels.voxel_coords_size());
	for (const auto& p : voxels.voxel_coords())
		res.indices.Add(converter.convert_index_proto(p));

	return res;
}

template<>
TArray<FVector> convert(const generated::Tcps& in)
{
	return convert_array<FVector>(in.points());
}

template<>
TArray<FVector> convert_meta(const generated::Tcps_TF_Meta& in, TF_Conv_Wrapper& cv)
{
	const auto& points = in.tcps().points();

	using namespace Transformation;
	if (in.has_transformation_meta())
		cv.set_source(convert<TransformationMeta>(in.transformation_meta()));

	if (!cv.has_converter())
		return convert_array<FVector>(points);

	TArray<FVector> out;
	out.Reserve(points.size());

	for (const auto& p : points)
		out.Add(cv.converter().convert_point_proto(p));

	return out;
}

template<>
TOptional<FTransform> convert(const generated::ICP_Result& in)
{
	if (!in.has_data())
		return {};

	const auto& data = in.data();

	if (!data.has_transformation_meta())
		return convert<FTransform>(data.matrix());

	using namespace Transformation;
	return TransformationConverter(
		convert<TransformationMeta>(data.transformation_meta()), UnrealMeta
	).convert_matrix_proto(data.matrix());
}

template<>
std::array<float, 3> convert(const FVector& in)
{
	std::array<float, 3> out;
	out[0] = in.X;
	out[1] = in.Y;
	out[2] = in.Z;

	return out;
}

void TF_Conv_Wrapper::set_source(const Transformation::TransformationMeta& meta)
{
	m_converter = std::make_unique<Transformation::TransformationConverter>(meta, Transformation::UnrealMeta);
}

const Transformation::TransformationConverter& TF_Conv_Wrapper::converter() const
{
	return *m_converter;
}

bool TF_Conv_Wrapper::has_converter() const
{
	return !!m_converter;
}