#include "Scene.h"
#include "Util.h"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

Scene::Scene()
    : mTexProgram(0)
    , mCamera(NULL)
    , mSampler(0)
    , mGame2(NULL)
    , mGame2Paused(false)
    , mNeedGame2Render(true)
{
}

Scene::~Scene()
{
}

void Scene::initialize(int w, int h)
{
    // set screen clearing color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_DEPTH_TEST);    // !!!!!!111!!1!!!11!^&#(!@^(!!!!!!

    glEnable(GL_CULL_FACE);

    // load shaders
    mUColorProgram = glsh::BuildShaderProgram("shaders/ucolor-vs.glsl", "shaders/ucolor-fs.glsl");
    mTexProgram = glsh::BuildShaderProgram("shaders/TexDirLight-vs.glsl", "shaders/TexDirLight-fs.glsl");
    mTexTintProgram = glsh::BuildShaderProgram("shaders/TexNoLight-vs.glsl", "shaders/TexTintNoLight-fs.glsl");

    // create mesh geometry
    mMesh = glsh::CreateTexturedCube(5.0f);

    glGenSamplers(1, &mSampler);

    mCamera = new glsh::FreeLookCamera(this);
    mCamera->setPosition(0, 5.0f, 10.0f);
    mCamera->lookAt(0, 5.0f, 0);
    mCamera->setSpeed(2.0f);

    
    mGame2 = new Game2;
    mGame2->initialize(mFBOWidth, mFBOHeight);
    mGame2->resize(mFBOWidth, mFBOHeight);

	//
    // Create texture to render Game2 into and attach it to a Framebuffer Object
    //
    {
        mFBOWidth = 256;
        mFBOHeight = 256;

        // create texture
        glGenTextures(1, &mMatrixTex);
        glBindTexture(GL_TEXTURE_2D, mMatrixTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mFBOWidth, mFBOHeight,
                     0, GL_RGB, GL_UNSIGNED_BYTE, NULL);            // allocate texture without sending any data

        // only one valid mipmap level for now
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        // create framebuffer object
        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mMatrixTex, 0);  // set framebuffer texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  // unbind for now
    }
}

void Scene::shutdown()
{
    mGame2->shutdown();
    delete mGame2;

    // FIXME: cleanup
}

void Scene::resize(int w, int h)
{
    // set viewport (subrect of screen to draw on)
    //glViewport(0, 0, w, h);

    mCamera->setViewportSize(w, h); 
}

void Scene::draw()
{
    //
    // render to texture, if needed
    //
    if (mNeedGame2Render) {

        glDisable(GL_MULTISAMPLE);  // our FBO doesn't support multisampling

        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);    // bind framebuffer object (FBO)
        glViewport(0, 0, mFBOWidth, mFBOHeight);    // match viewport to FBO size
        mGame2->draw();                             // render to FBO

        glBindFramebuffer(GL_FRAMEBUFFER, 0);       // unbind FBO

        // generate mipmaps for rendered texture
		glBindTexture(GL_TEXTURE_2D, mMatrixTex);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    glEnable(GL_DEPTH_TEST);

    // MSAA
    glEnable(GL_MULTISAMPLE);  // NOTE: render target must have been configured to support this

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, mSampler);

    glm::mat4 projMatrix = mCamera->getProjectionMatrix();
    glm::mat4 viewMatrix = mCamera->getViewMatrix();

    glUseProgram(mTexProgram);

    glsh::SetShaderUniformInt("u_TexSampler", 0);

    glsh::SetShaderUniform("u_ProjectionMatrix", projMatrix);

    // set lighting parameters
    glm::vec3 lightDir(1.0f, 3.0f, 2.0f);           // direction to light in world space
    lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
    lightDir = glm::normalize(lightDir);            // normalized for sanity
    glsh::SetShaderUniform("u_LightDir", lightDir);
    glsh::SetShaderUniform("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    
    //
    // draw cube
    //
    glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    
    glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    

    glBindTexture(GL_TEXTURE_2D, mMatrixTex);

    glm::mat4 T = glsh::CreateTranslation(0.0f, 5.0f, 0.0f);
    glsh::SetShaderUniform("u_ModelviewMatrix", viewMatrix * T * mMeshRotMatrix);
    glsh::SetShaderUniform("u_NormalMatrix", glm::mat3(viewMatrix * mMeshRotMatrix));

    mMesh->draw();
    
	GLSH_CHECK_GL_ERRORS("drawing");
}

bool Scene::update(float dt)
{
    const glsh::Keyboard* kb = getKeyboard();

    if (kb->keyPressed(glsh::KC_ESCAPE)) {
        return false; // request to exit
    }
    
    mGame2->update(dt);

    //
    // Pitch and yaw in local space.
    // Hold CTRL to pitch and yaw in camera space.
    //
    const float rotSpeed = glsh::PI;

    float yaw = 0;
    float pitch = 0;
    if (kb->isKeyDown(glsh::KC_LEFT)) {
        yaw -= dt * rotSpeed;
    }
    if (kb->isKeyDown(glsh::KC_RIGHT)) {
        yaw += dt * rotSpeed;
    }
    if (kb->isKeyDown(glsh::KC_UP)) {
        pitch -= dt * rotSpeed;
    }
    if (kb->isKeyDown(glsh::KC_DOWN)) {
        pitch += dt * rotSpeed;
    }

    // rotate the mesh
    if (kb->isKeyDown(glsh::KC_CTRL)) {
        // apply rotations about the camera axes
        glm::vec3 xAxis = mCamera->getRight();
        glm::vec3 yAxis = mCamera->getUp();
        glm::quat yawQuat = glsh::CreateQuaternion(yaw, yAxis);
        glm::quat pitchQuat = glsh::CreateQuaternion(pitch, xAxis);
        glm::quat Q = pitchQuat * yawQuat;
        mMeshRotMatrix = glm::toMat4(Q) * mMeshRotMatrix;
    } else {
        // apply rotations about the model's local axes
        glm::vec3 xAxis = glm::vec3(mMeshRotMatrix[0]);
        glm::vec3 yAxis = glm::vec3(mMeshRotMatrix[1]);
        glm::quat yawQuat = glsh::CreateQuaternion(yaw, yAxis);
        glm::quat pitchQuat = glsh::CreateQuaternion(pitch, yawQuat * xAxis);
        glm::quat Q = pitchQuat * yawQuat;
        mMeshRotMatrix = glm::toMat4(Q) * mMeshRotMatrix;
    }

    // reset mesh orientation
    if (kb->keyPressed(glsh::KC_R)) {
        mMeshRotMatrix = glm::mat4(1.0f);
    }

    bool fboResize = false;
    // cycle through FBO sizes
    if (kb->keyPressed(glsh::KC_5)) {
        if (mFBOWidth > 32 && mFBOHeight > 32) {
            // divide in half
            mFBOWidth >>= 1;
            mFBOHeight >>= 1;
            fboResize = true;
        }
    }
    if (kb->keyPressed(glsh::KC_6)) {
        if (mFBOWidth < 1024 && mFBOHeight < 1024) {
            // double the dimensions
            mFBOWidth <<= 1;
            mFBOHeight <<= 1;
            fboResize = true;
        }
    }

    if (fboResize) {
        glBindTexture(GL_TEXTURE_2D, mMatrixTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mFBOWidth, mFBOHeight,
                     0, GL_RGB, GL_UNSIGNED_BYTE, NULL);        // reallocate texture without sending any data

        // only one valid mipmap level for now
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }

    mCamera->update(dt);

    return true; // request to keep going
}
