#include "MatrixTexture.h"

MatrixTexture::MatrixTexture() 
	: mTextTintProgram(0)
	, mFont(nullptr)
	, mSampler(0)
	, mSymTableWidth(0)
	, mSymTableHeight(0)
	, mSymbolTable(nullptr)
{
}

MatrixTexture::~MatrixTexture()
{
	for (int i = 0; i < mSymTableHeight; i++)
	{
		delete [] mSymbolTable[i];
	}
	delete [] mSymbolTable;
}

void MatrixTexture::initialize(int w, int h)
{
    // set clearing (background) color
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

    // set blending function to alpha blending
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);

    // build shader programs
	mTextTintProgram = glsh::BuildShaderProgram("shaders/TexNoLight-vs.glsl", "shaders/TexTintNoLight-fs.glsl");

	glGenSamplers(1, &mSampler);

	mFont = glsh::CreateFont("fonts/Consolas13");

    // create symbol table
	mSymTableWidth = w / (int)mFont->getWidth() * 1.5f;
	mSymTableHeight = h / (int)mFont->getHeight();

	mSymbolTable = new char*[mSymTableHeight];
	for (int i = 0; i < mSymTableHeight; i++)
	{
		mSymbolTable[i] = new char[mSymTableWidth];
	}

	for (int i = 0; i < mSymTableHeight; i++) // rows
	{
		for (int j = 0; j < mSymTableWidth; j++) // column
		{
			mSymbolTable[i][j] = ' ';
		}
	}

	glsh::InitRandom();  // initialize random number generator

	GLSH_CHECK_GL_ERRORS("init");
}

void MatrixTexture::shutdown()
{
    glUseProgram(0);
    glDeleteProgram(mTextTintProgram);
}

void MatrixTexture::resize(int w, int h)
{
	GLSH_CHECK_GL_ERRORS("resizing");
    glViewport(0, 0, w, h);

	mScrWidth = (float)w;
    mScrHeight = (float)h;

	GLSH_CHECK_GL_ERRORS("resizing");
}

void MatrixTexture::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);

	glBindSampler(0, mSampler);
	glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(mSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glUseProgram(mTextTintProgram);
    glm::vec4 textColor(0.0f, 0.7f, 0.0f, 1.0f);

    float panelWidth = mTextBatch.GetWidth();
    float panelHeight = mTextBatch.GetHeight();

    float x = std::floor(0.5f * (mScrWidth - panelWidth));
	float y = (float)(mScrHeight - 1) - std::floor(0.5f * (mScrHeight - panelHeight));

    DrawTextArea(mTextBatch, glm::vec2(x, y), textColor);

	GLSH_CHECK_GL_ERRORS("drawing");
}


bool MatrixTexture::update(float dt)
{
	static float timer = 0.3f;

	if (timer > 0.1f)
	{
		UpdateSymTable();
		timer = 0;
	}

	timer += dt;

	GLSH_CHECK_GL_ERRORS("updating");
    return true;
}

void MatrixTexture::DrawTextArea(const glsh::TextBatch& textBatch, const glm::vec2& pos, const glm::vec4& textColor)
{
    glm::mat4 uiProj = glm::ortho(-0.5f, mScrWidth - 0.5f, -0.5f, mScrHeight - 0.5f, -1.0f, 1.0f);

    // draw text
    glsh::SetShaderUniform("u_ProjectionMatrix", uiProj);
    glsh::SetShaderUniform("u_ModelviewMatrix", glsh::CreateTranslation(pos.x, pos.y, 0.0f));
    glsh::SetShaderUniform("u_Tint", textColor);
	glsh::SetShaderUniformInt("u_TexSampler", 0);

    glBindTexture(GL_TEXTURE_2D, textBatch.GetFont()->getTex());

    textBatch.DrawGeometry();

	GLSH_CHECK_GL_ERRORS("text drawing");
}

void MatrixTexture::UpdateSymTable()
{
	static std::string str;
	static std::array<char, 17> symbols = { '~', '{', '@', '?', '%', '^', '&', '#', '`', ']', ':', '\'', '*', '>', '(' };

	str = "";
	for (int i = mSymTableHeight - 1; i > -1; i--) // rows
	{
		for (int j = 0; j < mSymTableWidth; j++) // column
		{
			if (i == 0)
			{
				str += mSymbolTable[i][j] = symbols[rand() % symbols.size()];
			}
			else if (j == 0)
			{
				str += mSymbolTable[i][j] = '\n';
			}
			else
			{
				str += mSymbolTable[i][j] = mSymbolTable[i - 1][j];
			}
		}
		str += "\n";
	}

	//for (int i = 0; i < mSymTableHeight; i++) // rows
	//{
	//	for (int j = 0; j < mSymTableWidth; j++) // column
	//	{
	//		mSymbolTable[i][j] = ' ';
	//	}
	//}

	//mTextBatch.SetText(mFont, str, true);
	mTextBatch.SetText(mFont, mSymbolTable, mSymTableWidth, mSymTableHeight, true);
}