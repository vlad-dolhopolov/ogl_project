#ifndef GAME_2_H_
#define GAME_2_H_

#include <array>

#include "GLSH.h"

class MatrixTexture : public glsh::App {

	GLuint					mTextTintProgram;

	glsh::Font*             mFont;
	glsh::TextBatch         mTextBatch;

	GLuint                  mSampler;

	float                   mScrWidth;
	float					mScrHeight;

	int						mSymTableWidth;
	int						mSymTableHeight;
	char**					mSymbolTable;

	std::array<char, 25>	mSymbols;
	
public:
                            MatrixTexture();
                            ~MatrixTexture();

    void                    initialize(int w, int h)    override;
    void                    shutdown()                  override;
    void                    resize(int w, int h)        override;
    void                    draw()                      override;
    bool                    update(float dt)            override;

private:
	void					DrawTextArea(const glsh::TextBatch& textBatch,
                                         const glm::vec2& pos,  // position of top-left corner in screen space
                                         const glm::vec4& textColor);

	void					UpdateSymTable();
    
};

#endif
