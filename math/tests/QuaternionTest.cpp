#include "Quaternion.h"
#include "Rotation.h"
#include "Plane.h"
#include "BoundingSphere.h"
#include "Culler.h"
#include <gtest/gtest.h>

#include <iostream>
using namespace std;


TEST(Quaternion,Base){
Plane3 p;
    Quaternion a{-6.0f, -9.0f, 15.0f,0.5f};
    Quaternion b{2.0f, 3.0f, -5.0f,2.0f};
    Quaternion c{-11.0f, -16.5f, 27.5f, 115.0f};
    EXPECT_EQ( a*b , c);

    a =Quaternion { 1.0f, 3.0f, -2.0f, -4.0f};
    b =Quaternion {-0.5f, 1.5f,  3.0f, 12.0f};
    EXPECT_EQ(a.dot(b) , -50.0f);

    EXPECT_EQ((Quaternion{1.0f, 3.0f, -2.0f, -4.0f}.length()),sqrt(30.0f));

    a = Quaternion{1.0f, 3.0f, -2.0f, -4.0f};
    Quaternion inverted = inverse(a);
    EXPECT_EQ(inverted,(Quaternion{-1.0f, -3.0f, 2.0f, -4.0f}/30.0f));
    EXPECT_EQ(a*inverted,makeIdentityQuat());

    auto i = Quaternion{1.0f, 3.0f, -2.0f, -4.0f}.normalize().length();
    EXPECT_EQ(i,1);
    /*
      AxisAngle3 a3({1,2,3},50);
    AxisAngle3 b3(Float3{1,2,4},80);
    EulerAnglesF af(1,1.57f,2);
    af.canonize();
    Matrix33 a33;
    Rotation::convert(a,a33);
    Rotation::convert(a33,a);
    Rotation::convert(a33,a3);
    Rotation::convert(a3,a33);
    Rotation::convert(a,a3);
    Rotation::convert(a3,a);
    Rotation::convert(af,a33);
    a33.printTranspose();
    */
}

TEST(Quaternion,Rotation){
    auto t = 1.0f / sqrt(3.0f);
    Float3 aix{t,t,t};
    AxisAngle3 axi(aix,degToRad(120));
    Quaternion a1;
    axi.axis_.normalize();
    Rotation::convert(axi,a1);
    EXPECT_FLOAT_EQ(angleQuat(a1),degToRad(120.0f));
    EXPECT_FLOAT_EQ(a1.length(),1.0f);
    Quaternion b = createQuat(Float3{0.5,0.5,0.5},0.5);
    //b.normalize();
    EXPECT_TRUE( equalQuat(a1,b));
    //EXPECT_EQ(createQuat(Float3{0.5,0.5,0.5},0.5).normalize().dot(a1), 1.0f);
    EXPECT_EQ(axisQuat(a1),aix);

    axi = AxisAngle3(aix,degToRad(-120.f));
    b = createQuat(Float3{-0.5,-0.5,-0.5},0.5);
    EXPECT_FLOAT_EQ(angleQuat(b),degToRad(120.0f));
    EXPECT_EQ(axisQuat(b), -axi.axis_);

}

TEST(Quaternion,Angle){
    float a1 = angle(Quaternion(1.0f, 2.0f, -3.0f, -4.0f).normalize(),
                     Quaternion(4.0f, -3.0f, 2.0f, -1.0f).normalize());
    float a2 = angle(Quaternion(1.0f, 2.0f, -3.0f, -4.0f).normalize(),
                     Quaternion(4.0f, -3.0f, 2.0f, -1.0f).normalize());
    EXPECT_FLOAT_EQ(a2,a1);
    EXPECT_FLOAT_EQ(a1,1.704528f);
}

TEST(Quaternion,matrix){
    AxisAngle3 ax({-3.0f, 1.0f, 5.0f},degToRad(37.0f));
    ax.axis_.normalize();
    Quaternion q1 ; Rotation::convert(ax,q1);
    Matrix33   m  ; Rotation::convert(ax,m);

    Matrix33   me ; Rotation::convert(q1,me);
    EXPECT_EQ(me ,m);
    //Verify that negated quaternion gives the same rotation
    Rotation::convert(q1*(-1),me);
    EXPECT_EQ(me,m);

}

TEST(Quaternion,lerp){
    auto t = 1.0f / sqrt(3.0f);
    Float3 aix{t,t,t};
    aix.normalize();
    AxisAngle3 a1(aix,degToRad(15.0f));
    AxisAngle3 a2({1,0,0},degToRad(23.0f));
    Quaternion q1;Rotation::convert(a1,q1);
    Quaternion q2;Rotation::convert(a2,q2);
    Quaternion r1 = lerp(q1, q2, 0.35f);
    cout<<"q1:"<<q1.x<<","<<q1.y<<","<<q1.z<<","<<q1.w<<endl;
    cout<<"q2:"<<q2.x<<","<<q2.y<<","<<q2.z<<","<<q2.w<<endl;
    cout<<"==="<<r1.x<<" "<<r1.y<<" "<<r1.z<<" "<<r1.w<<endl;
    EXPECT_EQ(r1,(Quaternion(0.119127f, 0.049134f, 0.049134f, 0.990445f)));
}

TEST(Quaternion,slerp){
    auto t = 1.0f / sqrt(3.0f);
    Float3 aix{t,t,t};
    aix.normalize();
    AxisAngle3 a1(aix,degToRad(15.0f));
    AxisAngle3 a2({1,0,0},degToRad(23.0f));
    Quaternion q1;Rotation::convert(a1,q1);
    Quaternion q2;Rotation::convert(a2,q2);
    Quaternion r1 = slerp(q1, q2, 0.35f);
    cout<<"q1:"<<q1.x<<","<<q1.y<<","<<q1.z<<","<<q1.w<<endl;
    cout<<"q2:"<<q2.x<<","<<q2.y<<","<<q2.z<<","<<q2.w<<endl;
    cout<<"==="<<r1.x<<" "<<r1.y<<" "<<r1.z<<" "<<r1.w<<endl;
    EXPECT_EQ(r1,Quaternion(0.1191653f, 0.0491109f, 0.0491109f, 0.9904423f));

}

TEST(Quaternion,transformVector){
    AxisAngle3 a2({1,0,0},degToRad(23.0f));
    Quaternion q;Rotation::convert(a2,q);
    Matrix44   m;Rotation::convert(a2,m);
    Float3     v(5.0f, -3.6f, 0.7f);
    Float3    rotated = rotate(q,v);
    Float4    rbym    = m*Float4(v.x,v.y,v.z,0);

    EXPECT_EQ(rotated, Float3(rbym.x,rbym.y,rbym.z));

}

