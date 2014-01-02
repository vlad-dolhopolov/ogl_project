#ifndef GAME_2_H_
#define GAME_2_H_

#include "GLSH.h"

class Game2 : public glsh::App {

    GLuint                  mUColorProgram;     // uniform color program
    GLuint                  mVColorProgram;     // per-vertex color program

    glsh::Mesh*             mTriMesh;
    glsh::Mesh*             mQuadMesh;

    // animation parameters for the spinning object
    float                   mSpinnerAngle;              // in radians
    float                   mSpinnerAngularVelocity;    // in radians per second

    // animation parameters for the bouncing object
    float                   mMinX, mMinY;
    float                   mMaxX, mMaxY;
    glm::vec3               mBouncerPosition;
    glm::vec3               mBouncerVelocity;

    // animation parameters for the scaling object
    float                   mMinScale;
    float                   mMaxScale;
    float                   mCurrScale;     // current scale
    float                   mScalingSpeed;

    // animation parameters for the color blending object
    glm::vec4               mBlendColor1;
    glm::vec4               mBlendColor2;
    float                   mAlpha;         // to lerp between color1 and color2
    float                   mBlendSpeed;
    glm::vec4               mBlendedColor;

public:
                            Game2();
                            ~Game2();

    void                    initialize(int w, int h)    override;
    void                    shutdown()                  override;
    void                    resize(int w, int h)        override;
    void                    draw()                      override;
    bool                    update(float dt)            override;
};

#endif
