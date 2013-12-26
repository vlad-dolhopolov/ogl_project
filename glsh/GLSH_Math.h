#ifndef GLSH_MATH_H_
#define GLSH_MATH_H_

// gl math
#include <glm/glm.hpp>                      // glm::vec3, glm::vec4, glm::ivec4, glm::mat4, ...
#include <glm/gtc/matrix_transform.hpp>     // glm::translate, glm::rotate, glm::scale, glm::perspective, ...
#include <glm/gtc/type_ptr.hpp>             // glm::value_ptr

namespace glsh {

inline glm::mat4 CreateTranslation(float x, float y, float z)
{
    glm::mat4 m(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                x, y, z, 1);
    return m;
}

inline glm::mat4 CreateTranslation(const glm::vec3& t)
{
    glm::mat4 m(  1,   0,   0, 0,
                  0,   1,   0, 0,
                  0,   0,   1, 0,
                t.x, t.y, t.z, 1);
    return m;
}

inline glm::mat4 CreateScale(float s)
{
    glm::mat4 m(s, 0, 0, 0,
                0, s, 0, 0,
                0, 0, s, 0,
                0, 0, 0, 1);
    return m;
}

inline glm::mat4 CreateScale(float sx, float sy, float sz)
{
    glm::mat4 m(sx,  0,  0, 0,
                 0, sy,  0, 0,
                 0,  0, sz, 0,
                 0,  0,  0, 1);
    return m;
}

inline glm::mat4 CreateScale(const glm::vec3& s)
{
    glm::mat4 m(s.x,   0,   0, 0,
                  0, s.y,   0, 0,
                  0,   0, s.z, 0,
                  0,   0,   0, 1);
    return m;
}

inline glm::mat4 CreateRotationZ(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    glm::mat4 m( c, s, 0, 0,
                -s, c, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1);
    return m;
}

inline glm::mat4 CreateRotationY(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    glm::mat4 m( c,  0, -s, 0,
                 0,  1,  0, 0,
                 s,  0,  c, 0,
                 0,  0,  0, 1);
    return m;
}

inline glm::mat4 CreateRotationX(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    glm::mat4 m( 1,  0,  0, 0,
                 0,  c,  s, 0,
                 0, -s,  c, 0,
                 0,  0,  0, 1);
    return m;
}

}

#endif
