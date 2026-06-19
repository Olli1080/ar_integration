#pragma once
#include <cstdint>
#include <memory>
#include <ratio>
#include <tuple>
#include <array>

#include "Math/Vector.h"
#include "Math/TransformVectorized.h"

#include "grpc_include_begin.h"
#include "vertex.pb.h"
#include "object_prototype.pb.h"
#include "depth_image.pb.h"
#include "object.pb.h"
#include "hand_tracking.pb.h"
#include "robot.pb.h"
#include <base-transformation/TransformationHelper.h>
#include <base-transformation/Presets.h>

// --- Traits for Unreal Engine types ---
template<> struct Transformation::TraitsEnabled<FVector> : std::true_type {};
template<> struct Transformation::VectorTraits<FVector, double> {
	using type = FVector;
	static double get_x(const type& v) { return v.X; }
	static double get_y(const type& v) { return v.Y; }
	static double get_z(const type& v) { return v.Z; }
	static double get_idx(const type& v, std::size_t i) { return v[i]; }
	static void set_x(type& v, double val) { v.X = val; }
	static void set_y(type& v, double val) { v.Y = val; }
	static void set_z(type& v, double val) { v.Z = val; }
	static void set_idx(type& v, std::size_t i, double val) { v[i] = val; }
};

template<> struct Transformation::TraitsEnabled<FQuat> : std::true_type {};
template<> struct Transformation::QuaternionTraits<FQuat, double> {
	using type = FQuat;
	static double get_x(const type& q) { return q.X; }
	static double get_y(const type& q) { return q.Y; }
	static double get_z(const type& q) { return q.Z; }
	static double get_w(const type& q) { return q.W; }
	static double get_idx(const type& q, std::size_t i) { 
        if (i == 0) return q.X; if (i == 1) return q.Y; if (i == 2) return q.Z; return q.W;
    }
	static void set_x(type& q, double val) { q.X = val; }
	static void set_y(type& q, double val) { q.Y = val; }
	static void set_z(type& q, double val) { q.Z = val; }
	static void set_w(type& q, double val) { q.W = val; }
	static void set_idx(type& q, std::size_t i, double val) { 
        if (i == 0) q.X = val; else if (i == 1) q.Y = val; else if (i == 2) q.Z = val; else q.W = val;
    }
};

template<> struct Transformation::TraitsEnabled<FMatrix> : std::true_type {};
template<> struct Transformation::MatrixTraits<FMatrix, double> {
	using type = FMatrix;
	static constexpr std::size_t size = 4;
	static double get(const type& m, std::size_t row, std::size_t col) { return m.M[row][col]; }
	static void set(type& m, std::size_t row, std::size_t col, double val) { m.M[row][col] = val; }
};

// --- Traits for Protobuf types ---
#define DEFINE_PROTO_VECTOR_TRAITS(ProtoType) \
template<> struct Transformation::TraitsEnabled<ProtoType> : std::true_type {}; \
template<> struct Transformation::VectorTraits<ProtoType, double> { \
	using type = ProtoType; \
	static double get_x(const type& v) { return v.x(); } \
	static double get_y(const type& v) { return v.y(); } \
	static double get_z(const type& v) { return v.z(); } \
	static double get_idx(const type& v, std::size_t i) { return i == 0 ? v.x() : (i == 1 ? v.y() : v.z()); } \
	static void set_x(type& v, double val) { v.set_x((float)val); } \
	static void set_y(type& v, double val) { v.set_y((float)val); } \
	static void set_z(type& v, double val) { v.set_z((float)val); } \
	static void set_idx(type& v, std::size_t i, double val) { if (i == 0) v.set_x((float)val); else if (i == 1) v.set_y((float)val); else v.set_z((float)val); } \
};

DEFINE_PROTO_VECTOR_TRAITS(generated::vertex_3d)
DEFINE_PROTO_VECTOR_TRAITS(generated::vertex_3d_no_scale)
DEFINE_PROTO_VECTOR_TRAITS(generated::index_3d)
DEFINE_PROTO_VECTOR_TRAITS(generated::size_3d)

template<> struct Transformation::TraitsEnabled<generated::quaternion> : std::true_type {};
template<> struct Transformation::QuaternionTraits<generated::quaternion, double> {
	using type = generated::quaternion;
	static double get_x(const type& q) { return q.x(); }
	static double get_y(const type& q) { return q.y(); }
	static double get_z(const type& q) { return q.z(); }
	static double get_w(const type& q) { return q.w(); }
	static double get_idx(const type& q, std::size_t i) { return i == 0 ? q.x() : (i == 1 ? q.y() : (i == 2 ? q.z() : q.w())); }
	static void set_x(type& q, double val) { q.set_x((float)val); }
	static void set_y(type& q, double val) { q.set_y((float)val); }
	static void set_z(type& q, double val) { q.set_z((float)val); }
	static void set_w(type& q, double val) { q.set_w((float)val); }
	static void set_idx(type& q, std::size_t i, double val) { if (i == 0) q.set_x((float)val); else if (i == 1) q.set_y((float)val); else if (i == 2) q.set_z((float)val); else q.set_w((float)val); }
};

template<> struct Transformation::TraitsEnabled<generated::Matrix> : std::true_type {};
template<> struct Transformation::MatrixTraits<generated::Matrix, double> {
	using type = generated::Matrix;
	static constexpr std::size_t size = 4;
	static double get(const type& m, std::size_t row, std::size_t col) { return m.data()[row * 4 + col]; }
	static void set(type& m, std::size_t row, std::size_t col, double val) { m.set_data(row * 4 + col, (float)val); }
};

namespace Transformation
{
	class TransformationConverterWrapper
	{
	public:
		TransformationConverterWrapper(const TransformationMeta& origin, const TransformationMeta& target);

		[[nodiscard]] FTransform convert_matrix(const FTransform& in) const;
		[[nodiscard]] FQuat convert_quaternion(const FQuat& in) const;
		[[nodiscard]] FVector convert_point(const FVector& in) const;

		[[nodiscard]] FTransform convert_matrix_proto(const generated::Matrix& in) const;
		[[nodiscard]] FQuat convert_quaternion_proto(const generated::quaternion& in) const;
		[[nodiscard]] FVector convert_point_proto(const generated::vertex_3d& in) const;
		[[nodiscard]] FVector convert_point_proto(const generated::vertex_3d_no_scale& in) const;
		[[nodiscard]] FVector convert_index_proto(const generated::index_3d& in) const;
		[[nodiscard]] FVector convert_size_proto(const generated::size_3d& in) const;

		[[nodiscard]] generated::Matrix convert_matrix_proto(const FTransform& in) const;
		[[nodiscard]] generated::quaternion convert_quaternion_proto(const FQuat& in) const;
		[[nodiscard]] generated::vertex_3d convert_point_proto(const FVector& in) const;

		[[nodiscard]] double convert_scale(double scale) const;

	private:
		TransformationConverter<double> converter;
	};


	inline static Transformation::TransformationMeta UnrealMeta() {
		return Presets::Unreal();
	}
}
