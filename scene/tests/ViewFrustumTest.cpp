#include "ViewFrustum.h"
#include "Rotation.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace std;



TEST(ViewFrustum,Perspective){
    ViewFrustum fu;
    fu.setPosition(Float4{20,20,0,1});
    auto inim = fu.getProjectionViewMatrix();
    auto vm = fu.getViewMatrix();
    vm.print();
inim.print();
    
 
    auto fff = inim*Float4 {-0.5, -0.5, 0, 1};
    cout<<fff.x<<" "<<fff.y<<" "<<fff.z<<" "<<fff.w;
    return ;
    
    fu.getProjectionMatrix().print();
    cout<<"in ViewFrustum:\n";
    fu.setFrustum(-0.5,0.5,-0.5,0.5,1,10);
    //fu.getProjectionMatrix().print();
    Matrix44 result {
    2.000000,0.000000,0.000000,0.000000,
    0.000000,2.000000,0.000000,0.000000,
    0.000000,0.000000,-1.222222,-2.222222,
    0.000000,0.000000,-1.000000,0.000000};
    EXPECT_EQ( result , fu.getProjectionMatrix());

    Float4 eye{0,0,10,1};
    Float4 target{0,0,-2,1};
    #if 0
    Float4 zaxis = (eye - target).normalize(); // The "forward" vector.
    Float4 xaxis = up.cross(zaxis);            // The "right" vector.
    Float4 yaxis = zaxis.cross(xaxis);         // The "up" vector.
    fu.setFrame(eye,xaxis,yaxis,zaxis);
#endif
    AxisAngle3 temp({1,0,0},0);
    Matrix44 r;
    Rotation::convert(temp,r);

    //fu.getViewMatrix().print();
    fu.setPosition(eye);
    fu.setAxis(r.axis(0),r.axis(1),r.axis(2));
    //fu.getViewMatrix().print();
    EXPECT_EQ(fu.getViewMatrix(),(Matrix44{
                1.000000,0.000000,0.000000,0.000000,
                0.000000,1.000000,0.000000,0.000000,
                0.000000,0.000000,1.000000,-10.000000,
                0.000000,0.000000,0.000000,1.000000
                    }));

    temp.set({1,0,0},-6*_DegToRad);
    temp.normalize();
    Rotation::convert(temp,r);
    fu.setPosition(eye);
    fu.setAxis(r.axis(0),r.axis(1),r.axis(2));
    EXPECT_EQ(fu.getViewMatrix(),(Matrix44{
                1.000000,0.000000,0.000000,-0.000000,
                    0.000000,0.994522,-0.104528,1.045285,
                    0.000000,0.104528,0.994522,-9.945219,
                    0.000000,0.000000,0.000000,1.000000
                       }));

}

TEST(ViewFrustum,Orthographic){
    ViewFrustum fu(ViewFrustum::ORTHOGRAPHIC);
    //cout<<"in ViewFrustum:\n";
    fu.setFrustum(-0.5,0.5,-0.5,0.5,1,10);
    //fu.getProjectionMatrix().print();
    Matrix44 result {
    2.000000,0.000000,0.000000,0.000000,
    0.000000,2.000000,0.000000,0.000000,
    0.000000,0.000000,-0.22222,-1.222222,
    0.000000,0.000000,0.000000,1.000000};
    EXPECT_EQ( result , fu.getProjectionMatrix());
}

