#include <Matrix.h>
#include <gtest/gtest.h>
#include <type_traits>

#include <iostream>
using namespace std;
TEST(Matrix,test2x2){
    using M22 = MatrixNN<Float2>;

    cout<<"ispod:"<<is_pod<M22>::value<<endl;
    M22 a1{0,0};
    EXPECT_TRUE( (a1.cols[0] == M22().cols[0]) );
    EXPECT_TRUE( (a1.cols[1] == M22().cols[1]) );

    M22 a2{1,2,3,4};
    EXPECT_FLOAT_EQ(a2[0],1.f);
    EXPECT_FLOAT_EQ(a2[1],3.f);
    EXPECT_FLOAT_EQ(a2[2],2.f);
    EXPECT_FLOAT_EQ(a2[3],4.f);

    using RowView = M22::RowView;
    RowView rv(a2.rowView(0));
    EXPECT_TRUE((Float2{rv[0],rv[1]} == Float2{1,2}));
    rv = a2.rowView(1);
    EXPECT_TRUE((Float2{rv[0],rv[1]} == Float2{3,4}));

    using ColView = M22::ColView;
    ColView cv(a2.colView(0));
    EXPECT_TRUE((Float2{cv[0],cv[1]} == Float2{1,3}));
    cv = a2.colView(1);
    EXPECT_TRUE((Float2{cv[0],cv[1]} == Float2{2,4}));
    EXPECT_TRUE((a2.col(0) == Float2{1,3}));
    EXPECT_TRUE((a2.col(1) == Float2{2,4}));

    M22 a3 = a2.transpose();
    EXPECT_TRUE((a3 == M22{1,3,2,4}));

    a3.makeIdentity();
    EXPECT_TRUE((a3 == M22{1,0,0,1}));

    a3.makeDiagonal(Float2{5,6});
    EXPECT_TRUE((a3 == M22{5,0,0,6}));

    a3 += M22{-5,1,2,-6};
    EXPECT_TRUE((a3 == M22{0,1,2,0}));

    a3 /= 2;
    EXPECT_TRUE((a3 == M22{0,0.5f,1.f,0}));

    M22 a4 = a3 + M22{1,0.5f,2,2};
    EXPECT_EQ(a4,(M22{1,1,3,2}));

    EXPECT_FALSE(a4 == a3);

    Float2 f2{2,3};
    a4.makeIdentity();
    EXPECT_EQ(f2,a4*f2);


    a4.set({1,2,3,4});
    EXPECT_EQ(multiplyMD(a4,Float2{7,8}),(M22{7,16,21,32}));
    EXPECT_EQ(multiplyDM(Float2{7,8},a4),(M22{7,14,24,32}));
    EXPECT_EQ(multiplyMD(a4,Float2{1,1}),a4);

    a4.set({-3,5,0,0.5f});
    EXPECT_EQ((a4*Float2{-7,4}),(Float2{41,2}));
    EXPECT_EQ((a4*M22{-7,4,2,6}),(M22{31,18,1,3}));

    M22 a5{1,2,3,4};
    EXPECT_FLOAT_EQ(determinant(a5),(1*4-2*3));
    inverse(a5).print();
    M22{-2,1,1.5f,-0.5f}.print();
    EXPECT_EQ(inverse(a5),(M22{-2,1,1.5f,-0.5f}));
}

TEST(Matrix,test3x3){
    using M33 = MatrixNN<Float3>;
    cout<<"ispod:"<<is_pod<M33>::value<<endl;

    M33 a2{1,2,3,4,5,6,7,8,9};
    using RowView = M33::RowView;
    RowView rv(a2.rowView(0));
    EXPECT_TRUE((Float3{rv[0],rv[1],rv[2]} == Float3{1,2,3}));
    rv = a2.rowView(1);
    EXPECT_TRUE((Float3{rv[0],rv[1],rv[2]} == Float3{4,5,6}));
    rv = a2.rowView(2);
    EXPECT_TRUE((Float3{rv[0],rv[1],rv[2]} == Float3{7,8,9}));

    using ColView = M33::ColView;
    ColView cv(a2.colView(0));
    EXPECT_TRUE((Float3{cv[0],cv[1],cv[2]} == Float3{1,4,7}));
    cv = a2.colView(1);
    EXPECT_TRUE((Float3{cv[0],cv[1],cv[2]} == Float3{2,5,8}));
    cv = a2.colView(2);
    EXPECT_TRUE((Float3{cv[0],cv[1],cv[2]} == Float3{3,6,9}));

    EXPECT_TRUE((a2.col(0) == Float3{1,4,7}));
    EXPECT_TRUE((a2.col(1) == Float3{2,5,8}));
    EXPECT_TRUE((a2.col(2) == Float3{3,6,9}));

    M33 a3 = a2.transpose();
    EXPECT_TRUE((a3 == M33{1,4,7,2,5,8,3,6,9}));

    a3.makeIdentity();
    EXPECT_TRUE((a3 == M33{1,0,0,0,1,0,0,0,1}));

    a3.makeDiagonal(Float3{11,12,13});
    EXPECT_TRUE((a3 == M33{11,0,0,0,12,0,0,0,13}));

    a3 += M33{-5,1,2,-6,3,5,1,1,1};
    EXPECT_TRUE((a3 == M33{6,1,2,-6,15,5,1,1,14}));

    a3 /= 2;
    EXPECT_TRUE((a3 == M33{3,0.5,1,-3,7.5,2.5,0.5,0.5,7}));

    M33 a4 = a3 + M33{1,1,1,1,1,1,1,1,1};
    EXPECT_EQ(a4,(M33{4,1.5,2,-2,8.5,3.5,1.5,1.5,8}));

    EXPECT_FALSE(a4 == a3);

    Float3 f2{2,3,4};
    a4.makeIdentity();
    EXPECT_EQ(f2,a4*f2);

    EXPECT_EQ((M33{1,2}),(M33{1,2,0,0,0,0,0,0,0}));

    a4.set({1,2,3,4,5,6,7,8,9});
    EXPECT_EQ(multiplyMD(a4,Float3{7,8,9}),(M33{7,16,27,28,40,54,49,64,81}));

    EXPECT_EQ(multiplyDM(Float3{7,8,9},a4),(M33{7,14,21,32,40,48,63,72,81}));
    EXPECT_EQ(multiplyMD(a4,Float3{1,1,1}),a4);

    EXPECT_EQ((a4*Float3{-7,4,8}),(Float3{25,40,55}));
    EXPECT_EQ((a4*M33{-7,4,2,6,1,3,5,8,3}),(M33{20,30,17,32,69,41,44,108,65}));

    a4.set({1,0,7,-5,-2,2,3,6,-4});
    EXPECT_FLOAT_EQ(determinant(a4),-172);
    EXPECT_EQ(inverse(a4),(M33{0.0233,-0.2442,-0.0814,
                    0.0814,0.1453,0.2151,
                    0.1395,0.0349,0.0116}));
}
TEST(Matrix,test4x4){
    using M44 = MatrixNN<Float4>;
    cout<<"ispod:"<<is_pod<M44>::value<<endl;

    M44 a2{1,2,3,4,
            5,6,7,8,
            9,10,11,12,
            13,14,15,16};

    using RowView = M44::RowView;
    RowView rv(a2.rowView(0));
    EXPECT_TRUE((Float4{rv[0],rv[1],rv[2],rv[3]} == Float4{1,2,3,4}));
    rv = a2.rowView(1);
    EXPECT_TRUE((Float4{rv[0],rv[1],rv[2],rv[3]} == Float4{5,6,7,8}));
    rv = a2.rowView(2);
    EXPECT_TRUE((Float4{rv[0],rv[1],rv[2],rv[3]} == Float4{9,10,11,12}));
    rv = a2.rowView(3);
    EXPECT_TRUE((Float4{rv[0],rv[1],rv[2],rv[3]} == Float4{13,14,15,16}));


    using ColView = M44::ColView;
    ColView cv(a2.colView(0));
    EXPECT_TRUE((Float4{cv[0],cv[1],cv[2],cv[3]} == Float4{1,5,9,13}));
    cv = a2.colView(1);
    EXPECT_TRUE((Float4{cv[0],cv[1],cv[2],cv[3]} == Float4{2,6,10,14}));
    cv = a2.colView(2);
    EXPECT_TRUE((Float4{cv[0],cv[1],cv[2],cv[3]} == Float4{3,7,11,15}));
    cv = a2.colView(3);
    EXPECT_TRUE((Float4{cv[0],cv[1],cv[2],cv[3]} == Float4{4,8,12,16}));


    EXPECT_TRUE((a2.col(0) == Float4{1,5,9,13}));
    EXPECT_TRUE((a2.col(1) == Float4{2,6,10,14}));
    EXPECT_TRUE((a2.col(2) == Float4{3,7,11,15}));
    EXPECT_TRUE((a2.col(3) == Float4{4,8,12,16}));



    M44 a3 = a2.transpose();
    EXPECT_TRUE((a3 == M44{1,5,9,13,
                    2,6,10,14,
                    3,7,11,15,
                    4,8,12,16}));

    a3.makeIdentity();
    EXPECT_TRUE((a3 == M44{1,0,0,0,
                           0,1,0,0,
                           0,0,1,0,
                           0,0,0,1}));

    a3.makeDiagonal(Float4{11,12,13,14});
    EXPECT_TRUE((a3 == M44{11,0,0,0,
                    0,12,0,0,
                    0,0,13,0,
                    0,0,0,14}));
    a3 += M44{1,1,1,1,
            1,1,1,1,
            1,1,1,1,
            1,1,1,1};
    EXPECT_TRUE((a3 == M44{12,1,1,1,
                    1,13,1,1,
                    1,1,14,1,
                    1,1,1,15}));

    a3 /= 2;
    EXPECT_TRUE((a3 == M44{6,0.5,0.5,0.5,
                    0.5,6.5,0.5,0.5,
                    0.5,0.5,7,0.5,
                    0.5,0.5,0.5,7.5}));

    M44 a4 = a3 + M44{1,1,1,1,
                      1,1,1,1,
                      1,1,1,1,
                      1,1,1,1};
    EXPECT_EQ(a4,(M44{7,1.5,1.5,1.5,
                    1.5,7.5,1.5,1.5,
                    1.5,1.5,8,1.5,
                    1.5,1.5,1.5,8.5}));

    EXPECT_FALSE(a4 == a3);

    Float4 f2{2,3,4,5};
    a4.makeIdentity();
    EXPECT_EQ(f2,a4*f2);

    EXPECT_EQ((M44{1,2}),(M44{1,2,0,0,0,0,0,0,0}));

    Matrix44 p1{
         1,-5, 3, 0,
         0,-2, 6, 0,
         7, 2,-4, 0,
         0, 0, 0, 1};
    Matrix44 p2{
        -8, 6, 1, 0,
         7, 0,-3, 0,
         2, 4, 5, 0,
         0, 0, 0, 1};
         

    EXPECT_EQ(multiplyMD(p1,Float4{6,7,8,9}),(M44{6,-35,24,0,
                    0,-14,48,0,
                    42,14,-32,0,
                    0,0,0,9
                    }));

    EXPECT_EQ(multiplyDM(Float4{6,7,8,9},p1),(M44{6,-30,18,0,
                    0,-14,42,0,
                    56,16,-32,0,
                    0,0,0,9}));

    EXPECT_EQ(multiplyMD(a4,Float4{1,1,1,1}),a4);

    EXPECT_EQ((p1*Float4{-7,4,8,8}),(Float4{-3,40,-73,8}));
    EXPECT_EQ((p1*p2),(M44{-37.0000,18.0000,31.0000,0.0000,
                    -2.0000,24.0000,36.0000,0.0000,
                    -50.0000,26.0000,-19.0000,0.0000,
                    0.0000,0.0000,0.0000,1.0000
                    }));

    //EXPECT_FLOAT_EQ(determinant(a4),-172);
    EXPECT_EQ(inverse(p1),(M44{
                     0.0233 ,   0.0814 ,   0.1395 ,   0.0000,
                    -0.2442 ,   0.1453 ,   0.0349 ,   0.0000,
                    -0.0814 ,   0.2151 ,   0.0116 ,   0.0000,
                     0.0000 ,   0.0000 ,   0.0000 ,   1.0000
            }));

}













