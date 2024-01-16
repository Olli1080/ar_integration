#pragma once

#include "pch.h"
#if (PLATFORMS)

#include "Math/Matrix.h"
#include "Math/Plane.h"

namespace Spatial = winrt::Windows::Perception::Spatial;
namespace Numerics = winrt::Windows::Foundation::Numerics;
namespace Perception = winrt::Windows::Perception;

template<typename out, typename in>
out convert(const in&);

template<>
inline FMatrix convert(const Numerics::float4x4& in)
{
    return FMatrix(
        FPlane(in.m11, in.m12, in.m13, in.m14),
        FPlane(in.m21, in.m22, in.m23, in.m24),
        FPlane(in.m31, in.m32, in.m33, in.m34),
        FPlane(in.m41, in.m42, in.m43, in.m44));
}

template<>
inline FTransform convert(const Numerics::float4x4& in)
{
	return FTransform(FMatrix(
        FPlane(in.m33, -in.m31, -in.m32, -in.m34),
        FPlane(-in.m13, in.m11, in.m12, in.m14),
        FPlane(-in.m23, in.m21, in.m22, in.m24),
        FPlane(-in.m43, in.m41, in.m42, in.m44)));
}

template<>
inline FMatrix convert(const DirectX::XMFLOAT4X4& in)
{
    return FMatrix(
        FPlane(in._11, in._12, in._13, in._14),
        FPlane(in._21, in._22, in._23, in._24),
        FPlane(in._31, in._32, in._33, in._34),
        FPlane(in._41, in._42, in._43, in._44));
}

template<>
inline FTransform convert(const DirectX::XMFLOAT4X4& in)
{
    return FTransform(FMatrix(
        FPlane(in._33, -in._31, -in._32, -in._34),
        FPlane(-in._13, in._11, in._12, in._14),
        FPlane(-in._23, in._21, in._22, in._24),
        FPlane(-in._43, in._41, in._42, in._44)));
}

template<>
inline Numerics::float4x4 convert(const Spatial::SpatialLocation& in)
{
    return Numerics::make_float4x4_from_quaternion(in.Orientation()) *
        Numerics::make_float4x4_translation(in.Position());
}

template<>
inline FTransform convert(const Spatial::SpatialLocation& in)
{
    return convert<FTransform>(convert<Numerics::float4x4>(in));
}

#endif