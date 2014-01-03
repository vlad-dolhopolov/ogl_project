#ifndef GAME_H_
#define GAME_H_

#include "GLSH.h"

#include "MatrixTexture.h"

#include <vector>


class Scene : public glsh::App {

    GLuint                  mUColorProgram;
    GLuint                  mTexProgram;
    GLuint                  mTexTintProgram;

    glsh::Mesh*             mMesh;
    glm::mat4               mMeshRotMatrix;

    glsh::FreeLookCamera*   mCamera;

    GLuint                  mSampler;   

    MatrixTexture*          mMatrixGenerator;     
    bool                    mGame2Paused;

    GLsizei                 mFBOWidth, mFBOHeight;
    GLuint                  mFBO;

	GLuint					mMatrixTex;

public:
                            Scene();
                            ~Scene();

    void                    initialize(int w, int h)    override;
    void                    shutdown()                  override;
    void                    resize(int w, int h)        override;
    void                    draw()                      override;
    bool                    update(float dt)            override;
};

#endif
