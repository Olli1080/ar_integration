#include "TransformHelper.h"

namespace Transformation
{
	TransformationMeta::TransformationMeta(AxisAlignment right, AxisAlignment forward, AxisAlignment up,
		Ratio scale)
		: scale(scale), m_right(right), m_forward(forward), m_up(up)
	{
		if (right.axis == forward.axis || forward.axis == up.axis || right.axis == up.axis)
			throw std::exception("The same axis occurs twice!");
	}

	TransformationMeta::TransformationMeta(const TransformationMeta& other)
		: scale(other.scale), m_right(other.m_right), m_forward(other.m_forward), m_up(other.m_up)
	{
		if (other.right_handed)
			right_handed = std::make_unique<bool>(*other.right_handed);
	}

	bool TransformationMeta::isRightHanded() const
	{
		if (right_handed)
			return *right_handed;

		auto cpy = *this;

		//1. normalize to positive X axis as right axis
		if (m_right.axis == Axis::X)
		{
			//compare
		}
		else if (m_forward.axis == Axis::X)
		{
			cpy.rotateLeft();
		}
		else //up.axis == Axis::X
		{
			cpy.rotateRight();
		}
		if (cpy.m_right.direction == AxisDirection::NEGATIVE)
		{
			cpy.m_right.direction = AxisDirection::POSITIVE;
			cpy.m_forward.direction = invert(cpy.m_forward.direction);
		}
		const bool fw_up_eq = cpy.m_forward.direction == cpy.m_up.direction;

		//if (X, Y, Z) then the sign of Y and Z must be equal otherwise they are different
		//e.g. (X, Z, Y)
		right_handed = std::make_unique<bool>((cpy.m_forward.axis == Axis::Y) ? fw_up_eq : !fw_up_eq);
		return *right_handed;
	}

	/**
	 * convenience method
	 */
	bool TransformationMeta::isLeftHanded() const
	{
		return !isRightHanded();
	}

	FTransform TransformationConverter::get_conv_matrix(const TransformationMeta& target) const
	{
		FMatrix matrix;
		auto& M = matrix.M;

		for (const auto& [column, row, multiplier] : assignments)
		{
			//doesn't matter if we go over x or y for the result
			//only matters for cache performance
			for (int8_t x = 0; x < 3; ++x)
			{
				if (x == column)
					M[column][row] = multiplier * factor;
				else
					M[x][row] = 0.f;
			}
		}

		return FTransform{ matrix };
	}

	FTransform TransformationConverter::convert_matrix(const FTransform& in) const
	{
		return convert(assignments, in, factor);
	}

	FQuat TransformationConverter::convert_quaternion(const FQuat& in) const
	{
		FQuat out;

		if (hand_changed)
			out.W = -in.W;
		else
			out.W = in.W;

		static_assert(sizeof(FQuat) == 4 * sizeof(double), "Engine related code changed; Fix this!");

		auto coeffs = &out.X;
		const auto in_c = &in.X;

		for (const auto& [column, row, multiplier] : assignments)
			coeffs[row] = in_c[column] * multiplier;

		return out;
	}

	FVector TransformationConverter::convert_point(const FVector& in_f) const
	{
		FVector out_f;

		static_assert(sizeof(FVector) == 3 * sizeof(double), "Engine related code changed; Fix this!");

		auto out = &out_f.X;
		const auto in = &in_f.X;

		for (const auto& [column, row, multiplier] : assignments)
			out[row] = in[column] * factor * multiplier;

		return out_f;
	}

	FTransform TransformationConverter::convert_matrix_proto(const generated::Matrix& in) const
	{
		return convert_proto(assignments, in, factor);
	}

	FQuat TransformationConverter::convert_quaternion_proto(const generated::quaternion& in) const
	{
		FQuat out;

		if (hand_changed)
			out.W = -in.w();
		else
			out.W = in.w();

		static_assert(sizeof(FQuat) == 4 * sizeof(double), "Engine related code changed; Fix this!");

		auto coeffs = &out.X;
		const auto in_c = std::to_array<float>({ in.x(), in.y(), in.z() });

		for (const auto& [column, row, multiplier] : assignments)
			coeffs[row] = in_c[column] * multiplier;

		return out;
	}

	FVector TransformationConverter::convert_point_proto(const generated::vertex_3d& in_f) const
	{
		FVector out_f;

		static_assert(sizeof(FVector) == 3 * sizeof(double), "Engine related code changed; Fix this!");

		auto out = &out_f.X;
		const auto in = std::to_array({ in_f.x(), in_f.y(), in_f.z()});

		for (const auto& [column, row, multiplier] : assignments)
			out[row] = in[column] * factor * multiplier;

		return out_f;
	}

	FVector TransformationConverter::convert_index_proto(const generated::index_3d& in_f) const
	{
		FVector out_f;

		static_assert(sizeof(FVector) == 3 * sizeof(double), "Engine related code changed; Fix this!");

		auto out = &out_f.X;
		const auto in = std::to_array({ in_f.x(), in_f.y(), in_f.z() });

		for (const auto& [column, row, multiplier] : assignments)
			out[row] = static_cast<float>(in[column]) * multiplier;

		return out_f;
	}

	FVector TransformationConverter::convert_size_proto(const generated::size_3d& in_f) const
	{
		FVector out_f;

		static_assert(sizeof(FVector) == 3 * sizeof(double), "Engine related code changed; Fix this!");

		auto out = &out_f.X;
		const auto in = std::to_array({ in_f.x(), in_f.y(), in_f.z() });

		for (const auto& [column, row, multiplier] : assignments)
			out[row] = in[column] * factor;

		return out_f;
	}

	generated::Matrix TransformationConverter::convert_matrix_proto(const FTransform& in) const
	{
		return convert_proto(assignments, in, factor);
	}

	generated::quaternion TransformationConverter::convert_quaternion_proto(const FQuat& in) const
	{
		generated::quaternion out;

		if (hand_changed)
			out.set_w(-in.W);
		else
			out.set_w(in.W);

		static_assert(sizeof(FQuat) == 4 * sizeof(double), "Engine related code changed; Fix this!");

		auto coeffs = std::to_array({
			&generated::quaternion::set_x,
			&generated::quaternion::set_y,
			&generated::quaternion::set_z,
		});
		const auto in_c = &in.X;

		for (const auto& [column, row, multiplier] : assignments)
			(out.*coeffs[row])(in_c[column] * multiplier);

		return out;
	}

	generated::vertex_3d TransformationConverter::convert_point_proto(const FVector& in_f) const
	{
		generated::vertex_3d out_f;

		static_assert(sizeof(FVector) == 3 * sizeof(double), "Engine related code changed; Fix this!");

		auto out = std::to_array({
			&generated::vertex_3d::set_x,
			&generated::vertex_3d::set_y,
			&generated::vertex_3d::set_z,
			});
		const auto in = &in_f.X;

		for (const auto& [column, row, multiplier] : assignments)
			(out_f.*out[row])(in[column] * factor * multiplier);

		return out_f;
	}

	float TransformationConverter::convert_scale(float scale) const
	{
		return factor * scale;
	}

	FTransform TransformationConverter::convert(const SparseAssignments& ttt, const FTransform& in, float scale)
	{
		FMatrix matrix;
		auto& M = matrix.M;
		for (size_t x = 0; x < 3; ++x)
			M[x][3] = 0.f;
		M[3][3] = 1.f;

		const auto in_Matrix = in.ToMatrixWithScale();
		const auto& in_M = in_Matrix.M;

		for (size_t y = 0; y < 3; ++y)
		{
			//column == y
			const auto& [column, out_row, multiplier_y] = ttt[y];

			for (size_t x = 0; x < 3; ++x)
			{
				//exploiting symmetry //row == x
				const auto& [row, out_column, multiplier_x] = ttt[x];
				M[out_column][out_row] = in_M[x][y] * multiplier_y * multiplier_x;
			}
			M[3][out_row] = in_M[3][y] * multiplier_y * scale;
		}
		return FTransform{ matrix };
	}

	FTransform TransformationConverter::convert_proto(const SparseAssignments& ttt, const generated::Matrix& in, float scale)
	{
		FMatrix matrix;
		auto& M = matrix.M;
		for (size_t x = 0; x < 3; ++x)
			M[x][3] = 0.f;
		M[3][3] = 1.f;

		for (size_t y = 0; y < 3; ++y)
		{
			//column == y
			const auto& [column, out_row, multiplier_y] = ttt[y];

			for (size_t x = 0; x < 3; ++x)
			{
				//exploiting symmetry //row == x
				const auto& [row, out_column, multiplier_x] = ttt[x];
				M[out_column][out_row] = in.data()[x * 4 + y] * multiplier_y * multiplier_x;
			}
			M[3][out_row] = in.data()[3 * 4 + y] * multiplier_y * scale;
		}
		return FTransform{ matrix };
	}

	generated::Matrix TransformationConverter::convert_proto(const SparseAssignments& ttt, const FTransform& in, float scale)
	{
		generated::Matrix matrix;
		matrix.set_cols(4);
		matrix.set_rows(4);
		auto& M = *matrix.mutable_data();
		M.Resize(16, 0);
		
		M.Set(15, 1.f);

		const auto in_Matrix = in.ToMatrixWithScale();
		const auto& in_M = in_Matrix.M;

		for (size_t y = 0; y < 3; ++y)
		{
			//column == y
			const auto& [column, out_row, multiplier_y] = ttt[y];

			for (size_t x = 0; x < 3; ++x)
			{
				//exploiting symmetry //row == x
				const auto& [row, out_column, multiplier_x] = ttt[x];
				M.Set(out_column * 4 + out_row, in_M[x][y] * multiplier_y * multiplier_x);
			}
			M.Set(4 * 3 + out_row, in_M[3][y] * multiplier_y * scale);
		}
		return matrix;
	}

	void TransformationMeta::rotateLeft()
	{
		const auto tmp = m_right;
		m_right = m_forward;
		m_forward = m_up;
		m_up = tmp;
	}

	void TransformationMeta::rotateRight()
	{
		const auto tmp = m_up;
		m_up = m_forward;
		m_forward = m_right;
		m_right = tmp;
	}

	Assignment compute_assignment(AxisAlignment axis, AxisAlignment target_axis)
	{
		//column, row, multiplier
		return {
			static_cast<int8_t>(axis.axis),
			static_cast<int8_t>(target_axis.axis),
			static_cast<float>(axis.direction) * static_cast<float>(target_axis.direction)
		};
	}

	SparseAssignments compute_assignments(const TransformationMeta& origin, const TransformationMeta& target)
	{
		SparseAssignments ttt;
		auto tmp = compute_assignment(origin.right(), target.right());
		ttt[std::get<0>(tmp)] = std::move(tmp);

		tmp = compute_assignment(origin.forward(), target.forward());
		ttt[std::get<0>(tmp)] = std::move(tmp);

		tmp = compute_assignment(origin.up(), target.up());
		ttt[std::get<0>(tmp)] = std::move(tmp);

		return ttt;
	}

	AxisDirection invert(AxisDirection in)
	{
		if (in == AxisDirection::POSITIVE)
			return AxisDirection::NEGATIVE;
		return AxisDirection::POSITIVE;
	}

	Ratio::Ratio(std::intmax_t Num, std::intmax_t Denom)
		: Num(Num), Denom(Denom)
	{}

	float Ratio::factor() const
	{
		return static_cast<float>(Num) / static_cast<float>(Denom);
	}

	float Ratio::factor(const Ratio& other) const
	{
		return static_cast<float>(Num * other.Denom) / static_cast<float>(Denom * other.Num);
	}

	TransformationConverter::TransformationConverter(const TransformationMeta& origin, const TransformationMeta& target)
		: factor(origin.scale.factor(target.scale)), assignments(compute_assignments(origin, target)), hand_changed(origin.isRightHanded() != target.isRightHanded())
	{}

	const AxisAlignment& TransformationMeta::right() const
	{
		return m_right;
	}

	const AxisAlignment& TransformationMeta::forward() const
	{
		return m_forward;
	}

	const AxisAlignment& TransformationMeta::up() const
	{
		return m_up;
	}
}