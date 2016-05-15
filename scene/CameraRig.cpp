#include "CameraRig.h"
#include "Rotation.h"


void CameraRig::turnRight(){
// Y
    Matrix44 incremental;
    Rotation::convert(
        AxisAngle4(v_.getYVector(), -rotationSpeed_),incremental);

    v_.setAxis(
        incremental * v_.getXVector(),
        incremental * v_.getYVector(),
        incremental * v_.getZVector());
}

void CameraRig::turnLeft(){
// Y
    Matrix44 incremental;
    Rotation::convert(
        AxisAngle4(v_.getYVector(), rotationSpeed_),incremental);

    v_.setAxis(
        incremental * v_.getXVector(),
        incremental * v_.getYVector(),
        incremental * v_.getZVector());
}

void CameraRig::lookUp(){
// x
    Matrix44 incremental;
    Rotation::convert(
        AxisAngle4(v_.getXVector(), rotationSpeed_),incremental);

    v_.setAxis(
        incremental * v_.getXVector(),
        incremental * v_.getYVector(),
        incremental * v_.getZVector());
}

void CameraRig::lookDown(){
// x
    Matrix44 incremental;
    Rotation::convert(
        AxisAngle4(v_.getXVector(), -rotationSpeed_),incremental);

    v_.setAxis(
        incremental * v_.getXVector(),
        incremental * v_.getYVector(),
        incremental * v_.getZVector());
}

void CameraRig::rollClockwise(){
// Z
    Matrix44 incremental;
    Rotation::convert(
        AxisAngle4(v_.getZVector(), rotationSpeed_),incremental);

    v_.setAxis(
        incremental * v_.getXVector(),
        incremental * v_.getYVector(),
        incremental * v_.getZVector());
}

void CameraRig::rollCounterclockwise(){
// Z
    Matrix44 incremental;
    Rotation::convert(
        AxisAngle4(v_.getZVector(), -rotationSpeed_),incremental);

    v_.setAxis(
        incremental * v_.getXVector(),
        incremental * v_.getYVector(),
        incremental * v_.getZVector());
}
