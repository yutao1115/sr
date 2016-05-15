#pragma once
#include "ViewFrustum.h"

class CameraRig
{
public:
    // moving the camera around in a world and updating the pvw-matrices
    CameraRig(ViewFrustum& view,float translationSpeed,float rotationSpeed):
        v_(view),
        translationSpeed_(translationSpeed),
        rotationSpeed_(rotationSpeed) {}

    void setTranslationSpeed(float translationSpeed){translationSpeed_=translationSpeed;}
    float getTranslationSpeed() const{return translationSpeed_;}
    void setRotationSpeed(float rotationSpeed){rotationSpeed_=rotationSpeed;}
    float getRotationSpeed() const{return rotationSpeed_;}
    
    void lookAt(const Float4& eye,const Float4& center,const Float4& up){
        Float4 zaxis = (eye - center).normalize(); // The "forward" vector.
        Float4 xaxis = up.cross(zaxis).normalize();// The "right" vector.
        Float4 yaxis = zaxis.cross(xaxis);         // The "up" vector.
        v_.setFrame(eye,xaxis,yaxis,zaxis);
    }
    

    void standPosition(float x,float y,float z){v_.setPosition(Float4{x,y,z,1});}
    void forward() {v_.setPosition(v_.getPosition() - v_.getZVector()*translationSpeed_  );}
    void backward(){v_.setPosition(v_.getPosition() + v_.getZVector()*translationSpeed_  );}
    void up()      {v_.setPosition(v_.getPosition() - v_.getYVector()*translationSpeed_  );}
    void down()    {v_.setPosition(v_.getPosition() + v_.getYVector()*translationSpeed_  );}
    void right()   {v_.setPosition(v_.getPosition() - v_.getXVector()*translationSpeed_  );}
    void left()    {v_.setPosition(v_.getPosition() + v_.getXVector()*translationSpeed_  );}
    void turnRight();
    void turnLeft();
    void lookUp();
    void lookDown();
    void rollClockwise();
    void rollCounterclockwise();
private:
    ViewFrustum&  v_;
    float translationSpeed_;
    float rotationSpeed_;
};
