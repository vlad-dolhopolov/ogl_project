#ifndef GAME_H_
#define GAME_H_

#include "GLSH.h"

#include "MatrixTexture.h"

#include <vector>


class Scene : public glsh::App {

    GLuint							mUColorProgram;
    GLuint							mTexProgram;
    GLuint							mTexTintProgram;

	std::vector<glsh::Mesh*>        mCreatedMeshes;		// simple geometry created using GLSH functions
    std::vector<glsh::Mesh*>        mGeneratedMeshes;	// procedurally generated geometry
	std::vector<glsh::Mesh*>        mLoadedMeshes;		// geometry loaded from OBJ file
	std::vector<glsh::Mesh*>        mActiveMeshes;		// meshes which need to be drawn

    glm::mat4						mMeshRotMatrix;

    glsh::FreeLookCamera*			mCamera;

    GLuint							mSampler;   

    MatrixTexture*					mMatrixGenerator;     
    bool							mGame2Paused;

    GLsizei							mFBOWidth, mFBOHeight;
    GLuint							mFBO;

	GLuint							mMatrixTex;

	int								mMinFilterIndex;    // minification filter index
	float							mMaxAnisotropy;     // max supported anisotropy

	float							frustum[6][4];		// camera frustum
	glm::mat4						clippingMatrix;		// clipping matrix; required in frustum calculation

public:
									Scene();
									~Scene();

    void							initialize(int w, int h)    override;
    void							shutdown()                  override;
    void							resize(int w, int h)        override;
    void							draw()                      override;
    bool							update(float dt)            override;

	void							applyFilteringSettings(GLuint sampler);
	void							generateGeometry();
	void							calculateFrustum(glm::mat4 projMatrix, glm::mat4 mdvMatrix);
};

#endif