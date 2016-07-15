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

#include <Rack.h>
#include <Clock.h>
#include "product/cube.c"

using namespace std;

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void error_callback(int error, const char* description);
void initOpenGLProgram(GLFWwindow* window);
void drawScene(GLFWwindow* window, float angle);
int createAllModels();

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
const float PI = 3.141592653589793f;
float speed = 0;
bool loadModels = true;
std::string source = "/Users/kon/GitHub/clock-project/";
vector<Object> Objects;

// ----------------------------------------------------------
// Function DEFINITION
// ----------------------------------------------------------
void key_callback(GLFWwindow* window, int key,
                  int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed = -3.14;
        if (key == GLFW_KEY_RIGHT) speed = 3.14;
    }

    if (action == GLFW_RELEASE) {
        speed = 0;
    }
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
    glClearColor(0, 0, 0, 1); //Czyœæ ekran na czarno
    //glEnable(GL_LIGHTING); //W³¹cz tryb cieniowania
    glEnable(GL_LIGHT0); //W³¹cz domyslne œwiat³o
    glEnable(GL_DEPTH_TEST); //W³¹cz u¿ywanie Z-Bufora
    glEnable(GL_COLOR_MATERIAL); //glColor3d ma modyfikowaæ w³asnoœci materia³u

    glfwSetKeyCallback(window, key_callback);

}

//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle) {

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów


    glm::mat4 V = glm::lookAt( //Wylicz macierz widoku
            glm::vec3(0.0f, 0.0f, -2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 P = glm::perspective(50 * PI / 90, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

    //Za³aduj macierz rzutowania do OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(P));

    //PrzejdŸ w tryb pracy z macierz¹ Model-Widok
    glMatrixMode(GL_MODELVIEW);

    //Wylicz macierz obrotu o k¹t angle wokó³ osi (0,1,1)
    glm::mat4 M = glm::mat4(1.0f);
    M = glm::rotate(M, angle, glm::vec3(0, 1, 1));
    glLoadMatrixf(glm::value_ptr(V*M)); //Za³aduj wyliczon¹ macierz do OpenGL


    //Narysuj model
    glEnableClientState(GL_VERTEX_ARRAY); //W³¹cz u¿ywanie tablicy wierzcho³ków
//    glEnableClientState(GL_COLOR_ARRAY); //W³¹cz u¿ywanie tablicy kolorów

    glVertexPointer(3, GL_FLOAT, 0, cubePositions); //Wska¿ tablicê wierzcho³ków
//    glColorPointer(3, GL_FLOAT, 0, myColors); //Wska¿ tablicê kolorów

    glDrawArrays(GL_TRIANGLES, 0, cubeVertices); //Wykonaj rysowanie

    glDisableClientState(GL_VERTEX_ARRAY); //Wy³¹cz u¿ywanie tablicy wierzcho³ków
    glDisableClientState(GL_COLOR_ARRAY); //Wy³¹cz u¿ywanie tablicy kolorów
//    glBufferData(GL_ARRAY_BUFFER, cubeVertices * sizeof(glm::vec3), &cubePositions[0], GL_STATIC_DRAW);

    glfwSwapBuffers(window);

}


int createAllModels() {
    int count = 10;
    Objects.reserve(count);
    Objects.push_back( Object(0,1,1,1,source,"cube") );

    if(loadModels) {
        Objects[0].loadModel();
    }

    pos tmp = {
            cubeVertices,
            cubePositions,
            cubeTexels,
            cubeNormals
        };
    Objects[0].setModel(tmp);
};


int main(int argc, char *argv[]) {

    createAllModels();

    GLFWwindow* window; //WskaŸnik na obiekt reprezentuj¹cy okno

    glfwSetErrorCallback(error_callback);//Zarejestruj procedurê obs³ugi b³êdów

    if (!glfwInit()) { //Zainicjuj bibliotekê GLFW
        fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

    if (!window) //Je¿eli okna nie uda³o siê utworzyæ, to zamknij program
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
    glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

    if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekê GLEW
        fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
        exit(EXIT_FAILURE);
    }
    glewInit();//
    initOpenGLProgram(window); //Operacje inicjuj¹ce

    float angle = 0; //K¹t obrotu torusa
    glfwSetTime(0); //Wyzeruj licznik czasu

    while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
    {
        angle += speed*glfwGetTime(); //Zwiêksz k¹t o prêdkoœæ k¹tow¹ razy czas jaki up³yn¹³ od poprzedniej klatki
        glfwSetTime(0); //Wyzeruj licznik czasu
        drawScene(window,angle); //Wykonaj procedurê rysuj¹c¹
        glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
    }

    glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
    glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
    exit(EXIT_SUCCESS);
}


