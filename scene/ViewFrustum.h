#pragma once

#include "Matrix.h"


class  ViewFrustum
{
public:
    static const bool  ORTHOGRAPHIC = false;
    static const bool  PERSPECTIVE  = true;
    // Construction and destruction.
    ViewFrustum(bool isPerspective = PERSPECTIVE);


    // translate T
    inline void setPosition(const Float4& position){
        position_ = position;
        onFrameChange();
    }

    // rotation  R
    void setAxis(
        const Float4& xVector, // Z axis
        const Float4& yVector, // Y axis
        const Float4& zVector);// X axis

    // Coordinate frame support.  The default coordinate frame {P;D,U,R}
    // is in right-handed world coordinates where
    //   position  P = (0, 0,  0; 1)
    //   direction D = (0, 0, -1; 0)  Z
    //   up        U = (0, 1,  0; 0)  Y
    //   right     R = (1, 0,  0; 0)  X
    // The basis {D,U,R} is required to be a right-handed orthonormal set.        
    inline void setFrame(
        const Float4& position,
        const Float4& xVector,
        const Float4& yVector,
        const Float4& zVector){
        position_ = position;
        setAxis(xVector, yVector, zVector);
    }

    inline void getAxis(
         Float4& xVector,
         Float4& yVector,
         Float4& zVector) const{
        zVector = zVector_;
        yVector = yVector_;
        xVector = xVector_;
    }

    inline void getFrame(
         Float4& position,
         Float4& xVector,
         Float4& yVector,
         Float4& zVector) const{
        position = position_;
        zVector = zVector_;
        yVector = yVector_;
        xVector = xVector_;
    }

    inline const Float4& getPosition() const{return position_;}
    inline const Float4& getZVector()  const{return zVector_;}
    inline const Float4& getYVector()  const{return yVector_;}
    inline const Float4& getXVector()  const{return xVector_;}
    inline  Float4& getPosition() {return position_;}
    inline  Float4& getZVector()  {return zVector_;}
    inline  Float4& getYVector()  {return yVector_;}
    inline  Float4& getXVector()  {return xVector_;}
    // Access the view matrix of the coordinate frame.  If D = (d0,d1,d2),
    // U = (u0,u1,u2), and R = (r0,r1,r2), then the view matrix is
    //     +-                  -+
    //     | r0 r1 r2 -Dot(R,P) |
    // V = | u0 u1 u2 -Dot(U,P) |
    //     | d0 d1 d2 -Dot(D,P) |
    //     |  0  0  0         1 |
    //     +-                  -+
    // the view matrix multiplies vectors on its right, viewMat * vector4.
    // A point X = (x0,x1,x2,1) can be represented by X = P + y0*R + y1*U + y2*D,
    // where y0 = Dot(R,X-P), y1 = Dot(U,X-P), and y2 = Dot(D,X-P).
    // if Y = (y0,y1,y2,1), then Y = V*X, where V is the view matrix.
    inline const Matrix44& getViewMatrix() const{return viewMatrix_;}

    enum
    {
        VF_ZMIN = 0,  // near
        VF_ZMAX = 1,  // far
        VF_YMIN = 2,  // bottom
        VF_YMAX = 3,  // top
        VF_XMIN = 4,  // left
        VF_XMAX = 5,  // right
        VF_QUANTITY = 6
    };

    inline bool isPerspective() const {return isPerspective_;}

    inline void setFrustum(float xMin, float xMax, float yMin, float yMax,
        float zMin, float zMax){
        frustum_[VF_ZMIN] = zMin;
        frustum_[VF_ZMAX] = zMax;
        frustum_[VF_YMIN] = yMin;
        frustum_[VF_YMAX] = yMax;
        frustum_[VF_XMIN] = xMin;
        frustum_[VF_XMAX] = xMax;
        onFrustumChange();
    }

    // the order:  dmin, dmax, umin, umax, rmin, rmax.
    inline void setFrustum(const float* frustum){
        frustum_[VF_ZMIN] = frustum[VF_ZMIN];
        frustum_[VF_ZMAX] = frustum[VF_ZMAX];
        frustum_[VF_YMIN] = frustum[VF_YMIN];
        frustum_[VF_YMAX] = frustum[VF_YMAX];
        frustum_[VF_XMIN] = frustum[VF_XMIN];
        frustum_[VF_XMAX] = frustum[VF_XMAX];
        onFrustumChange();
    }

    // upFovyDegrees in (0,180).
    void setFrustum(float upFovyDegrees,float width,float height,
                    float zMin,float zMax){
        assert(width>0 && height>0);
        auto halfAngleRadians = 0.5f * upFovyDegrees * (float)_DegToRad;
        frustum_[VF_YMAX] = zMin*tan(halfAngleRadians);
        frustum_[VF_XMAX] = width/height*frustum_[VF_YMAX];
        frustum_[VF_YMIN] = -frustum_[VF_YMAX];
        frustum_[VF_XMIN] = -frustum_[VF_XMAX];
        frustum_[VF_ZMIN] = zMin;
        frustum_[VF_ZMAX] = zMax;
       
        onFrustumChange();
    }
  

    bool getFrustum(float& upFovyDegrees, float& aspectRatio, float& zMin,
        float& zMax) const;

    inline float getZMin() const{return frustum_[VF_ZMIN];}
    inline float getZMax() const{return frustum_[VF_ZMAX];}
    inline float getYMin() const{return frustum_[VF_YMIN];}
    inline float getYMax() const{return frustum_[VF_YMAX];}
    inline float getXMin() const{return frustum_[VF_XMIN];}
    inline float getXMax() const{return frustum_[VF_XMAX];}

    /* Access the projection matrices of the camera.These matrices map to
    // depths in the interval [-1,1].
    // The frustum values are N (near), F (far), B (bottom), T (top),
    // L (left), and R (right).  The various matrices are as follows,
    // shown for GTE_USE_MAT_VEC.
    // R+L =0 R-L= 2R (width) , T+B =0 T-B =2T (height)
    // perspective
    //   +-                                                                                     -+
    //   | 2*N/(R-L)  0           (R+L)/(R-L)    0          | N/R  0     0           0           |
    //   | 0          2*N/(T-B)   (T+B)/(T-B)    0          |  0  N/T    0           0           |
    //   | 0          0           -F+N/(F-N)   -2*N*F/(F-N) |  0   0   -F+N/(F-N)  -2*N*F/(F-N)  |
    //   | 0          0           -1             0          |  0   0     -1          0           |
    //   +-                                                                                     -+
    //Note:------------------------------------------------------------------------------
    //   Znear Zfar in world map to -Znear ,-Zfar in clip space 
    //   in clip space transformed result vector.Wclip = -Zeyespace = -Zworld
    //   Zeyespace is map to -nearPlan*ratio in worldspzce,so Wc direct map to coordinate of -Z in worldspace
    //   Zn expressed by Zeye Weye  
    //   Zeye 在计算中强制映射到投影平面,一般就取近剪裁面-Znear。
    //   为了使用Z进行剪裁和深度测试，必须保留唯一的Z值，
    //   Zclip = (AZeye + BWeye) ((Weye always equal to 1))
    //         = (AZeye + B    ) (A = -2/(F-N) , B = -F+N/(F-N) )
    //
    //   Zndc  = Zclip          / Wclip 
    //         = (AZeye + B    )/ -Zeye  ( Zn和Zeye非线性关系,Z Fighting根源)
    //
    //   so   ==>  Zclip = Zndc * -Zeye
    //-----------------------------------------------------------------------------------
    // orthographic
    //   +-                                                                        -+
    //   | 2/(R-L)  0        0       -(R+L)/(R-L) | 1/R  0     0           0        |
    //   | 0        2/(T-B)  0       -(T+B)/(T-B) |  0  1/T    0           0        |
    //   | 0        0       -2/(F-N) -F+N/(F-N)   |  0   0   -2/(F-N)   -F+N/(F-N)  |
    //   | 0        0        0          1         |  0   0     0           1        |
    //   +-                                                                        -+
    */
    inline const Matrix44 & getProjectionMatrix() const{return projectionMatrix_;}
    inline void setProjectionMatrix(Matrix44 const& projMatrix){projectionMatrix_ = projMatrix; updatePVMatrix();}
    inline const Matrix44 & getProjectionViewMatrix() const {return projectionViewMatrix_;}
    inline const Matrix44 & getViweMatrix()const {return viewMatrix_;}

private:
    // Compute the view matrix after the frame changes and then update the
    // projection-view matrix.
    void onFrameChange();

    // Compute the projection matrices after the frustum changes and then
    // update the projection-view matrix.
    void onFrustumChange();

    // Compute the projection-view matrix.  Derived classes can include
    // pre-view and post-projection matrices in the product.
    inline void updatePVMatrix(){ projectionViewMatrix_ = projectionMatrix_ * viewMatrix_  ;}

    // The coordinate frame.
    Float4 position_, zVector_, yVector_, xVector_;

    Matrix44 viewMatrix_;
    Matrix44 projectionMatrix_;
    Matrix44 projectionViewMatrix_;

    // The view frustum, stored in order as dmin (near), dmax (far),
    // umin (bottom), umax (top), rmin (left), and rmax (right).
    float frustum_[VF_QUANTITY];
    bool isPerspective_;
};
 
   

