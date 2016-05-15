#pragma once

#include <math.h>

// Let f(t,A) = sin(t*A)/sin(A).  The slerp of quaternions q0 and q1 is
//   slerp(t,q0,q1) = f(1-t,A)*q0 + f(t,A)*q1.
// Let y = 1-cos(A); we allow A in [0,pi], so y in [0,1].  As a function of y,
// a series representation for f(t,y) is
//   f(t,y) = sum_{i=0}^{infinity} c_{i}(t) y^{i}
// where c_0(t) = t, c_{i}(t) = c_{i-1}(t)*(i^2 - t^2)/(i*(2*i+1)) for i >= 1.
// The c_{i}(t) are polynomials in t of degree 2*i+1.  The paper
//
//   A Fast and Accurate Algorithm for Computing SLERP,
//   David Eberly,
//   Journal of Graphics, GPU, and Game Tools, 15 : 3, 161 - 176, 2011
//   http://www.tandfonline.com/doi/abs/10.1080/2151237X.2011.610255#.VHRB7ouUd8E
//
// derives an approximation
//   g(t,y) = sum_{i=0}^{n-1} c_{i}(t) y^{i} + (1+u_n) c_{n}(t) y^n
// which has degree 2*n+1 in t and degree n in y.  The u_n were chosen to
// balance the error at y = 1 (A = pi/2).  Unfortunately, the analysis for the
// global error bounds (0 <= y <= 1) is flawed; the error bounds printed in
// the paper are too small by about one order of magnitude (factor of 10).
// Instead they are the following, verified by Mathematica using larger
// precision than 'float' or 'double':
//
//   n | error      |  n | error      |  n | error      |  n | error
//   --+------------+----+------------+----+------------+----+-------------
//   1 | 2.60602e-2 |  5 | 3.63188e-4 |  9 | 1.12223e-5 | 13 | 4.37180e-7
//   2 | 7.43321e-3 |  6 | 1.47056e-4 | 10 | 4.91138e-6 | 14 | 1.98230e-7
//   3 | 2.51798e-3 |  7 | 6.11808e-5 | 11 | 2.17345e-6 | 15 | 9.04302e-8
//   4 | 9.30819e-4 |  8 | 2.59880e-5 | 12 | 9.70876e-7 | 16 | 4.03665e-8
//
// The maximum errors all occur for an angle that is nearly pi/2.  The
// approximation is much more accurate when for angles A in [0,pi/4].  With
// this restriction, the global error bounds are
//
//   n | error      |  n | error      |  n | error       |  n | error
//   --+------------+----+------------+----+-------------+----+-------------
//   1 | 1.90648e-2 |  5 | 5.83197e-6 |  9 | 6.80465e-10 | 13 | 3.39728e-13
//   2 | 2.43581e-3 |  6 | 8.00278e-6 | 10 | 9.12477e-11 | 14 | 4.70735e-14
//   3 | 3.20235e-4 |  7 | 1.10649e-7 | 11 | 4.24608e-11 | 15 | 1.55431e-15
//   4 | 4.29242e-5 |  8 | 1.53828e-7 | 12 | 5.93392e-12 | 16 | 1.11022e-16
//
// Given q0 and q1 such that cos(A) = dot(q0,q1) in [0,1], in which case
// A in [0,pi/2], let qh = (q0+q1)/|q0 + q1| = slerp(1/2,q0,q1).  Note that
// |q0 + q1| = 2*cos(A/2) because
//   sin(A/2)/sin(A) = sin(A/2)/(2*sin(A/2)*cos(A/2)) = 1/(2*cos(A/2))
// The angle between q0 and qh is the same as the angle between qh and q1,
// namely, A/2 in [0,pi/4].  It may be shown that
//                    +--
//   slerp(t,q0,q1) = | slerp(2*t,q0,qh),     0 <= t <= 1/2
//                    | slerp(2*t-1,qh,q1), 1/2 <= t <= 1
//                    +--
// The slerp functions on the right-hand side involve angles in [0,pi/4], so
// the approximation is more accurate for those evaluations than using the
// original.

namespace ChebyshevRatio {
    // Compute f(t,A) = sin(t*A)/sin(A), where t in [0,1], A in [0,pi/2],
    // cosA = cos(A), f0 = f(1-t,A), and f1 = f(t,A).
    inline static void get(float& f0, float& f1,float t, float cosA) {
        if (cosA < 1.0f) {
            // The angle A is in (0,pi/2].
            float A = acos(cosA);
            float invSinA = 1.0f / sin(A);
            f0 = sin((1.0f - t) * A) * invSinA;
            f1 = sin(t * A) * invSinA;
        }
        else {
            // The angle theta is 0.
            f0 = 1.0f - t;
            f1 = t;
        }
    }

    // Compute estimates for f(t,y) = sin(t*A)/sin(A), where t in [0,1],
    // A in [0,pi/2], y = 1 - cos(A), f0 is the estimate for f(1-t,y), and
    // f1 is the estimate for f(t,y).  The approximating function is a
    // polynomial of two variables. The template parameter N must be in
    // {1..16}.  The degree in t is 2*N+1 and the degree in Y is N.
    template <int N>
    inline static void getEstimate(float& f0, float& f1,float t, float y){
        static_assert(1 <= N && N <= 16, "Invalid degree.");

        // The ASM output of the MSVS 2013 Release build shows that the constants
        // in these arrays are loaded to XMM registers as literal values, and only
        // those constants required for the specified degree D are loaded.  That
        // is, the compiler does a good job of optimizing the code.

        float const onePlusMu[16] =
            {
                (float)1.62943436108234530,
                (float)1.73965850021313961,
                (float)1.79701067629566813,
                (float)1.83291820510335812,
                (float)1.85772477879039977,
                (float)1.87596835698904785,
                (float)1.88998444919711206,
                (float)1.90110745351730037,
                (float)1.91015881189952352,
                (float)1.91767344933047190,
                (float)1.92401541194159076,
                (float)1.92944142668012797,
                (float)1.93413793373091059,
                (float)1.93824371262559758,
                (float)1.94186426368404708,
                (float)1.94508125972497303
            };

        float const a[16] =
            {
                (N != 1 ? (float)1 : onePlusMu[0]) / ((float)1 * (float)3),
                (N != 2 ? (float)1 : onePlusMu[1]) / ((float)2 * (float)5),
                (N != 3 ? (float)1 : onePlusMu[2]) / ((float)3 * (float)7),
                (N != 4 ? (float)1 : onePlusMu[3]) / ((float)4 * (float)9),
                (N != 5 ? (float)1 : onePlusMu[4]) / ((float)5 * (float)11),
                (N != 6 ? (float)1 : onePlusMu[5]) / ((float)6 * (float)13),
                (N != 7 ? (float)1 : onePlusMu[6]) / ((float)7 * (float)15),
                (N != 8 ? (float)1 : onePlusMu[7]) / ((float)8 * (float)17),
                (N != 9 ? (float)1 : onePlusMu[8]) / ((float)9 * (float)19),
                (N != 10 ? (float)1 : onePlusMu[9]) / ((float)10 * (float)21),
                (N != 11 ? (float)1 : onePlusMu[10]) / ((float)11 * (float)23),
                (N != 12 ? (float)1 : onePlusMu[11]) / ((float)12 * (float)25),
                (N != 13 ? (float)1 : onePlusMu[12]) / ((float)13 * (float)27),
                (N != 14 ? (float)1 : onePlusMu[13]) / ((float)14 * (float)29),
                (N != 15 ? (float)1 : onePlusMu[14]) / ((float)15 * (float)31),
                (N != 16 ? (float)1 : onePlusMu[15]) / ((float)16 * (float)33)
            };

        float const b[16] =
            {
                (N != 1 ? (float)1 : onePlusMu[0]) * (float)1 / (float)3,
                (N != 2 ? (float)1 : onePlusMu[1]) * (float)2 / (float)5,
                (N != 3 ? (float)1 : onePlusMu[2]) * (float)3 / (float)7,
                (N != 4 ? (float)1 : onePlusMu[3]) * (float)4 / (float)9,
                (N != 5 ? (float)1 : onePlusMu[4]) * (float)5 / (float)11,
                (N != 6 ? (float)1 : onePlusMu[5]) * (float)6 / (float)13,
                (N != 7 ? (float)1 : onePlusMu[6]) * (float)7 / (float)15,
                (N != 8 ? (float)1 : onePlusMu[7]) * (float)8 / (float)17,
                (N != 9 ? (float)1 : onePlusMu[8]) * (float)9 / (float)19,
                (N != 10 ? (float)1 : onePlusMu[9]) * (float)10 / (float)21,
                (N != 11 ? (float)1 : onePlusMu[10]) * (float)11 / (float)23,
                (N != 12 ? (float)1 : onePlusMu[11]) * (float)12 / (float)25,
                (N != 13 ? (float)1 : onePlusMu[12]) * (float)13 / (float)27,
                (N != 14 ? (float)1 : onePlusMu[13]) * (float)14 / (float)29,
                (N != 15 ? (float)1 : onePlusMu[14]) * (float)15 / (float)31,
                (N != 16 ? (float)1 : onePlusMu[15]) * (float)16 / (float)33
            };

        float term0 = (float)1 - t, term1 = t;
        float sqr0 = term0 * term0, sqr1 = term1 * term1;
        f0 = term0;
        f1 = term1;
        for (int i = 0; i < N; ++i)
        {
            term0 *= (b[i] - a[i] * sqr0) * y;
            term1 *= (b[i] - a[i] * sqr1) * y;
            f0 += term0;
            f1 += term1;
        }
    }
}

