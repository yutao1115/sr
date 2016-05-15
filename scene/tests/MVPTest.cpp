#include "ViewFrustum.h"
#include "Rotation.h"
#include "Transform.cpp"
#include "ViewFrustum.cpp"
#include "CameraRig.cpp"
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
using namespace std;



TEST(MVP,allMatrix){
//world:
Matrix44 world_M={
0.707107, 0.000000, 0.707107, 2.000000,
0.000000, 2.000000, 0.000000, 0.000000,
-0.707107, 0.000000, 0.707107, 0.00000,
0.000000, 0.000000, 0.000000, 1.000000,};

//view:
Matrix44 view_M={
0.600000, 0.000000, -0.800000, -0.00000,
-0.411597, 0.857493, -0.308697, -0.0000,
0.685994, 0.514496, 0.514496, -5.830953,
0.000000, 0.000000, 0.000000, 1.000000,};

//project:
Matrix44 project_M={
0.002500, 0.000000, 0.000000, 0.000000,
0.000000, 0.003333, 0.000000, 0.000000,
0.000000, 0.000000, -1.004008, -2.004008,
0.000000, 0.000000, -1.000000, 0.000000, };

//project view:
Matrix44 projectView_M={
0.001500, 0.000000, -0.002000, 0.000000,
-0.001372, 0.002858, -0.001029, 0.000000,
-0.688744, -0.516558, -0.516558, 3.850315,
-0.685994, -0.514496, -0.514496, 5.830953,};

//world project view:
Matrix44 mvp_M={
0.002475,-0.000000,-0.000354,0.003000,
-0.000243,0.005717,-0.001698,-0.002744,
-0.121754,-1.033116,-0.852277,2.472827,
-0.121268,-1.028992,-0.848875,4.458964,};
    
    Transform worldTransform ;
    worldTransform.setTranslation(2.0f, 0.0f, 0.0f);
    worldTransform.setRotation( AxisAngle4{{0.0f, 1.0f, 0.0f,0},PiHalf/2.0f});
    worldTransform.setScale(1.0f, 2.0f, 1.0f);
    
    ViewFrustum viewfrustum;
    viewfrustum.setFrustum(-400,400,-300,300,1,500);    
    
    Float4 eye{4, 3, 3,1};
    Float4 target{0,0,0,1};
    Float4 up{0, 1, 0,1};
    CameraRig cam_(viewfrustum,5,PiHalf);
    cam_.lookAt(eye,target,up);

    printf("world:\r\n");
    worldTransform.getHMatrix().print();
    printf("view:\r\n");
    viewfrustum.getViweMatrix().print();
    printf("project:\r\n");    
    viewfrustum.getProjectionMatrix().print();
    printf("project view:\r\n");    
    viewfrustum.getProjectionViewMatrix().print();
    auto wp = viewfrustum.getProjectionViewMatrix() * worldTransform.getHMatrix();
    printf("world project view:\r\n");
    wp.print();
    
    EXPECT_EQ(worldTransform.getHMatrix() , world_M);
    EXPECT_EQ(viewfrustum.getViweMatrix() , view_M);
    EXPECT_EQ(viewfrustum.getProjectionMatrix() , project_M); 
    EXPECT_EQ(viewfrustum.getProjectionViewMatrix() , projectView_M); 
    EXPECT_EQ(wp , mvp_M); 
}
