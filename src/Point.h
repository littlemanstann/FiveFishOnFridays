#pragma once
#include "ofMain.h"

class Point {
public:
    Point();
    void setup(float radius, ofColor col);
    virtual void update(float deltatime) = 0;
    virtual void draw(ofShader* myShader, glm::vec3 pos);
    virtual ~Point() = default;

    void setPosition(float x, float y, float z);
    void setColor(ofColor col);


    glm::vec3 position;


    bool operator==(const Point& other) const {
        return position == other.position;
    }
    ofColor color;


private:
    ofSpherePrimitive sphere;


};
