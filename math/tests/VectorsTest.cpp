#include <Vectors.h>
#include <stdlib.h>
#include <gtest/gtest.h>

using namespace std;

TEST(VectorsTest,Vectest){
    Float2 a1;
    EXPECT_FLOAT_EQ(a1.x,0.0f);
    EXPECT_FLOAT_EQ(a1.y,0.0f);
    Float2 a2{1,1};
    a2 *=8;
    EXPECT_EQ(a2,(Float2{8,8}));

    Float2 a3{1,2};
    Float2 a4{1.0f+LowEpsilon,2.0f+LowEpsilon};
    EXPECT_NE(a3,a4);

    Float2 a5{0,4};
    EXPECT_FLOAT_EQ(a5.length(),4.0f);
    a5.normalize();
    EXPECT_FLOAT_EQ(a5.length() , 1.0f);

    Float3 a6{0,0,4};
    EXPECT_FLOAT_EQ(a6.length(), 4.0f);

    Float3 a7{1,3,4};
    Float3 a8 = a7.cross(Float3{2,-5,8});
    cout<<"cross result: "<<a8.x<<" "<<a8.y<<" "<<a8.z<<endl;
    EXPECT_TRUE((a8 == Float3{44,0,-11}));
}
