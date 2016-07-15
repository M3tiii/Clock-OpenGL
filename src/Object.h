
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <loader.cpp>

using namespace std;

struct coor {
    int x;
    int y;
    int z;
};

struct pos {
    int vertices;
    float * positions;
    float * texels;
    float * normals;
};

class Object {
    int id;
    coor coordinates;
    pos actual;
    string path;
    string name;

    bool loaded = false;
public:
    Object (int, int, int, int, string, string);
    void draw();
    string loadModel();
    void setModel(pos tmp);
    void setCoordinates(int _x, int _y, int _z);
    coor getCoordinates();
    bool isLoaded();
};
Object::Object (int _id, int _x, int _y, int _z, string _path, string _name){
    id = _id;
    setCoordinates(_x, _y, _z);
    path = _path;
    name = _name;
};
string Object::loadModel() {
    cout << "Loading model of " << name << " ..." << endl;
    createModel(path, name);
    cout << "Loader complited." << endl;
    return name;
};
void Object::setModel(pos tmp) {
    actual = tmp;
    loaded = true;
    cout << "Set model of " << name << endl;
};
void Object::setCoordinates(int _x, int _y, int _z) {
    coordinates.x = _x;
    coordinates.y = _y;
    coordinates.z = _z;
};
coor Object::getCoordinates() {
    return coordinates;
};
bool Object::isLoaded() {
    return loaded;
};

#endif