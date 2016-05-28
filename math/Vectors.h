#pragma once
#include "Common.h"
#include <assert.h>
#include <stdio.h>
//#include "xmmintrin.h"

struct Int2 {
    union{
        int m_[2];
        struct {int x;int y;};
        // __m128 m128;
    };
    Int2(const int* const p):x(p[0]),y(p[1]){}
    Int2(const int x1=0,const int y1=0):x(x1),y(y1){}
};

struct Float2 {
    static constexpr int N = 2;
    static constexpr size_t ELEMSIZE = sizeof(float);
    using type = float;
    union{
        float m[2];
        struct {float x;float y;};
    };
    //Float2():x(0),y(0){}
    Float2() = default;
    Float2(const float x1,const float y1):x(x1),y(y1){}
    Float2 & set ( float val){x = val; y = val;return *this;}
	Float2 & operator += ( const Float2 & p ) {x += p.x; y += p.y; return (*this);}
	Float2 & operator -= ( const Float2 & p ) {x -= p.x; y -= p.y; return (*this);}
	Float2 & operator *= ( const float p ) {x *= p; y *= p; return (*this);}
	Float2 & operator /= ( const float p ) {float inv=1.0f/p;x *= inv; y *= p; return (*this);}

	Float2 operator + ( const Float2 & p ) const {return Float2(x+p.x,y+p.y);}
	Float2 operator - ( const Float2 & p ) const {return Float2(x-p.x,y-p.y);}
	Float2 operator * ( const float p ) const {return Float2(x*p,y*p);}
	Float2 operator / ( const float p ) const {float inv=1.0f/p;return Float2(x*inv,y*inv);}

	bool operator == ( const Float2 & p ) const {
        return equal(p.x,x) && equal(p.y,y);
    }
	bool operator != ( const Float2 & p ) const {
		return !(*this == p);
    }
	float length() const {return sqrtf( x * x + y * y );}
	float lengthSquare() const {return x * x + y * y;}
	Float2 & normalize() {float len=length(); if(len){float inv =1.0f/length();(*this) *= inv;}return (*this);}
	void makeZero() {x = 0; y = 0;}
	Float2& reflect( Float2& i, Float2& n) {*this= i - n * (i.dot(n)*2.f); return *this;}
	float dot( const Float2& rhs ) const {return x * rhs.x + y * rhs.y;}
	static float dot( const Float2& lhs, const Float2& rhs ) {return lhs.x * rhs.x + lhs.y * rhs.y;}
};
struct Float3 {
    static constexpr const int N = 3;
    static constexpr size_t ELEMSIZE = sizeof(float);
    using  type = float;
	union
	{
		float m[3];
		struct { float x , y , z; };
		struct { float r , g , b; };
	};
    //Float3():x(0),y(0),z(0){}
    Float3() = default;
	Float3( const float px , const float py , const float pz ) {x = px; y = py; z = pz;}
	Float3( const Float2& pxy , const float pz ) {x = pxy.x; y = pxy.y; z = pz;}
    Float3 & set (float val){x = val; y = val; z = val; return *this;}
	Float3 & operator += ( const Float3 & p ) {x += p.x; y += p.y; z += p.z; return (*this);}
	Float3 & operator -= ( const Float3 & p ) {x -= p.x; y -= p.y; z -= p.z; return (*this);}
	//Float3 operator *= ( const Float3 & p ) {x *= p.x; y *= p.y; z *= p.z; return (*this);}
	Float3 & operator *= ( const float p ) {x *= p; y *= p; z *= p; return (*this);}
	Float3 & operator /= ( const float p ) {float inv=1.0f/p;x *= inv; y *= inv; z *= inv; return (*this);}
	Float3 operator + ( const Float3 & p ) const {return Float3( x + p.x , y + p.y , z + p.z );}
	Float3 operator - ( const Float3 & p ) const {return Float3 ( x - p.x , y - p.y , z - p.z );}
	Float3 operator - () const {return Float3 ( -x , -y , -z );}
	Float3& cross ( const Float3 & p ) {*this= Float3 ( y * p.z - z * p.y , z * p.x - x * p.z , x * p.y - y * p.x );return (*this);}
	Float3 cross ( const Float3 & p ) const {return Float3 ( y * p.z - z * p.y , z * p.x - x * p.z , x * p.y - y * p.x );}
	Float3 operator * ( const float p ) const {return Float3 ( x * p , y * p , z * p );}
	Float3 operator / ( const float p ) const {float inv=1.0f/p;return Float3 ( x * inv , y * inv , z * inv );}
	bool operator == ( const Float3 & p ) const {
        return equal(x,p.x) && equal(y,p.y) && equal(z,p.z);
    }
	bool operator != ( const Float3 & p ) const {
        return !(*this == p);
    }
    float lengthSquare() const {return x * x + y * y + z * z;}
	float length() const {return sqrtf( lengthSquare() );}
	Float3& normalize() {float len=length(); if(len){float inv =1.0f/length();(*this) *= inv;}return (*this);}
	Float3& reflect( Float3& i, Float3& n) {*this= i - n * (i.dot(n)*2.f); return *this;}
    Float3& makeZero(){x=y=z=0;return *this;}
    Float3& makeUnit(){x=y=0;z=1;return *this;}
	float dot( const Float3& rhs ) const {return x * rhs.x + y * rhs.y + z * rhs.z;}
	static float dot( const Float3& lhs, const Float3& rhs ) {return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;}
};

struct Float4 {
    static constexpr int N = 4;
    static constexpr size_t ELEMSIZE = sizeof(float);
    using type = float;
	union
	{
		float m[4];

		struct { float x , y , z , w; };
		struct { float r , g , b , a; };
	};
	//Float4():x(0),y(0),z(0),w(0) {}
    Float4() = default;
	Float4( const float * const p ) {x = p[0]; y = p[1]; z = p[2]; w = p[3];}
	Float4( const float px , const float py , const float pz , const float pw ) {x = px; y = py; z = pz; w = pw;}
	Float4( const float px ) {x = px;y = px;z = px;w = px;}
	Float4( const Float3& v, const float pw ) {x = v.x;y = v.y; z = v.z; w = pw;}
    Float4 & set(float val){x = y = z = w = val;return *this;}
	Float4 & operator += ( const Float4 & p ) {x += p.x; y += p.y; z += p.z; w += p.w; return (*this);}
	Float4 & operator -= ( const Float4 & p ) {x -= p.x; y -= p.y; z -= p.z; w -= p.w; return (*this);}
	Float4 & operator *= ( const float p ) {x *= p; y *= p; z *= p; w *= p; return (*this);}
	Float4 & operator /= ( const float p ) {float inv=1.0f/p;x *= inv; y *= inv; z *= inv; w *= inv; return (*this);}

	Float4 operator + ( const Float4 & p ) const {return Float4 ( x + p.x , y + p.y , z + p.z , w + p.w );}
	Float4 operator - ( const Float4 & p ) const {return Float4 ( x - p.x , y - p.y , z - p.z , w - p.w );}
	Float4 operator * ( const float p ) const {return Float4 ( x * p , y * p , z * p , w * p );}
	Float4 operator / ( const float p ) const {float inv=1.0f/p;return Float4 ( x * inv , y * inv , z * inv , w * inv );}
    static Float4 identity() {return Float4(0,0,0,1);}

    Float4 cross ( const Float4 & p ) const {return Float4 ( y * p.z - z * p.y , z * p.x - x * p.z , x * p.y - y * p.x,0);}

	bool operator == ( const Float4 & p ) const {
        return equal(x,p.x) && equal(y,p.y) && equal(z,p.z) && equal(w,p.w);
	}
	bool operator != ( const Float4 & p ) const {
		return !(*this == p);
	}

	float length() const {return sqrtf( x * x + y * y + z * z + w * w );}
	float lengthSquare() const {return x * x + y * y + z * z + w * w;}
	Float4& normalize() {float len=length();if(len){float inv =1.0f/length();(*this) *= inv;} return (*this);}
    Float4& makeZero() {x=y=z=w=0;return *this;}
    Float4& makeUnit() {x=y=z=0;w=1;return *this;}
	void sqrt() {x = sqrtf(x); y = sqrtf(y); z = sqrtf(z); w = sqrtf(w);}
	float dot( const Float4& rhs ) const {return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;}
    void print(void) const{printf("x:%f,y:%f,z:%f,w:%f\r\n",x,y,z,w);}
};


inline Float4 clamp(  const Float4& p , float min, float max ) {
	return Float4(clamp(p.x, min, max),clamp(p.y, min, max),clamp(p.z, min, max),clamp(p.w, min, max));
}

inline Float4 pow( const Float4& f, float pow) {
    return Float4(powf( f.x, pow ), powf( f.y, pow ), powf( f.z, pow ), powf( f.w, pow ));
}

inline Float3 pow( const Float3& f, float pow) {
    return Float3(powf( f.x, pow ), powf( f.y, pow ), powf( f.z, pow ));
}


// Gram-Schmidt orthonormalization to generate orthonormal vectors from the
// linearly independent inputs.  The function returns the smallest length of
// the unnormalized vectors computed during the process.  If this value is
// nearly zero, it is possible that the inputs are linearly dependent
template<typename V>
float gramSchmidtOrthlize(int Num,V* v){
    assert(Num >= 2);
    if (v && 1 <= Num && Num <= V::N)
    {
        v[0].normalize();
        float minLength = v[0].length();
        for (int i = 1; i < Num; ++i)
        {
            for (int j = 0; j < i; ++j)
            {
                auto dot = v[i].dot(v[j]);
                v[i] -= v[j] * dot;
            }
            v[i].normalize();
            auto length = v[i].length();
            if (length < minLength)
            {
                minLength = length;
            }
        }
        return minLength;
    }

    return 0;
}
