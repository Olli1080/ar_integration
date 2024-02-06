#pragma once
#include <cstdint>
#include <memory>
#include <ratio>
#include <tuple>
#include <array>
#include <concepts>

#include "Math/Vector.h"
#include "Math/TransformVectorized.h"

//#include "grpc_wrapper.h"

#include "grpc_include_begin.h"
#include "vertex.pb.h"
#include "object_prototype.pb.h"
#include "depth_image.pb.h"
#include "object.pb.h"
#include "hand_tracking.pb.h"
#include "robot.pb.h"
#include "grpc_include_end.h"

//#include "TransformHelper.generated.h"
namespace Transformation
{
	enum class Axis : int8_t
	{
		X = 0,
		Y = 1,
		Z = 2
	};

	enum class AxisDirection : int8_t
	{
		POSITIVE = +1,
		NEGATIVE = -1
	};

	[[nodiscard]] AxisDirection invert(AxisDirection in);

	enum class TransformOperation
	{
		RIGHT_AND_FORWARD,
		RIGHT_AND_UP,
		FORWARD_AND_UP
	};

	struct AxisAlignment
	{
		Axis axis;
		AxisDirection direction;
	};

	struct Ratio
	{
		std::intmax_t Num;
		std::intmax_t Denom;

		Ratio(std::intmax_t Num, std::intmax_t Denom);

		template<std::intmax_t Num, std::intmax_t Denom>
		inline Ratio()
			: Num(Num), Denom(Denom)
		{}

		template<std::intmax_t Num, std::intmax_t Denom>
		inline Ratio(std::ratio<Num, Denom> ratio)
			: Num(Num), Denom(Denom)
		{}

		[[nodiscard]] float factor() const;
		[[nodiscard]] float factor(const Ratio& other) const;
	};

	//column, row, multiplier
	typedef std::tuple<int8_t, int8_t, float> Assignment;
	typedef std::array<Assignment, 3> SparseAssignments;

	class TransformationMeta;
	static Assignment compute_assignment(AxisAlignment axis, AxisAlignment target_axis);
	static SparseAssignments compute_assignments(const TransformationMeta& origin, const TransformationMeta& target);

	class TransformationConverter
	{
	public:

		TransformationConverter(const TransformationMeta& origin, const TransformationMeta& target);

		[[nodiscard]] FTransform get_conv_matrix(const TransformationMeta& target) const;

		[[nodiscard]] FTransform convert_matrix(const FTransform& in) const;
		[[nodiscard]] FQuat convert_quaternion(const FQuat& in) const;
		[[nodiscard]] FVector convert_point(const FVector& in) const;

		[[nodiscard]] FTransform convert_matrix_proto(const generated::Matrix& in) const;
		[[nodiscard]] FQuat convert_quaternion_proto(const generated::quaternion& in) const;
		[[nodiscard]] FVector convert_point_proto(const generated::vertex_3d& in) const;
		[[nodiscard]] FVector convert_index_proto(const generated::index_3d& in) const;
		//[[nodiscard]] FVector convert_point_proto(const generated::vertex_3d& in) const;

		[[nodiscard]] generated::Matrix convert_matrix_proto(const FTransform& in) const;
		[[nodiscard]] generated::quaternion convert_quaternion_proto(const FQuat& in) const;
		[[nodiscard]] generated::vertex_3d convert_point_proto(const FVector& in) const;


		float convert_scale(float scale) const;

	private:

		static FTransform convert(const SparseAssignments& ttt, const FTransform& in, float scale);
		static FTransform convert_proto(const SparseAssignments& ttt, const generated::Matrix& in, float scale);
		static generated::Matrix convert_proto(const SparseAssignments& ttt, const FTransform& in, float scale);

		float factor;
		SparseAssignments assignments;
		bool hand_changed;
	};

	class TransformationMeta
	{
	public:

		TransformationMeta(
			AxisAlignment right,
			AxisAlignment forward,
			AxisAlignment up,
			Ratio scale = { 1, 1 }
		);

		TransformationMeta(const TransformationMeta& other);

		[[nodiscard]] bool isRightHanded() const;
		[[nodiscard]] bool isLeftHanded() const;

		Ratio scale;

		const AxisAlignment& right() const;
		const AxisAlignment& forward() const;
		const AxisAlignment& up() const;

	private:

		void rotateLeft();
		void rotateRight();


		AxisAlignment m_right;
		AxisAlignment m_forward;
		AxisAlignment m_up;

		mutable std::unique_ptr<bool> right_handed;
	};

	inline static Transformation::TransformationMeta UnrealMeta(
		{ Transformation::Axis::Y, Transformation::AxisDirection::POSITIVE },
		{ Transformation::Axis::X, Transformation::AxisDirection::POSITIVE },
		{ Transformation::Axis::Z, Transformation::AxisDirection::POSITIVE },
		{ std::centi{} }
	);
}