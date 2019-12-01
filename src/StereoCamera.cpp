#include "../tools/glm/glm.hpp"
#include "../tools/glm/gtc/matrix_transform.hpp"
#include "../tools/glm/gtc/type_ptr.hpp" //for glm::value_ptr
#include "../tools/glm/gtx/transform2.hpp" // for glm::translate, glm::rotate, glm::scale, glm::perspective
#define PI 3.141592653589793

class StereoCamera
{
public:
	glm::mat4 P;

    StereoCamera(float Convergence, float EyeSeparation, float AspectRatio, float FOV, float NearClippingDistance, float FarClippingDistance)
    {
        mConvergence            = Convergence; 
        mEyeSeparation          = EyeSeparation; 
        mAspectRatio            = AspectRatio; 
        mFOV                    = FOV * PI / 180.0f; 
        mNearClippingDistance   = NearClippingDistance;
        mFarClippingDistance    = FarClippingDistance;
    }

    void ApplyLeftFrustum()
    {
        float top, bottom, left, right;

        top     = mNearClippingDistance * tan(mFOV/2);
        bottom  = -top;

        float a = mAspectRatio * tan(mFOV/2) * mConvergence;

        float b = a - mEyeSeparation/2;
        float c = a + mEyeSeparation/2;

        left    = -b * mNearClippingDistance/mConvergence;
        right   =  c * mNearClippingDistance/mConvergence;

        // Set the Projection Matrix
		P = glm::mat4(1.0f); 
		P = glm::frustum(left, right, bottom, top, mNearClippingDistance, mFarClippingDistance);

    /*    // Displace the world to right
        glMatrixMode(GL_MODELVIEW);                     
        glLoadIdentity();   
        glTranslatef(mEyeSeparation/2, 0.0f, 0.0f);
	*/
    }

    void ApplyRightFrustum()
    {
        float top, bottom, left, right;

        top     = mNearClippingDistance * tan(mFOV/2);
        bottom  = -top;

        float a = mAspectRatio * tan(mFOV/2) * mConvergence;

        float b = a - mEyeSeparation/2;
        float c = a + mEyeSeparation/2;

        left    =  -c * mNearClippingDistance/mConvergence;
        right   =   b * mNearClippingDistance/mConvergence;

        // Set the Projection Matrix
        P = glm::mat4(1.0f); 
		P = glm::frustum(left, right, bottom, top, mNearClippingDistance, mFarClippingDistance);

    /*    // Displace the world to right
        glMatrixMode(GL_MODELVIEW);                     
        glLoadIdentity();   
        glTranslatef(-mEyeSeparation/2, 0.0f, 0.0f);
	*/
    }

private:
    float mConvergence;
    float mEyeSeparation;
    float mAspectRatio;
    float mFOV;
    float mNearClippingDistance;
    float mFarClippingDistance;

};