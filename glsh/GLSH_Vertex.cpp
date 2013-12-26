#include "GLSH_Vertex.h"

namespace glsh {

// default constructor (creates empty attribute list)
VertexFormat::VertexFormat()
    : size(0)
{ }

// constructors for easily creating VertexFormats with a few attributes

VertexFormat::VertexFormat(const VertexAttrib& va)
    : size(0)
{
    attribs.push_back(va);  size += va.getSizeInBytes();
}

VertexFormat::VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2)
    : size(0)
{
    attribs.push_back(va1);  size += va1.getSizeInBytes();
    attribs.push_back(va2);  size += va2.getSizeInBytes();
}

VertexFormat::VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2, const VertexAttrib& va3)
    : size(0)
{
    attribs.push_back(va1);  size += va1.getSizeInBytes();
    attribs.push_back(va2);  size += va2.getSizeInBytes();
    attribs.push_back(va3);  size += va3.getSizeInBytes();
}

VertexFormat::VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2, const VertexAttrib& va3, const VertexAttrib& va4)
    : size(0)
{
    attribs.push_back(va1);  size += va1.getSizeInBytes();
    attribs.push_back(va2);  size += va2.getSizeInBytes();
    attribs.push_back(va3);  size += va3.getSizeInBytes();
    attribs.push_back(va4);  size += va4.getSizeInBytes();
}


const VertexFormat& VertexPosition::GetFormat()
{
    static VertexFormat fmt(VertexAttrib(VA_POSITION, 3, GL_FLOAT, 3 * sizeof(GLfloat), (void*)0));
    return fmt;
}

const VertexFormat& VertexPositionColor::GetFormat()
{
    static VertexFormat fmt(VertexAttrib(VA_POSITION, 3, GL_FLOAT, 7 * sizeof(GLfloat), (void*)0),
                            VertexAttrib(VA_COLOR,    4, GL_FLOAT, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));
    return fmt;
}

const VertexFormat& VertexPositionTexture::GetFormat()
{
    static VertexFormat fmt(VertexAttrib(VA_POSITION, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0),
                            VertexAttrib(VA_TEXCOORD, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));
    return fmt;
}

const VertexFormat& VertexPositionNormal::GetFormat()
{
    static VertexFormat fmt(VertexAttrib(VA_POSITION, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)0),
                            VertexAttrib(VA_NORMAL,   3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))));
    return fmt;
}

const VertexFormat& VertexPositionNormalTexture::GetFormat()
{
    static VertexFormat fmt(VertexAttrib(VA_POSITION, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)0),
                            VertexAttrib(VA_NORMAL,   3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))),
                            VertexAttrib(VA_TEXCOORD, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))));
    return fmt;
}


GLsizei GetGLTypeSize(GLenum type)
{
    switch (type) {
    case GL_FLOAT:
    case GL_INT:
    case GL_UNSIGNED_INT:
        return 4;
    case GL_HALF_FLOAT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return 2;
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    default:
        // not a supported type
        return 0; // ??
    };
}

}
