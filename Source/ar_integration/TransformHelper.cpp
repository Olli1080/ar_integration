#include "TransformHelper.h"

namespace Transformation
{
	TransformationConverterWrapper::TransformationConverterWrapper(const TransformationMeta& origin, const TransformationMeta& target)
		: converter(origin, target)
	{}

	FTransform TransformationConverterWrapper::convert_matrix(const FTransform& in) const
	{
		FMatrix inM = in.ToMatrixWithScale();
		FMatrix outM;
		converter.convert_matrix(inM, outM);
		return FTransform(outM);
	}

	FQuat TransformationConverterWrapper::convert_quaternion(const FQuat& in) const
	{
		FQuat out;
		converter.convert_quaternion(in, out);
		return out;
	}

	FVector TransformationConverterWrapper::convert_point(const FVector& in) const
	{
		FVector out;
		converter.convert_point(in, out);
		return out;
	}

	FTransform TransformationConverterWrapper::convert_matrix_proto(const generated::Matrix& in) const
	{
		FMatrix outM;
		converter.convert_matrix(in, outM);
		return FTransform(outM);
	}

	FQuat TransformationConverterWrapper::convert_quaternion_proto(const generated::quaternion& in) const
	{
		FQuat out;
		converter.convert_quaternion(in, out);
		return out;
	}

	FVector TransformationConverterWrapper::convert_point_proto(const generated::vertex_3d& in) const
	{
		FVector out;
		converter.convert_point(in, out);
		return out;
	}

	FVector TransformationConverterWrapper::convert_point_proto(const generated::vertex_3d_no_scale& in) const
	{
		FVector out;
		converter.convert_point(in, out);
		return out;
	}

	FVector TransformationConverterWrapper::convert_index_proto(const generated::index_3d& in) const
	{
		FVector out;
		converter.convert_point(in, out);
		return out;
	}

	FVector TransformationConverterWrapper::convert_size_proto(const generated::size_3d& in) const
	{
		FVector out;
		converter.convert_point(in, out);
		return out;
	}

	generated::Matrix TransformationConverterWrapper::convert_matrix_proto(const FTransform& in) const
	{
		generated::Matrix out;
		out.set_cols(4);
		out.set_rows(4);
		for(int i=0; i<16; ++i) out.add_data(0.0f); 
		
		converter.convert_matrix(in.ToMatrixWithScale(), out);
		return out;
	}

	generated::quaternion TransformationConverterWrapper::convert_quaternion_proto(const FQuat& in) const
	{
		generated::quaternion out;
		converter.convert_quaternion(in, out);
		return out;
	}

	generated::vertex_3d TransformationConverterWrapper::convert_point_proto(const FVector& in) const
	{
		generated::vertex_3d out;
		converter.convert_point(in, out);
		return out;
	}

	double TransformationConverterWrapper::convert_scale(double scale) const
	{
		return converter.convert_scale(scale);
	}
}
