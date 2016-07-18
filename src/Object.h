#pragma once

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <lodepng.h>
#include <vector>
#include <math.h>
#include <string.h>
#include <iostream>
#include <cstdio>

#include "GL/glew.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "GLUT/glut.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"



using namespace std;
using namespace glm;

namespace Objects {
    class Object {
    public:
        int type;
        string path;
        string name;
        bool loaded = false;

        int verCount;
        float* vertices;
        float * vtexture;
        float * vnormals;

        mat4 M;

        GLuint objectVao;
        GLuint vertexbuffer;
        GLuint vertexUV;
        GLuint bufNormals;

        GLint texModel;
        GLint texRef;

        vec3 rotate;
        vec3 translate;
        vec3 scale;

        Object(int _type, string _name, char* _pathTex, char* _pathRef, vec3 rotate, vec3 translate, vec3 scale);

        void draw();

        void loadModel(int _v, float* _p, float* _t, float* _n);

        GLuint readTexture(char* filename);

        void setValue(GLuint value, GLuint &valueToSet);
    };
    extern Object object;
}
#endif