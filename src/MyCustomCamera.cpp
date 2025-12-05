#include "MyCustomCamera.h"
#include "ofMain.h"
#include "ofApp.h"
#include <glm/gtc/quaternion.hpp>



MyCustomCamera::MyCustomCamera() {
    movementSpeed = 0.5f; // units per second
    rotationSpeed = 0.3f;   // rads per second
    position = glm::vec3(0, 0, 0);
	targetPosition = glm::vec3(0, 0, 0);
    orientation = glm::quat(1, 0, 0, 0); // nothing
	cameraOffset = glm::vec3(0, 2.743, 7.135);
    BASE_SIDE = glm::vec3(1, 0, 0);
    BASE_UP = glm::vec3(0, 1, 0);
	BASE_DOWN = glm::vec3(0, -1, 0);
    BASE_FORWARD = glm::vec3(0, 0, -1); // don't need to store all three, note
    move = glm::vec3(0, 0, 0);

	setNearClip(0.1f);
	setFarClip(1000.0f);
	setPosition(0, 0, 10);
	lookAt(glm::vec3(0, 0, 0));

	myCone.set(1, 3);

	myPlayer.load("models/FISH.glb");
	myPlayer.enableNormals();
	float scale = 0.02;
	myPlayer.setScale(scale, -scale, -scale); //Player Model Scale
	
}

void MyCustomCamera::update(float deltaTime) {

	if (isTalking) {

		return;
	}
    if (!canMoveFlag) {
        return;
    }

	// GRAVITY CHECK:
	if (applyGravity) {
		gravityVel.y += GRAVITY * deltaTime;
	}

	//Look at VFX Code
    if (ofGetKeyPressed('1')) { //Move camera to look at fireworks
        move = glm::vec3(0, 0, 0);
        position = glm::vec3(0, -5, 15);
        betterLookAt(glm::vec3(1, 0, 0));
    }
    if (ofGetKeyPressed('2')) { //Move camera to look at bubble stream
        move = glm::vec3(0, 0, 0);
        position = glm::vec3(100, 100, -205);
        betterLookAt(glm::vec3(100, 100, -200));
    }



	//Dash Code, when moving you can dash
	if (ofGetKeyPressed('r') && dash == false) {
		dash = true;
		timeDashBegin = ofGetElapsedTimef();
	}

	

	//Variable Smoothing Code: https://www.desmos.com/calculator/98wozkt1jb
	if (dash) {
		speedModifier = (DASH_MULT - MOVE_MULT) / (1 + exp(-(DASH_MOVE_SMOOTHING * abs(ofGetElapsedTimef() - timeDashBegin) - 5))) + MOVE_MULT;
	} else {
		speedModifier = (MOVE_MULT - DASH_MULT) / (1 + exp(-(DASH_MOVE_SMOOTHING * abs(ofGetElapsedTimef() - timeDashBegin) - 5))) + DASH_MULT;
	}

	//Auto DOWNright Code, Uses basis matrix to determine how much to rotate fish to swim DOWN straight ?? idk why but down
	//We are using BASE_DOWN in OFX but in Godot we used BASE_UP
	float dotProduct = dot(BASE_DOWN, getqUp());
	float radAngle = acos(dotProduct / (BASE_DOWN.length() * getqUp().length()));
	float rightDotProduct = dot(BASE_DOWN, getqSide());
	roll(radAngle * 3.2 * rightDotProduct * deltaTime);
	

	//Movement Input Code
	float input = 0.0;
	if (ofGetKeyPressed('w')) input += 1;
	if (ofGetKeyPressed('s')) input -= 1;

	if (abs(input) > 0) {
		//Speed Velocity Smoothing Curve: https://www.desmos.com/calculator/bpjcnqq
		float baseline = 30;
		float topline = 100 - baseline;
		float accelerationGraph = (topline - baseline) / (1 + exp(-0.14 * abs(speed) - 5)) + baseline;
		speed = fminf(input * accelerationGraph * deltaTime + speed, MAX_SPEED); //does not concern with going backwards...
	} else {
		//Stop dashing when you stop pressing a movement key
		dash = false;

		//Slowing Curve
		float baseline = 100;
		float curveNearZero = baseline / (1 - exp(-0.127 * abs(speed)));
		float curveNearMaxSpeed = baseline + 0.5 * exp(0.047 * abs(speed));
		float curveCombination = fmaxf(curveNearZero, curveNearMaxSpeed);
		speed = glm::sign(speed) * fmaxf(abs(speed) - curveCombination * deltaTime, 0);
	}

	position += getqForward() * speed * speedModifier * deltaTime;
	// GRAVITY: Apply gravity velocity to position
	position += gravityVel * deltaTime;

    // need to set ofNode parameters using internal position, orientation
	myCone.setPosition(position);
    myCone.setOrientation(orientation);

	//1. GLM Quirk, normalizing (0,0,0) does not return (0,0,0) it return NaN and gets upset :(
	//
	//2. GLM Quirk, sometimes "position - glm::vec3(0,0,0)).length()" always gives zero so you
	//have to manually find the distance using pythagorean theorum :(


	//myCone Position Following Code
	float d = sqrt(pow((position.x - targetPosition.x),2) + pow((position.y - targetPosition.y),2) + pow((position.z - targetPosition.z), 2));
	if (d < 0.001) {
		//printf("Not a number!");
	} else {
		float targetSpeed = pow(d, 2);
		targetPosition += glm::normalize((position - targetPosition)) * targetSpeed * deltaTime;
	}

	//Camera Offset From Position Follower
	pitch(xRotation * DEG_TO_RAD); //rotate to camera rotation offset
	setPosition(targetPosition);
	setPosition(targetPosition + orientation * cameraOffset);
	setOrientation(orientation);
	pitch(-xRotation * DEG_TO_RAD); //rotate back
}

void MyCustomCamera::drawMe() {

	//3. GLM Quirk, assimpModelLoader doesn't have setOrientation
	// we must push a matrix of an ofNodePrimitive to rotate the fish model

	ofPushMatrix();
	ofMultMatrix(myCone.getGlobalTransformMatrix());
	myPlayer.drawFaces();
	ofPopMatrix();

}

void MyCustomCamera::betterLookAt(const glm::vec3 target)
{
    glm::vec3 targetDir = glm::normalize(target - position);
    glm::vec3 currentDir = glm::normalize(orientation * glm::vec3(0, 0, -1));
    float dot = glm::dot(currentDir, targetDir);
    if (dot > 0.9999f) return;
    glm::vec3 axis = (dot < -0.9999f) ? glm::vec3(0, 1, 0) : glm::normalize(glm::cross(currentDir, targetDir));
    float angle = (dot < -0.9999f) ? glm::radians(180.0f) : acos(glm::clamp(dot, -1.0f, 1.0f));
    orientation = glm::angleAxis(angle, axis) * orientation;
}

void MyCustomCamera::camRotate(glm::vec2 mouseInput) {
	if (!canMoveFlag) {
		return;
	}
	//Gets called when mouse moves across the screen in the MouseMove ofApp function.
	glm::vec2 rotationamt = -mouseInput * mouseSensitivity;
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
