#include "Scene.h"
#include "Util.h"
#include "Wavefront.h"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

struct MinFilter {
    GLint           mode;
    const char*     modeStr;
    float           anisotropy;
    const char*     descr;
};

struct MagFilter {
    GLint           mode;
    const char*     modeStr;
    const char*     descr;
};

const MinFilter g_minFilters[] = {
    { GL_NEAREST,               "GL_NEAREST",               1.0f,   "Nearest, no mipmaps"       },
    { GL_LINEAR,                "GL_LINEAR",                1.0f,   "Bilinear, no mipmaps"      },
    { GL_LINEAR_MIPMAP_NEAREST, "GL_LINEAR_MIPMAP_NEAREST", 1.0f,   "Bilinear, nearest mipmap"  },
    { GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  1.0f,   "Trilinear"                 },
    { GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  2.0f,   "Anisotropic x2"            },
    { GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  4.0f,   "Anisotropic x4"            },
    { GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR",  8.0f,   "Anisotropic x8"            },
    { GL_LINEAR_MIPMAP_LINEAR,  "GL_LINEAR_MIPMAP_LINEAR", 16.0f,   "Anisotropic x16"           },
};
const int g_numMinFilters = sizeof(g_minFilters) / sizeof(g_minFilters[0]);

Scene::Scene()
    : mTexProgram(0)
    , mCamera(NULL)
    , mSampler(0)
    , mMatrixGenerator(NULL)
    , mGame2Paused(false)
	, mMinFilterIndex(0)
{
}

Scene::~Scene()
{
}

void Scene::initialize(int w, int h)
{
    // set screen clearing color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // load shaders
    mUColorProgram = glsh::BuildShaderProgram("shaders/ucolor-vs.glsl", "shaders/ucolor-fs.glsl");
    mTexProgram = glsh::BuildShaderProgram("shaders/TexNoLight-vs.glsl", "shaders/TexNoLight-fs.glsl");
    mTexTintProgram = glsh::BuildShaderProgram("shaders/TexNoLight-vs.glsl", "shaders/TexTintNoLight-fs.glsl");

    // create mesh geometry
	// create textured cube
	mCreatedMeshes.push_back(glsh::CreateTexturedCube(2.5f));

	// load geometry from OBJ file
	mLoadedMeshes.push_back(LoadWavefrontOBJ("models/hall.obj"));

	// procedurally generate a room from textured qauds
	generateGeometry();

	mActiveMeshes = mCreatedMeshes;

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mMaxAnisotropy);

    glGenSamplers(1, &mSampler);
	ApplyFilteringSettings(mSampler);

    mCamera = new glsh::FreeLookCamera(this);
    mCamera->setPosition(0.0f, 1.0f, 7.0f);
    mCamera->lookAt(0.0f, 1.0f, 0.0f);
    mCamera->setSpeed(2.0f);

	//
    // Create texture to render Game2 into and attach it to a Framebuffer Object
    //
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
    

	mMatrixGenerator = new MatrixTexture;
    mMatrixGenerator->initialize(mFBOWidth, mFBOHeight);
	mMatrixGenerator->resize(mFBOWidth, mFBOHeight);
}

void Scene::shutdown()
{
    mMatrixGenerator->shutdown();
    delete mMatrixGenerator;

	for (std::vector<glsh::Mesh*>::iterator meshItr = mCreatedMeshes.begin(); meshItr != mCreatedMeshes.end(); meshItr++) {
		//mMeshes.erase(meshItr);
		delete *meshItr;
	}

	for (std::vector<glsh::Mesh*>::iterator meshItr = mGeneratedMeshes.begin(); meshItr != mGeneratedMeshes.end(); meshItr++) {
		//mMeshes.erase(meshItr);
		delete *meshItr;
	}

	for (std::vector<glsh::Mesh*>::iterator meshItr = mLoadedMeshes.begin(); meshItr != mLoadedMeshes.end(); meshItr++) {
		//mMeshes.erase(meshItr);
		delete *meshItr;
	}
    // FIXME: cleanup
}

void Scene::resize(int w, int h)
{
    // set viewport (subrect of screen to draw on)
    glViewport(0, 0, w, h);

    mCamera->setViewportSize(w, h); 
}

void Scene::draw()
{
    //
    // render to texture, if needed
    //
    glDisable(GL_MULTISAMPLE);  // our FBO doesn't support multisampling

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);    // bind framebuffer object (FBO)
    glViewport(0, 0, mFBOWidth, mFBOHeight);    // match viewport to FBO size
    mMatrixGenerator->draw();                        // render to FBO

    glBindFramebuffer(GL_FRAMEBUFFER, 0);       // unbind FBO

    // generate mipmaps for rendered texture
	glBindTexture(GL_TEXTURE_2D, mMatrixTex);
    glGenerateMipmap(GL_TEXTURE_2D);

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
    glm::vec3 lightDir(0.0f, 3.0f, 3.0f);			// direction to light in world space
    lightDir = glm::mat3(viewMatrix) * lightDir;    // direction to light in camera space
    lightDir = glm::normalize(lightDir);            // normalized for sanity
	
    glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);    

    //glBindTexture(GL_TEXTURE_2D, mMatrixTex);

    glsh::SetShaderUniform("u_ModelviewMatrix", viewMatrix * mMeshRotMatrix);

	for (unsigned int i = 0; i < mActiveMeshes.size(); i++) {
		mActiveMeshes[i]->draw();
	}
	
	GLSH_CHECK_GL_ERRORS("drawing");
}

bool Scene::update(float dt)
{
    const glsh::Keyboard* kb = getKeyboard();

    if (kb->keyPressed(glsh::KC_ESCAPE)) {
        return false; // request to exit
    }
    
    mMatrixGenerator->update(dt);

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

	// choose geometry
	// draw primitive meshes
	if (kb->keyPressed(glsh::KC_1)) {
		mActiveMeshes = mCreatedMeshes;
    }

	// draw procedurally generated geometry
	if (kb->keyPressed(glsh::KC_2)) {
		mActiveMeshes = mGeneratedMeshes;
    }

	// draw geometry loaded from OBJ file
	if (kb->keyPressed(glsh::KC_3)) {
		mActiveMeshes = mLoadedMeshes;
    }

	bool filteringChanged = false;
	// cycle through minification filters
    if (kb->keyPressed(glsh::KC_9)) {
        if (mMinFilterIndex < g_numMinFilters - 1 && g_minFilters[mMinFilterIndex + 1].anisotropy <= mMaxAnisotropy) {
            ++mMinFilterIndex;
        } else {
            mMinFilterIndex = 0;
        }
        filteringChanged = true;
    }
    if (kb->keyPressed(glsh::KC_0)) {
        if (mMinFilterIndex > 0) {
            --mMinFilterIndex;
        } else {
            mMinFilterIndex = g_numMinFilters - 1;
        }
        filteringChanged = true;
    }

	//
    // update our texture sampler
    //
    if (filteringChanged) {
        ApplyFilteringSettings(mSampler);
    }

    mCamera->update(dt);

    return true; // request to keep going
}

void Scene::ApplyFilteringSettings(GLuint sampler)
{
    // get current settings
    const MinFilter& minFilter = g_minFilters[mMinFilterIndex];
    
    // configure the sampler
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minFilter.mode);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, minFilter.anisotropy);
}

void Scene::generateGeometry() {

	std::vector<glsh::VPNT> vertices;
	float unitLength = 1.0f; // lenght of the side of one quad; should not be modified!
	int roomWidth = 4; // width of the room; can be modified just for fun

	// far wall
	for (int i = 0; i < roomWidth - 1; i++) {
		vertices.push_back(glsh::VPNT(0.0f - unitLength * i - unitLength, 0.0f, 0.0f, 0, 0, 1, 0.5f, 0));
		vertices.push_back(glsh::VPNT(0.0f - unitLength * i, 0.0f, 0.0f, 0, 0, 1, 1, 0));
		vertices.push_back(glsh::VPNT(0.0f - unitLength * i - unitLength, 2 * unitLength, 0.0f, 0, 0, 1, 0.5f, 1));
		vertices.push_back(glsh::VPNT(0.0f - unitLength * i, 2 * unitLength, 0.0f, 0, 0, 1, 1, 1));

		mGeneratedMeshes.push_back(glsh::CreateMesh(GL_TRIANGLE_STRIP, vertices));
		vertices.clear();

		vertices.push_back(glsh::VPNT(0.0f + unitLength * i, 0.0f, 0.0f, 0, 0, 1, 0.5f, 0));
		vertices.push_back(glsh::VPNT(0.0f + unitLength * i + unitLength, 0.0f, 0.0f, 0, 0, 1, 1, 0));
		vertices.push_back(glsh::VPNT(0.0f + unitLength * i, 2 * unitLength, 0.0f, 0, 0, 1, 0.5f, 1));
		vertices.push_back(glsh::VPNT(0.0f + unitLength * i + unitLength, 2 * unitLength, 0.0f, 0, 0, 1, 1, 1));

		mGeneratedMeshes.push_back(glsh::CreateMesh(GL_TRIANGLE_STRIP, vertices));
		vertices.clear();
	}

	// right wall
	for (int i = 0; i < roomWidth; i++) {
		vertices.push_back(glsh::VPNT(roomWidth - unitLength, 0.0f, unitLength * i, -1, 0, 0, 0.5f, 0));
		vertices.push_back(glsh::VPNT(roomWidth - unitLength, 0.0f, unitLength * i + unitLength, -1, 0, 0, 1, 0));
		vertices.push_back(glsh::VPNT(roomWidth - unitLength, 2 * unitLength, unitLength * i, -1, 0, 0, 0.5f, 1));
		vertices.push_back(glsh::VPNT(roomWidth - unitLength, 2 * unitLength, unitLength * i + unitLength, -1, 0, 0, 1, 1));

		mGeneratedMeshes.push_back(glsh::CreateMesh(GL_TRIANGLE_STRIP, vertices));
		vertices.clear();
	}

	// left wall
	for (int i = 0; i < roomWidth; i++) {
		vertices.push_back(glsh::VPNT(-roomWidth + unitLength, 0.0f, unitLength * i + unitLength, 1, 0, 0, 0.5f, 0));
		vertices.push_back(glsh::VPNT(-roomWidth + unitLength, 0.0f, unitLength * i, 1, 0, 0, 1, 0));
		vertices.push_back(glsh::VPNT(-roomWidth + unitLength, 2 * unitLength, unitLength * i + unitLength, 1, 0, 0, 0.5f, 1));
		vertices.push_back(glsh::VPNT(-roomWidth + unitLength, 2 * unitLength, unitLength * i, 1, 0, 0, 1, 1));

		mGeneratedMeshes.push_back(glsh::CreateMesh(GL_TRIANGLE_STRIP, vertices));
		vertices.clear();
	}

	// floor
	for (int i = 1; i < roomWidth * 2 - 1; i++) {
		for(int j = 0; j < roomWidth - 1; j++) {

			vertices.push_back(glsh::VPNT(-roomWidth	 + unitLength * i, 0.0f, 2 * unitLength + unitLength * j, 0, 1, 0, 0.5f, 0));
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i + unitLength, 0.0f, 2 * unitLength + unitLength * j, 0, 1, 0, 1, 0));
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i, 0.0f, 0.0f + unitLength * j, 0, 1, 0, 0.5f, 1));
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i + unitLength, 0.0f, 0.0f + unitLength * j, 0, 1, 0, 1, 1));

			mGeneratedMeshes.push_back(glsh::CreateMesh(GL_TRIANGLE_STRIP, vertices));
			vertices.clear();
		}
	}

	// ceeling
	for (int i = 1; i < roomWidth * 2 - 1; i++) {
		for(int j = 0; j < roomWidth - 1; j++) {
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i, 2 * unitLength, 0.0f + unitLength * j, 0, -1, 0, 0.5, 0));
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i + unitLength, 2 * unitLength, 0.0f + unitLength * j, 0, -1, 0, 1, 0));
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i, 2 * unitLength, 2 * unitLength + unitLength * j, 0, -1, 0, 0.5, 1));
			vertices.push_back(glsh::VPNT(-roomWidth + unitLength * i + unitLength, 2 * unitLength, 2 * unitLength + unitLength * j, 0, -1, 0, 1, 1));

			mGeneratedMeshes.push_back(glsh::CreateMesh(GL_TRIANGLE_STRIP, vertices));
			vertices.clear();
		}
	}
}