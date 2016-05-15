#pragma once
#include <math.h> //sqrtf sqrt fabsf pow powf

const unsigned int MaxUInt32 = 0xFFFFFFFF;
const int MinInt32 = 0x80000000;
const int MaxInt32 = 0x7FFFFFFF;
const float MaxFloat = 3.402823466e+38F;
const float MinPosFloat = 1.175494351e-38F;

const float Pi = 3.141592654f;
const float TwoPi = 6.283185307f;
const float PiHalf = 1.570796327f;
const float _1OverPi = 1.0f / Pi;
const float _1Over2Pi = 1.0f / TwoPi;
const float PiOver180 = Pi / 180.0f;
const float _180OverPi = 180.0f / Pi;
 
// Conversions between degrees and radians.
const float _DegToRad = 0.0174532925199433f;  // pi / 180.0
const float _RadToDeg = 57.295779513082321f;  // 180.0 / pi


const float LowEpsilon = 0.001f;
const float HighEpsilon = 0.000001f;
const float ZeroEpsilon = 32.0f * MinPosFloat;  // Very small epsilon for checking against 0.0f

#ifdef __GNUC__
	const float NaN = __builtin_nanf("");
#else
	const float NaN = *(float *)&MaxUInt32;
#endif

template<class T>
const T& min2(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

template<class T>
const T& max2(const T& a, const T& b)
{
    return (a < b) ? b : a;
}

template<typename T>
struct identityT{ using type = T;};

template<typename Real>
void sinCos(Real *returnSin, Real *returnCos, typename identityT<Real>::type theta) {
    // For simplicity, we'll just use the normal trig functions.
    // Note that on some platforms we may be able to do better
    *returnSin = sin(theta);
    *returnCos = cos(theta);
}
// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi
template<typename Real>
Real wrapPi(Real theta) {
    theta += Pi;
    theta -= floor(theta * _1Over2Pi) * TwoPi;
    theta -= Pi;
    return theta;
}
//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
inline float invSqrt(float x) {
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;
  i = 0x5f3759df - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}

// 0 ---- pi
inline float radToDeg( const float fVal ) {
    return fVal * _RadToDeg;
}
// 0 ---- 180
inline float degToRad( const float fVal ) {
    return fVal * _DegToRad;
}
inline bool equal(float x, float y, float evsilon = LowEpsilon) {
	if ( fabs( x - y ) < evsilon ) {
		return true;
	}
	return false;
}
template<typename T>
T clamp( const T& p , const T& min , const T& max ) {
    if( p < min ) return min;
    if( p > max ) return max;
    return p;
}

/**
Smooth Hermite插值
 */
inline float smoothInterpolate(float mini, float maxi, float x) {
	float t = clamp((x - mini) / (maxi - mini), 0.0f, 1.0f);
	return t * t * (3.f - 2.f * t);
}

/**
Smooth Hermite插值
 */
inline float smoothInterpolate(float maxi, float x) {
	x = clamp( x / maxi, 0.f, 1.f );
	return  x * x  * (3.0f - 2.0f * x);
}

/**
  线性插值 factor[0.0 , 1.0]
 */
template<typename T>
T lerp(const T &res,const T &rhs, float factor) {
    return res + (rhs - res) * factor;// a * (1.f - ratio) + b * ratio;
};

// 快速线性插值
template<class T>
T fastLerp(const T& a, const T& b, float ratio, float inv_ratio)
{
    return (a * inv_ratio + b * ratio);
}



// 透视矫正插值
template<class T>
inline T wCorrectLerp(const T& a, const T& b, float ratio, float wa, float wb) {
    return (a * wa * (1.f - ratio) + b * wb * ratio) / ( wb * ratio + wa * (1.f - ratio) );
}

