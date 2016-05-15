#pragma once

#include <initializer_list>
#include "Vectors.h"
#include <string.h> // memcpy
#include <assert.h> // assert
#include <vector>
#include <stdio.h> //printf

template<class ColType>
struct MatrixNN{
private:
    struct initstr{};
    MatrixNN(initstr){memset(&cols,0,sizeof(cols));}
public:
    using type = typename ColType::type;
    static constexpr int N = ColType::N;
    ColType cols[N];
    MatrixNN() = default;
    MatrixNN(std::initializer_list<ColType> args):MatrixNN(initstr()){
        assert(args.size()<=N);
        memcpy(cols,args.begin(),args.size()*N*ColType::ELEMSIZE);
    }
    MatrixNN(std::initializer_list<type> args):MatrixNN(initstr()){set(args);}
    void set(std::initializer_list<type> args){
        assert(args.size()<=N*N);
        memcpy(cols,args.begin(),args.size()*ColType::ELEMSIZE);
        makeTranspose();
    }

    inline const ColType& axis(int i) const { assert(i>=0&&i<N); return cols[i];}
    inline ColType& axis(int i) { assert(i>=0&&i<N); return cols[i];}

    inline type  operator()(int r, int c) const{assert(r<N&&c<N); return cols[c].m[r];}
    inline type& operator()(int r, int c){ assert(r<N&&c<N); return cols[c].m[r];}

    inline type  operator[](int p) const { assert(p<N*N); return cols[0].m[p]; }
    inline type& operator[](int p)       { assert(p<N*N); return cols[0].m[p]; }

    struct RowView{
        type*  m_;
        RowView(MatrixNN& matrix,int r):m_(&(matrix.cols[0].m[r])){assert(r<N);}
        type& operator[](int indexOnRow){assert(indexOnRow<N); return m_[ N * indexOnRow];}
    };
    struct ColView{
        type* m_;
        ColView(MatrixNN& matrix,int c):m_(&(matrix.cols[c].m[0])){assert(c<N);}
        type& operator[](int indexOnCol){assert(indexOnCol<N); return m_[ indexOnCol];}
    };

    ColView   colView(int c) {assert(c<N); return ColView(*this,c);}
    RowView   rowView(int r) {assert(r<N); return RowView(*this,r);}

    ColType   col(int c) const {assert(c<N); return cols[c];}
    ColType&  col(int c) {assert(c<N); return cols[c];}

    MatrixNN transpose() const {
        MatrixNN result(*this);
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                 if(c!=r)
                 result.cols[c].m[r] = cols[r].m[c];
            }
        }
		return result;
	}
    MatrixNN& makeTranspose(){
        *this = transpose();
        return (*this);
    }

	MatrixNN& makeIdentity() {
        ColType t;
        t.set((type) 1);
        return makeDiagonal(t);
	}

    MatrixNN& makeDiagonal(ColType const& d){
        for(int i =0;i<N;++i) {
            for(int j=0;j<N;++j){
                if(i == j) cols[i].m[i]=d.m[i];
                else       cols[i].m[j]=0;
            }
        }
        return *this;
    }

    void print() const {
        for(int i=0;i<N;++i){
            for(int j=0;j<N;++j){
                printf("%f,",(*this)(i,j));
            }
            printf("\n");
        }
        printf("\n");
    }
    void printTranspose() const {
        for(int i=0;i<N;++i){
            for(int j=0;j<N;++j){
                printf("%f,",cols[i].m[j]);
            }
            printf("\n");
        }
        printf("\n");
    }
};

using DiagonalM22 = Float2;
using DiagonalM33 = Float3;
inline DiagonalM22 inverse(const DiagonalM22& m){
    assert(m.x != 0 && m.y != 0);
    return Float2{1.0f/m.x,1.0f/m.y};
}

inline DiagonalM33 inverse(const DiagonalM33& m){
    assert(m.x!=0 && m.y!=0 && m.z!=0);
    return Float3{1.0f/m.x,1.0f/m.y,1.0f/m.z};
}

using Matrix33 = MatrixNN<Float3>;
using Matrix44 = MatrixNN<Float4>;

template <class ColType>
MatrixNN<ColType>& operator+=(MatrixNN<ColType>& M0, MatrixNN<ColType> const& M1) {
    for (int i = 0; i < ColType::N ; ++i) {
        M0.cols[i] += M1.cols[i];
    }
    return M0;
}

template <class ColType>
MatrixNN<ColType>& operator-=(MatrixNN<ColType>& M0, MatrixNN<ColType> const& M1) {
    for (int i = 0; i < ColType::N ; ++i) {
        M0.cols[i] -= M1.cols[i];
    }
    return M0;
}

template <class ColType,typename Real>
MatrixNN<ColType>& operator*=(MatrixNN<ColType>& M0,Real scalar){
    for(int i=0;i<ColType::N;++i){
        M0.cols[i] *= scalar;
    }
    return M0;
}

template <class ColType,typename Real>
MatrixNN<ColType>& operator/=(MatrixNN<ColType>& M,Real scalar) {
    using type = typename ColType::type;
    if (scalar != 0) {
        type invScalar = ((type)1) / scalar;
        for (int i = 0; i < ColType::N ; ++i) {
            M.cols[i] *= invScalar;
        }
    }
    else {
        ColType t;memset(&t,0,sizeof(ColType));
        for (int i = 0; i < ColType::N ; ++i) {
            M.cols[i] = t;
        }
    }
    return M;
}

template <class ColType>
MatrixNN<ColType> operator+(MatrixNN<ColType> const& M0, MatrixNN<ColType> const& M1) {
    MatrixNN<ColType> result = M0;
    return result += M1;
}

template <class ColType>
MatrixNN<ColType> operator-(MatrixNN<ColType> const& M0, MatrixNN<ColType> const& M1) {
    MatrixNN<ColType> result = M0;
    return result -= M1;
}

template<class ColType>
bool operator==(MatrixNN<ColType> const& M0,MatrixNN<ColType> const& M1){
    for(int i=0;i<MatrixNN<ColType>::N;++i){
        if ( M0.col(i) != M1.col(i) )
            return false;
    }
    return true;
}

template<class ColType>
bool operator!=(MatrixNN<ColType> const& M0,MatrixNN<ColType> const& M1){
    return ! (M0 == M1);
}


template <class ColType,typename Real>
MatrixNN<ColType> operator*(MatrixNN<ColType> const& M,Real scalar) {
    MatrixNN<ColType> result = M;
    return result *= scalar;
}

template <class ColType,typename Real>
MatrixNN<ColType> operator*(Real scalar, MatrixNN<ColType> const& M) {
    MatrixNN<ColType> result = M;
    return result *= scalar;
}

template <class ColType, typename Real>
MatrixNN<ColType> operator/(MatrixNN<ColType> const& M, Real scalar) {
    MatrixNN<ColType> result = M;
    return result /= scalar;
}


template <class Vector>
Vector operator*(MatrixNN<Vector> const& M, Vector const& V) {
    Vector result;
    result.set(0);
    for (int r = 0; r < Vector::N ; ++r) {
        for (int c = 0; c < Vector::N; ++c) {
            result.m[r] += M.cols[c].m[r] * V.m[c];
        }
    }
    return result;
}

template <class DIAGONAL>
MatrixNN<DIAGONAL> multiplyMD(MatrixNN<DIAGONAL> const& M, DIAGONAL const& D) {
    MatrixNN<DIAGONAL> result;
    for (int r = 0; r < DIAGONAL::N; ++r) {
        for (int c = 0; c < DIAGONAL::N; ++c) {
            result(r, c) = M(r, c) * D.m[c];
        }
    }
    return result;
}


template <class DIAGONAL>
MatrixNN<DIAGONAL> multiplyDM(DIAGONAL const& D, MatrixNN<DIAGONAL> const& M) {
    MatrixNN<DIAGONAL> result;
    for (int r = 0; r < DIAGONAL::N; ++r) {
        for (int c = 0; c < DIAGONAL::N; ++c) {
            result(r, c) = D.m[r] * M(r, c);
        }
    }
    return result;
}

template <class ColType>
MatrixNN<ColType> operator*(MatrixNN<ColType> const& A, MatrixNN<ColType> const& B) {
    MatrixNN<ColType> result;
    for (int c = 0; c < ColType::N ; ++c) {
        result.cols[c] = A * B.cols[c];
    }
    return result;
}
using Matrix22 = MatrixNN<Float2>;

inline float determinant(Matrix22 const& M) {
    return M.cols[0].x*M.cols[1].y - M.cols[0].y*M.cols[1].x;
}

inline Matrix22 inverse(Matrix22 const& m22){
    float det = determinant(m22);
    if (det != 0) {
        float invDet = 1.0f / det;
        return Matrix22 { m22.cols[1].y*invDet ,-m22.cols[1].x*invDet,
                         -m22.cols[0].y*invDet , m22.cols[0].x*invDet};
    }
    return Matrix22();
}

inline float determinant(Matrix33 const& M){
    float c00 = M(1, 1)*M(2, 2) - M(1, 2)*M(2, 1);
    float c10 = M(1, 2)*M(2, 0) - M(1, 0)*M(2, 2);
    float c20 = M(1, 0)*M(2, 1) - M(1, 1)*M(2, 0);
    return M(0, 0)*c00 + M(0, 1)*c10 + M(0, 2)*c20;
}

inline Matrix33 inverse(Matrix33 const& M){
    float c00 = M(1, 1)*M(2, 2) - M(1, 2)*M(2, 1);
    float c10 = M(1, 2)*M(2, 0) - M(1, 0)*M(2, 2);
    float c20 = M(1, 0)*M(2, 1) - M(1, 1)*M(2, 0);
    float det = M(0, 0)*c00 + M(0, 1)*c10 + M(0, 2)*c20;

    if (det != 0) {
        float invDet = 1.0f/ det;
        return Matrix33 {c00*invDet,
                         (M(0, 2)*M(2, 1) - M(0, 1)*M(2, 2))*invDet,
                         (M(0, 1)*M(1, 2) - M(0, 2)*M(1, 1))*invDet,
                         c10*invDet,
                         (M(0, 0)*M(2, 2) - M(0, 2)*M(2, 0))*invDet,
                         (M(0, 2)*M(1, 0) - M(0, 0)*M(1, 2))*invDet,
                         c20*invDet,
                         (M(0, 1)*M(2, 0) - M(0, 0)*M(2, 1))*invDet,
                         (M(0, 0)*M(1, 1) - M(0, 1)*M(1, 0))*invDet
                    };
    }
    return Matrix33();
}
inline float determinant(Matrix44 const& M){
    float a0 = M(0, 0)*M(1, 1) - M(0, 1)*M(1, 0);
    float a1 = M(0, 0)*M(1, 2) - M(0, 2)*M(1, 0);
    float a2 = M(0, 0)*M(1, 3) - M(0, 3)*M(1, 0);
    float a3 = M(0, 1)*M(1, 2) - M(0, 2)*M(1, 1);
    float a4 = M(0, 1)*M(1, 3) - M(0, 3)*M(1, 1);
    float a5 = M(0, 2)*M(1, 3) - M(0, 3)*M(1, 2);
    float b0 = M(2, 0)*M(3, 1) - M(2, 1)*M(3, 0);
    float b1 = M(2, 0)*M(3, 2) - M(2, 2)*M(3, 0);
    float b2 = M(2, 0)*M(3, 3) - M(2, 3)*M(3, 0);
    float b3 = M(2, 1)*M(3, 2) - M(2, 2)*M(3, 1);
    float b4 = M(2, 1)*M(3, 3) - M(2, 3)*M(3, 1);
    float b5 = M(2, 2)*M(3, 3) - M(2, 3)*M(3, 2);
    return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
}

inline Matrix44 inverse(Matrix44 const& M){
    float a0 = M(0, 0)*M(1, 1) - M(0, 1)*M(1, 0);
    float a1 = M(0, 0)*M(1, 2) - M(0, 2)*M(1, 0);
    float a2 = M(0, 0)*M(1, 3) - M(0, 3)*M(1, 0);
    float a3 = M(0, 1)*M(1, 2) - M(0, 2)*M(1, 1);
    float a4 = M(0, 1)*M(1, 3) - M(0, 3)*M(1, 1);
    float a5 = M(0, 2)*M(1, 3) - M(0, 3)*M(1, 2);
    float b0 = M(2, 0)*M(3, 1) - M(2, 1)*M(3, 0);
    float b1 = M(2, 0)*M(3, 2) - M(2, 2)*M(3, 0);
	float b2 = M(2, 0)*M(3, 3) - M(2, 3)*M(3, 0);
    float b3 = M(2, 1)*M(3, 2) - M(2, 2)*M(3, 1);
    float b4 = M(2, 1)*M(3, 3) - M(2, 3)*M(3, 1);
    float b5 = M(2, 2)*M(3, 3) - M(2, 3)*M(3, 2);
    float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
    if (det != 0) {
        float invDet = 1.0f / det;
        return Matrix44 {
                        (+M(1, 1)*b5 - M(1, 2)*b4 + M(1, 3)*b3)*invDet,
                        (-M(0, 1)*b5 + M(0, 2)*b4 - M(0, 3)*b3)*invDet,
                        (+M(3, 1)*a5 - M(3, 2)*a4 + M(3, 3)*a3)*invDet,
                        (-M(2, 1)*a5 + M(2, 2)*a4 - M(2, 3)*a3)*invDet,
                        (-M(1, 0)*b5 + M(1, 2)*b2 - M(1, 3)*b1)*invDet,
                        (+M(0, 0)*b5 - M(0, 2)*b2 + M(0, 3)*b1)*invDet,
                        (-M(3, 0)*a5 + M(3, 2)*a2 - M(3, 3)*a1)*invDet,
                        (+M(2, 0)*a5 - M(2, 2)*a2 + M(2, 3)*a1)*invDet,
                        (+M(1, 0)*b4 - M(1, 1)*b2 + M(1, 3)*b0)*invDet,
                        (-M(0, 0)*b4 + M(0, 1)*b2 - M(0, 3)*b0)*invDet,
                        (+M(3, 0)*a4 - M(3, 1)*a2 + M(3, 3)*a0)*invDet,
                        (-M(2, 0)*a4 + M(2, 1)*a2 - M(2, 3)*a0)*invDet,
                        (-M(1, 0)*b3 + M(1, 1)*b1 - M(1, 2)*b0)*invDet,
                        (+M(0, 0)*b3 - M(0, 1)*b1 + M(0, 2)*b0)*invDet,
                        (-M(3, 0)*a3 + M(3, 1)*a1 - M(3, 2)*a0)*invDet,
                        (+M(2, 0)*a3 - M(2, 1)*a1 + M(2, 2)*a0)*invDet
                    };
    }
    return Matrix44();
}

