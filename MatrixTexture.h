#ifndef GAME_2_H_
#define GAME_2_H_

#include "GLSH.h"

class MatrixTexture : public glsh::App {

	GLuint					mTextTintProgram;

	glsh::Font*             mFont;

	glsh::TextBatch         mTextBatch;

	GLuint                  mSampler;

	float                   mScrWidth, mScrHeight;

	void					DrawTextArea(const glsh::TextBatch& textBatch,
                                         const glm::vec2& pos,  // position of top-left corner in screen space
                                         const glm::vec4& textColor);
    
public:
                            MatrixTexture();
                            ~MatrixTexture();

    void                    initialize(int w, int h)    override;
    void                    shutdown()                  override;
    void                    resize(int w, int h)        override;
    void                    draw()                      override;
    bool                    update(float dt)            override;
};

#endif
