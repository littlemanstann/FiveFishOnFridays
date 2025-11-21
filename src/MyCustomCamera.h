#pragma once
#include "ofCamera.h"
#include "of3dPrimitives.h"
#include "ofxAssimpModelLoader.h"
class MyCustomCamera : public ofCamera {
public:
    MyCustomCamera();

    void update(float deltaTime, float size);
	void drawMe();
	void camRotate(glm::vec2 mouseInput);


	glm::vec3 getPlayerPosition() { return myCone.getPosition(); }
	

    glm::vec3 getqForward();
    glm::vec3 getqSide();
    glm::vec3 getqUp();


    void pitch(float a);
    void roll(float a);
    void yaw(float a);

	void addAcceleration(float force);
	float getSpeed() const { return glm::length(move); }

    // Dictates if the player can move or not
    void canMove(bool move) { canMoveFlag = move; }

	private:
    float movementSpeed;
    float rotationSpeed;
    glm::vec3 position;
    glm::quat orientation;

    glm::vec3 BASE_UP;
    glm::vec3 BASE_SIDE;
    glm::vec3 BASE_FORWARD;
	glm::vec3 BASE_DOWN;

    glm::vec3 move;
    float size;
	bool canMoveFlag = true;

    void betterLookAt(glm::vec3 target);


	float MAX_SPEED = 100;//reccomended to not change max speed
	float DASH_MULT = 1.0;//and change dash/movemult
	float MOVE_MULT = 0.3;//as these don't affect graphs
	float DASH_MOVE_SMOOTHING = 19.6;
	float speed = 0;
	float speedModifier = 0;
	float mouseSensitivity = 0.003;
	bool dash = false;
	float timeDashBegin = 0.0;

	glm::vec3 targetPosition;
	glm::vec3 cameraOffset;
	float xRotation = -15;

	ofConePrimitive myCone;
	ofxAssimpModelLoader myPlayer;
	
};
