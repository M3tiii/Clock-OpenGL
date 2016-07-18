#include "Object.h"

namespace Objects {

    Object::Object(int _id, string _name, char* _pathTex, char* _pathRef, vec3 rotate, vec3 translate, vec3 scale) {
        this->id = _id;
        this->name = _name;

        this->texModel = readTexture(_pathTex);
        this->texRef = readTexture(_pathRef);

        this->rotate = rotate;
        this->translate = translate;
        this->scale = scale;
    };

    void Object::loadModel(int _v, float* _p, float* _t, float* _n) {
        this->verCount = _v;
        this->vertices = _p;
        this->vtexture = _t;
        this->vnormals = _n;
    }

    void Object::setValue(GLuint value,GLuint &valueToSet) {
        valueToSet=value;
    };

    GLuint Object::readTexture(char* filename) {
        GLuint tex;
        glActiveTexture(GL_TEXTURE0);
        //Wczytanie do pamięci komputera
        std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
        unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
        //Wczytaj obrazek
        unsigned error = lodepng::decode(image, width, height, filename);

        //Import do pamięci karty graficznej
        glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
        glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
        //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return tex;
    }

}