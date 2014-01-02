#include "Game2.h"

Game2::Game2() 
    : mUColorProgram(0)
    , mVColorProgram(0)
    , mTriMesh(NULL)
    , mQuadMesh(NULL)
    , mSpinnerAngle(0.0f)
    , mSpinnerAngularVelocity(glm::radians(90.0f))
    , mMinX(-1.0f)
    , mMinY(-1.0f)
    , mMaxX(1.0f)
    , mMaxY(1.0f)
    , mBouncerPosition(0.0f, 0.0f, 0.0f)
    , mBouncerVelocity(0.0f, 0.0f, 0.0f)
    , mMinScale(0.5f)
    , mMaxScale(5.0f)
    , mCurrScale(mMinScale)
    , mScalingSpeed(2.0f)
    , mBlendColor1(1.0f, 1.0f, 0.0f, 1.0f)
    , mBlendColor2(1.0f, 0.0f, 0.0f, 1.0f)
    , mAlpha(1.0f)
    , mBlendSpeed(-0.5f)
    , mBlendedColor(mAlpha * mBlendColor1 + (1 - mAlpha) * mBlendColor2)
{
    //glsh::InitRandom();  // initialize random number generator

    mBouncerPosition.x = glsh::Random(mMinX, mMaxX);
    mBouncerPosition.y = glsh::Random(mMinY, mMaxY);

    float angle = glm::radians(glsh::Random(-180.0f, 180.0f));
    float speed = 0.75f;
    mBouncerVelocity.x = speed * std::cos(angle);
    mBouncerVelocity.y = speed * std::sin(angle);
}

Game2::~Game2()
{
}

void Game2::initialize(int w, int h)
{
    // set clearing (background) color
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

    // set blending function to alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // import some symbols from glsh namespace
    using glsh::VertexPosition;
    using glsh::VertexPositionColor;

    // define triangle mesh data (positions only)
    VertexPosition triVerts[] = {
        VertexPosition( 0.0f,  0.2f, 0.0f),
        VertexPosition(-0.2f, -0.2f, 0.0f),
        VertexPosition( 0.2f, -0.2f, 0.0f),
    };

    // define quad mesh data (positions and colors)
    VertexPositionColor quadVerts[] = {
        VertexPositionColor( -0.2f, -0.2f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f),  // bl
        VertexPositionColor(  0.2f, -0.2f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f),  // br
        VertexPositionColor( -0.2f,  0.2f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f),  // tl
        VertexPositionColor(  0.2f,  0.2f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f),  // tr
    };

    // create meshes
    mTriMesh = glsh::CreateMesh(GL_TRIANGLES, triVerts, 3);
    mQuadMesh = glsh::CreateMesh(GL_TRIANGLE_STRIP, quadVerts, 4);

    // build shader programs
    mUColorProgram = glsh::BuildShaderProgram("shaders/ucolor-vs.glsl", "shaders/ucolor-fs.glsl");
    mVColorProgram = glsh::BuildShaderProgram("shaders/vcolor-vs.glsl", "shaders/vcolor-fs.glsl");
}

void Game2::shutdown()
{
    delete mTriMesh;
    delete mQuadMesh;

    glUseProgram(0);
    glDeleteProgram(mUColorProgram);
    glDeleteProgram(mVColorProgram);
}

void Game2::resize(int w, int h)
{
    //glViewport(0, 0, w, h);
}

void Game2::draw()
{
    //glViewport(0, 0, w, h);

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    glm::mat4 M;

    // activate the per-vertex color shader program
    glUseProgram(mVColorProgram);

    glsh::SetShaderUniform("u_ProjectionMatrix", glm::mat4(1.0f));  // set default projection

    // bouncing object
    M = glm::mat4(1.0f);
    M = glm::translate(M, mBouncerPosition);
    glsh::SetShaderUniform("u_ModelviewMatrix", M);
    mQuadMesh->draw();

    // spinning object in top-right
    M = glm::mat4(1.0f);
    M = glm::translate(M, glm::vec3(0.5f, 0.5f, 0.0f));
    M = glm::rotate(M, glm::degrees(mSpinnerAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    glsh::SetShaderUniform("u_ModelviewMatrix", M);
    mQuadMesh->draw();

    // activate the uniform color shader program
    glUseProgram(mUColorProgram);

    glsh::SetShaderUniform("u_ProjectionMatrix", glm::mat4(1.0f));  // set default projection

    // scaling object at left
    M = glm::mat4(1.0f);
    M = glm::translate(M, glm::vec3(-0.5f, 0.0f, 0.0f));
    M = glm::rotate(M, 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    M = glm::scale(M, glm::vec3(1.0f, mCurrScale, 1.0f));
    glsh::SetShaderUniform("u_ModelviewMatrix", M);
    glsh::SetShaderUniform("u_Color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    mTriMesh->draw();

    // color-shifting object in bottom-right
    M = glm::mat4(1.0f);
    M = glm::translate(M, glm::vec3(0.5f, -0.5f, 0.0f));
    M = glm::scale(M, glm::vec3(1.3f, 1.3f, 1.0f));
    glsh::SetShaderUniform("u_ModelviewMatrix", M);
    glsh::SetShaderUniform("u_Color", mBlendedColor);
    mTriMesh->draw();

    // fading object in bottom-left
    M = glm::mat4(1.0f);
    M = glm::translate(M, glm::vec3(-0.5f, -0.5f, 0.0f));
    M = glm::scale(M, glm::vec3(1.3f, 1.3f, 1.0f));
    glsh::SetShaderUniform("u_ModelviewMatrix", M);
    glsh::SetShaderUniform("u_Color", glm::vec4(0.0f, 1.0f, 1.0f, mAlpha));
    mQuadMesh->draw();      // note that per-vertex colors are ignored using this shader!
}


bool Game2::update(float dt)
{
    // update spinner angle
    mSpinnerAngle += dt * mSpinnerAngularVelocity;

    // keep the angle in standard range
    if (mSpinnerAngle > 180.0f) {
        mSpinnerAngle -= 360.0f;
    } else if (mSpinnerAngle < -180.0f) {
        mSpinnerAngle += 360.0f;
    }


    // update bouncer position
    mBouncerPosition += dt * mBouncerVelocity;

    // bounce off of the horizontal boundaries of the screen
    if ((mBouncerVelocity.x > 0 && mBouncerPosition.x > mMaxX) ||
        (mBouncerVelocity.x < 0 && mBouncerPosition.x < mMinX))
    {
        mBouncerVelocity.x *= -1;
    }

    // bounce off of the vertical boundaries of the screen
    if ((mBouncerVelocity.y > 0 && mBouncerPosition.y > mMaxY) ||
        (mBouncerVelocity.y < 0 && mBouncerPosition.y < mMinY))
    {
        mBouncerVelocity.y *= -1;
    }


    // update scaling object
    mCurrScale += dt * mScalingSpeed;

    // keep the scale within desired bounds
    if (mCurrScale > mMaxScale && mScalingSpeed > 0) {
        mCurrScale = mMaxScale;
        mScalingSpeed *= -1;
    } else if (mCurrScale < mMinScale && mScalingSpeed < 0) {
        mCurrScale = mMinScale;
        mScalingSpeed *= -1;
    }


    // update alpha
    mAlpha += dt * mBlendSpeed;

    // keep the alpha between 0 and 1
    if (mAlpha > 1 && mBlendSpeed > 0) {
        mAlpha = 1;
        mBlendSpeed *= -1;
        mBlendColor2 = glm::vec4(glsh::UnitRandom(), glsh::UnitRandom(), glsh::UnitRandom(), 1.0f);
    } else if (mAlpha < 0 && mBlendSpeed < 0) {
        mAlpha = 0;
        mBlendSpeed *= -1;
        mBlendColor1 = glm::vec4(glsh::UnitRandom(), glsh::UnitRandom(), glsh::UnitRandom(), 1.0f);
    }

    // lerp between the two colors
    mBlendedColor = mAlpha * mBlendColor1 + (1 - mAlpha) * mBlendColor2;

    return true;
}
