#ifndef GLSH_VERTEX_H_
#define GLSH_VERTEX_H_

#include <GL/glew.h>

#include <vector>

namespace glsh {

//
// Vertex attribute locations.
// All vertex shaders must follow this convention (use layout(location=X) qualifiers)
//
enum VertexAttribIndex {
    VA_POSITION  = 0,
    VA_COLOR     = 1,
    VA_NORMAL    = 2,
    VA_TEXCOORD  = 3,
};

//
// Useful utilities
//
GLsizei GetGLTypeSize(GLenum type);  // get the size of GL_FLOAT, GL_INT, GL_UNSIGNED_SHORT, etc.

//
// A structure that holds the arguments to glVertexAttribPointer.
// In essence, it defines the layout of a vertex attribute in a vertex array or a VBO.
//
struct VertexAttrib {
    GLuint          index;
    GLint           size;   // number of components
    GLenum          type;
    GLsizei         stride;
    const GLvoid*   offset;

    // default constructor initializes everything to 0 (meaningless values)
    VertexAttrib()
        : index(0), size(0), type(0), stride(0), offset(0)
    { }

    VertexAttrib(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* offset)
        : index(index), size(size), type(type), stride(stride), offset(offset)
    { }

    GLsizei getSizeInBytes() const
    {
        return size * GetGLTypeSize(type);  // number of components times component size
    }
};


//
// A VertexFormat is defined as a list of VertexAttributes
//
struct VertexFormat {
    std::vector<VertexAttrib>   attribs;   // list of attributes
    GLsizei                     size;      // total size in bytes

    // default constructor (creates empty attribute list)
    VertexFormat();

    // constructors for easily creating VertexFormats with a few attributes
    VertexFormat(const VertexAttrib& va);
    VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2);
    VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2, const VertexAttrib& va3);
    VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2, const VertexAttrib& va3, const VertexAttrib& va4);
};


//
// a structure that stores vertex positions
//
struct VertexPosition {

    // the following floats will be laid out consecutively in memory like this: { x, y, z }
    GLfloat x, y, z;

    // default constructor initializes all members to 0 (except alpha, which defaults to 1)
    VertexPosition()
        : x(0), y(0), z(0)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPosition(GLfloat x, GLfloat y, GLfloat z)
        : x(x), y(y), z(z)
    { }

    static const VertexFormat& GetFormat();
};

//
// a structure that stores vertex position and color
//
struct VertexPositionColor {

    // the following floats will be laid out consecutively in memory like this: { x, y, z, r, g, b, a }
    GLfloat x, y, z;      // position
    GLfloat r, g, b, a;   // color

    // default constructor initializes all members to 0 (except alpha, which defaults to 1)
    VertexPositionColor()
        : x(0), y(0), z(0), r(0), g(0), b(0), a(1)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionColor(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
        : x(x), y(y), z(z), r(r), g(g), b(b), a(a)
    { }

    static const VertexFormat& GetFormat();
};


//
// a structure that stores vertex position and texture coordinates
//
struct VertexPositionTexture {

    // the following floats will be laid out consecutively in memory like this: { x, y, z, u, v }
    GLfloat x, y, z;      // position
    GLfloat u, v;         // texcoord

    // default constructor initializes all members to 0
    VertexPositionTexture()
        : x(0), y(0), z(0), u(0), v(0)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionTexture(GLfloat x, GLfloat y, GLfloat z, GLfloat u, GLfloat v)
        : x(x), y(y), z(z), u(u), v(v)
    { }

    static const VertexFormat& GetFormat();
};

//
// a structure that stores vertex position and normal
//
struct VertexPositionNormal {

    // the following floats will be laid out consecutively in memory like this: { x, y, z, nx, ny, nz }
    GLfloat x, y, z;      // position
    GLfloat nx, ny, nz;   // normal

    // default constructor initializes all members to 0 (CAUTION: this creates an invalid normal!)
    VertexPositionNormal()
        : x(0), y(0), z(0), nx(0), ny(0), nz(0)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionNormal(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz)
        : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz)
    { }

    static const VertexFormat& GetFormat();
};

//
// a structure that stores vertex position, normal, and texture coordinates
//
struct VertexPositionNormalTexture {

    // the following floats will be laid out consecutively in memory like this: { x, y, z, nx, ny, nz, u, v }
    GLfloat x, y, z;      // position
    GLfloat nx, ny, nz;   // normal
    GLfloat u, v;         // texcoord

    // default constructor initializes all members to 0 (CAUTION: this creates an invalid normal!)
    VertexPositionNormalTexture()
        : x(0), y(0), z(0), nx(0), ny(0), nz(0), u(0), v(0)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionNormalTexture(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat u, GLfloat v)
        : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), u(u), v(v)
    { }

    static const VertexFormat& GetFormat();
};

//
// Short aliases for vertex types (saves some typing and space)
//

typedef VertexPosition                  VP;
typedef VertexPositionColor             VPC;
typedef VertexPositionNormal            VPN;
typedef VertexPositionTexture           VPT;
typedef VertexPositionNormalTexture     VPNT;

}

#endif
