#include "Point.h"

Point::Point() {
    color = ofColor(255); // default white
}



void Point::setup(float radius, ofColor col) {
    sphere.setRadius(radius);
    color = col;
}


void Point::draw(ofShader* myShader) {
    //ofSetColor(color);
   // sphere.drawWireframe();   // or sphere.draw() for solid
    //ofSetColor(0, 0, 255);
	printf("i am called \n");
}

void Point::setPosition(float x, float y, float z) {
    sphere.setPosition(x, y, z);
    position = glm::vec3(x, y, z);
}

void Point::setColor(ofColor col) {
    color = col;
}
