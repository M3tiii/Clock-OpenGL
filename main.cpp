#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_GLCOREARB 1
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GLFW/glfw3.h>

//CLASSES
#include <Rack.h>
#include <Clock.h>
#include <Object.h>

//EXTRERNAL LIBS
//#include <lodepng.h>
#include <shaderprogram.h>
#include <loader.cpp>

//MODELS
#include "product/cube.c"
//#include "product/sRack.c"
#include "product/lRack.c"
#include "product/sTip.c"
#include "product/lTip.c"
#include "product/pendulum.c"
#include "product/clock.c"

using namespace std;
using namespace glm;
using namespace Objects;

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void error_callback(int error, const char* description);
void initOpenGLProgram(GLFWwindow* window);
void drawScene(GLFWwindow* window, float angle_x, float angle_y);
int createAllModels();
void drawObject(Objects::Object object, ShaderProgram *shaderProgram,mat4 mV, mat4 mP, float rotation_x,float rotation_y, vec3 rotacja, vec3 translacja, vec3 skalowanie);
Objects::Object *test;
// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
const float PI = 3.141592653589793f;
float speed_x = 0; // [radiany/s]
float speed_y = 0; // [radiany/s]
bool loadModels = true;
std::string source = "/Users/kon/GitHub/clock-project/";
vector<Object> World;

glm::mat4 P;
glm::mat4 V;
glm::mat4 M;

//Uchwyty na shadery
ShaderProgram *shaderProgram; //Wskaźnik na obiekt reprezentujący program cieniujący.


// ----------------------------------------------------------
// Function DEFINITION
// ----------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed_y = -3.14;
        if (key == GLFW_KEY_RIGHT) speed_y = 3.14;
        if (key == GLFW_KEY_UP) speed_x = -3.14;
        if (key == GLFW_KEY_DOWN) speed_x = 3.14;
    }


    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) speed_y = 0;
        if (key == GLFW_KEY_RIGHT) speed_y = 0;
        if (key == GLFW_KEY_UP) speed_x = 0;
        if (key == GLFW_KEY_DOWN) speed_x = 0;
    }
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram() {
    //TODO
//    delete shaderProgram; //Usunięcie programu cieniującego
//
//    glDeleteVertexArrays(1,&vao); //Usunięcie vao
//    glDeleteBuffers(1,&bufVertices); //Usunięcie VBO z wierzchołkami
//    glDeleteBuffers(1,&bufColors); //Usunięcie VBO z kolorami
//    glDeleteBuffers(1,&bufNormals); //Usunięcie VBO z wektorami normalnymi
//    glDeleteBuffers(1, &bufTexCoords); //Usunięcie VBO ze współrzędnymi teksturowania
}

//Tworzy bufor VBO z tablicy
GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
    GLuint handle;

    glGenBuffers(1,&handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierał tablicę danych
    glBindBuffer(GL_ARRAY_BUFFER,handle);  //Uaktywnij wygenerowany uchwyt VBO
    glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablicę do VBO

    return handle;
}

//Przypisuje bufor VBO do atrybutu
void assignVBOtoAttribute(ShaderProgram *shaderProgram,char* attributeName, GLuint bufVBO, int vertexSize) {
    GLuint location=shaderProgram->getAttribLocation(attributeName); //Pobierz numery slotów dla atrybutu
    glBindBuffer(GL_ARRAY_BUFFER,bufVBO);  //Uaktywnij uchwyt VBO
    glEnableVertexAttribArray(location); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej location
    glVertexAttribPointer(location,vertexSize,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location mają być brane z aktywnego VBO
}

void loadObjectVBO(Objects::Object *object) {
    //Zbuduj VBO z danymi obiektu do narysowania
    GLuint vb, vuv, vn;
    GLuint vao;
    vb = makeBuffer((*object).vertices, (*object).verCount, sizeof(glm::vec3)); //VBO ze wspó³rzêdnymi wierzcho³ków
    vuv = makeBuffer((*object).vtexture, (*object).verCount, sizeof(glm::vec2));//VBO z UV
    vn = makeBuffer((*object).vnormals, (*object).verCount, sizeof(glm::vec3));//VBO z wektorami normalnymi wierzcho³ków

    glGenVertexArrays(1, &vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej
    glBindVertexArray(vao); //Uaktywnij nowo utworzony VAO

    assignVBOtoAttribute(shaderProgram, "vertex", vb, 3); //"vertex" odnosi siê do deklaracji "in vec4 vertex;" w vertex shaderze
    assignVBOtoAttribute(shaderProgram, "vertexUV", vuv, 2); //"vertexUV" odnosi siê do deklaracji "in vec2 vertexUV;" w vertex shaderze
    assignVBOtoAttribute(shaderProgram, "normal", vn, 3); //"bufNormals" odnosi siê do deklaracji "in vec3 normal;" w vertex shaderze

    (*object).setValue(vao, (*object).objectVao);
    (*object).setValue(vb, (*object).vertexbuffer);
    (*object).setValue(vuv, (*object).vertexUV);
    (*object).setValue(vn, (*object).bufNormals);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {

    glClearColor(0, 0, 0, 1);
    glEnable(GL_LIGHT0); //Włącz domyslne światło
    glEnable(GL_DEPTH_TEST); //Włącz używanie Z-Bufora
//    glEnable(GL_LIGHTING); //Włącz tryb cieniowania
//    glEnable(GL_COLOR_MATERIAL); //glColor3d ma modyfikować własności meteriału

    glEnable(GL_TEXTURE_2D); //Włączenie textur, niepotrzebne w shaderach ? XXX

    glfwSetKeyCallback(window, key_callback);

    //WYKORZYSTANIE SHADERÓW XXX
//    shaderProgram=new ShaderProgram("/Users/kon/GitHub/clock-project/src/vshader.txt",NULL,"/Users/kon/GitHub/clock-project/src/fshader.txt"); //Wczytaj program cieniujący
//    loadObjectVBO(test);

}

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów

    P = glm::perspective(50 * PI / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

    V = glm::lookAt( //Wylicz macierz widoku
            glm::vec3(0.0f, 1.0f, -0.2f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

    //Wylicz macierz modelu rysowanego obiektu
    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle_x, glm::vec3(1, 0, 0));
    M = glm::rotate(M, angle_y, glm::vec3(0, 1, 0));

    glLoadMatrixf(glm::value_ptr(V*M)); //Załaduj wyliczoną macierz do OpenGL

    for (int i = 0; i < World.size(); i++) {
        drawObject(World[i], shaderProgram, V, P, 0, 0, World[i].rotate, World[i].translate, World[i].scale);
    }

    glDisableVertexAttribArray(0);
    glfwSwapBuffers(window);
}

void drawObject(Objects::Object object, ShaderProgram *shaderProgram,mat4 modelV, mat4 modelP, float rotation_x,float rotation_y, vec3 mrotation, vec3 mtranslate, vec3 mscale) {

    //WYKORZYSTANIE SHADERÓW XXX
//    shaderProgram->use();
//
//    glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(modelP));
//    glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(modelV));
//    glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(modelMatrix));
//    glUniform4f(shaderProgram->getUniformLocation("lightPos0"), 0, 2, 0, 1); //Przekazanie wspó³rzêdnych Ÿród³a œwiat³a do zmiennej jednorodnej lightPos0
//
//    glUniform1i(shaderProgram->getUniformLocation("WRITE SOURCE TO TEXTURE"), 0); //SET LOCATION TO TEXTURE
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, object.texModel);
//
//    glBindVertexArray(object.objectVao);
//
//    glDrawArrays(GL_TRIANGLES, 0, object.verCount);
//
//    glBindVertexArray(0);
    //KONIEC


    //WERSJA BEZ SHADERÓW XXX

    glm::mat4 modelMatrix;
    modelMatrix = translate(modelMatrix, object.translate); //Pozycja
    modelMatrix = scale(modelMatrix, object.scale);
    modelMatrix = rotate(modelMatrix, object.rotate.x, vec3(1, 0, 0));
    modelMatrix = rotate(modelMatrix, object.rotate.y, vec3(0, 1, 0));
    modelMatrix = rotate(modelMatrix, object.rotate.z, vec3(0, 0, 1));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(value_ptr(V*modelMatrix));

    glBindTexture(GL_TEXTURE_2D, object.texModel);

    glEnableClientState(GL_VERTEX_ARRAY); //W³¹cz u¿ywanie tablicy wierzcho³ków
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); //Włącz używanie tablicy współrzędnych teksturowania
    glVertexPointer(3, GL_FLOAT, 0, object.vertices); //Wska¿ tablicê wierzcho³ków
    glTexCoordPointer(2, GL_FLOAT, 0, object.vtexture); //Zdefiniuj tablicę, która jest źródłem współrzędnych teksturowania

    glDrawArrays(GL_TRIANGLES, 0, object.verCount);

    glDisableClientState(GL_VERTEX_ARRAY); //Wy³¹cz u¿ywanie tablicy wierzcho³ków
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); //Wyłącz używanie tablicy współrzędnych teksturowania
    //KONIEC
}

int createAllModels() {
    World.push_back( Object(3, "sTip", "/Users/kon/GitHub/clock-project/source/metal.png",
                            "/Users/kon/GitHub/clock-project/source/metal.png", vec3(0,0,0), vec3(0,0,0), vec3(1,1,1)) );
//    World.push_back( Object(4, "lTip", "/Users/kon/GitHub/clock-project/source/metal.png",
//                            "/Users/kon/GitHub/clock-project/source/metal.png", vec3(0,0,0), vec3(0,0,0), vec3(1,1,1)) );

    if(loadModels) {
//        createModel("/Users/kon/GitHub/clock-project/", "cube");
//        createModel("/Users/kon/GitHub/clock-project/", "clock");
//        createModel("/Users/kon/GitHub/clock-project/", "pendulum");
//        createModel("/Users/kon/GitHub/clock-project/", "sTip");
//        createModel("/Users/kon/GitHub/clock-project/", "lTip");
//        createModel("/Users/kon/GitHub/clock-project/", "sRack");
//        createModel("/Users/kon/GitHub/clock-project/", "lRack");
    }
    for (int i = 0; i < World.size(); i++) {
        switch (World[i].type) {
            case 0:
                World[i].loadModel(cubeVertices, cubePositions, cubeTexels, cubeNormals);
                break;
            case 1:
                World[i].loadModel(clockVertices, clockPositions, clockTexels, clockNormals);
                break;
            case 2:
                World[i].loadModel(pendulumVertices, pendulumPositions, pendulumTexels, pendulumNormals);
                break;
            case 3:
                World[i].loadModel(sTipVertices, sTipPositions, sTipTexels, sTipNormals);
                break;
            case 4:
                World[i].loadModel(lTipVertices, lTipPositions, lTipTexels, lTipNormals);
                break;
            case 5:
//                World[i].loadModel(sRackVertices, sRackPositions, sRackTexels, sRackNormals);
                break;
            case 6:
                World[i].loadModel(lRackVertices, lRackPositions, lRackTexels, lRackNormals);
                break;
        }
    }

}


int main(int argc, char *argv[]) {

    GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

    glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

    if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);  //Utwórz okno 800x800 o tytule "OpenGL" i kontekst OpenGL.

    if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
    {
        fprintf(stderr, "Nie można utworzyć okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
    glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
        fprintf(stderr, "Nie można zainicjować GLEW.\n");
        exit(EXIT_FAILURE);
    }

    createAllModels();

    initOpenGLProgram(window); //Operacje inicjujące

    float angle_x = 0; //Kąt obrotu obiektu
    float angle_y = 0; //Kąt obrotu obiektu

    glfwSetTime(0); //Wyzeruj licznik czasu

    //Główna pętla
    while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
    {
        angle_x += speed_x*glfwGetTime(); //Zwiększ kąt o prędkość kątową razy czas jaki upłynął od poprzedniej klatki
        angle_y += speed_y*glfwGetTime(); //Zwiększ kąt o prędkość kątową razy czas jaki upłynął od poprzedniej klatki
        glfwSetTime(0); //Wyzeruj licznik czasu
        drawScene(window,angle_x,angle_y); //Wykonaj procedurę rysującą
        glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
    }

    freeOpenGLProgram();

    glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
    glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
    exit(EXIT_SUCCESS);
}


