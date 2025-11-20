#include "MyCustomCamera.h"
#include "ofMain.h"
#include "ofApp.h"
#include <glm/gtc/quaternion.hpp>



MyCustomCamera::MyCustomCamera() {
    movementSpeed = 0.5f; // units per second
    rotationSpeed = 0.3f;   // rads per second
    position = glm::vec3(0, 0, 0);
    orientation = glm::quat(1, 0, 0, 0); // nothing

    BASE_SIDE = glm::vec3(1, 0, 0);
    BASE_UP = glm::vec3(0, 1, 0);
    BASE_FORWARD = glm::vec3(0, 0, -1); // don't need to store all three, note
    move = glm::vec3(0, 0, 0);
    size = 0;
}

void MyCustomCamera::update(float deltaTime, float size) {
    if (!canMoveFlag) {
        return;
    }

    // W accel movement
    if (ofGetKeyPressed('w')) move += getqForward() * deltaTime * size * movementSpeed;
	//check to see if the movement is going to be directly backwards.
    
    float test = glm::dot(glm::normalize(move), -getqForward());
    //If so then stop the ship from moving and also stop it from listening to the deaccel input.
    if (fabs(test - 1.0f) < 0.005f) {
        move = glm::vec3(0, 0, 0);
    }
    else {
		//Otherwise let it deaccelerate normally.
        if (ofGetKeyPressed('s')) move -= getqForward() * deltaTime;
    }

    if (glm::length(move) > 0.0f)  // did it move at all?
    {
        //add drag. ik its in space but i need this so that when you change orientation it will removes any horizontal velocity.
        move *= 0.99;
        position = position + move;
    }

    

    
    if (ofGetKeyPressed('1')) {
        size += deltaTime*5;
    }


    float rotationamt = rotationSpeed * deltaTime;

    // IJKL rotation
    if (ofGetKeyPressed('i')) pitch(rotationamt);
    if (ofGetKeyPressed('k')) pitch(-rotationamt);
    if (ofGetKeyPressed('j')) yaw(rotationamt);
    if (ofGetKeyPressed('l')) yaw(-rotationamt);
    if (ofGetKeyPressed('u')) roll(rotationamt);
    if (ofGetKeyPressed('o')) roll(-rotationamt);
    orientation = orientation / length(orientation); // normalize

    // need to set ofCamera parameters using internal position, orientation
    setPosition(position);
    setOrientation(orientation);
    
        


    
}

void MyCustomCamera::camRotate(glm::vec2 mouseInput) {
	float mouseSensitivity = 0.003;
	glm::vec2 rotationamt = mouseInput * mouseSensitivity;
	pitch(rotationamt.y);
	yaw(rotationamt.x);

}

// TODO: getqForward, getqSide, getqUp;
// TODO: pitch, yaw, roll

void MyCustomCamera::pitch(float amt) {
    glm::quat change = glm::angleAxis(amt, glm::vec3(1, 0, 0));
	orientation = orientation * change;

}


void MyCustomCamera::yaw(float amt) {
    glm::vec3 silly = glm::vec3(0, 1, 0);
    glm::quat change = glm::angleAxis(amt, silly);
    orientation = orientation * change;

}


void MyCustomCamera::roll(float amt) {
    glm::vec3 silly = glm::vec3(0, 0, 1);
    glm::quat change = glm::angleAxis(amt, silly);
	orientation = orientation * change;
}

glm::vec3 MyCustomCamera::getqForward() {


    return orientation * BASE_FORWARD; // because we look down -z axis
}


glm::vec3 MyCustomCamera::getqSide() {

    return orientation * BASE_SIDE;
}


glm::vec3 MyCustomCamera::getqUp() {

    return orientation * BASE_UP;
}

void MyCustomCamera::addAcceleration(float force) {
	move += getqForward() * force;
}
