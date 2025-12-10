#pragma once
#include "ofCamera.h"
#include "of3dPrimitives.h"
#include "ofxAssimpModelLoader.h"
#include "Player.h"

class MyCustomCamera : public ofCamera {
public:
    MyCustomCamera();

    void update(float deltaTime);
	void drawMeShaded();
	void camRotate(glm::vec2 mouseInput);

	glm::mat4 getMyGlobalTransformMatrix();
	glm::vec3 getPlayerPosition() { return myCone.getPosition(); }
	void setPlayerPosition(glm::vec3 pos) { position = pos; targetPosition = pos; }
	float getSpeed() const { return glm::length(move); }

    glm::vec3 getqForward();
    glm::vec3 getqSide();
    glm::vec3 getqUp();

    void pitch(float a);
    void roll(float a);
    void yaw(float a);

	void addAcceleration(float force);
    void canMove(bool move) { canMoveFlag = move; }
	void betterLookAt(glm::vec3 target);

	ofConePrimitive myCone;
	float interactRange = 5.0f;
	bool dash = false;
	bool isTalking = false;
	// Enable/Disable gravity effect on camera
	void setGravity(bool gravity);

	//Game Over Screen
	bool isDead = false;
	

private:
	Player myPlayer;
	ofTexture texture;

    glm::vec3 BASE_UP;
    glm::vec3 BASE_SIDE;
    glm::vec3 BASE_FORWARD;
	glm::vec3 BASE_DOWN;

    glm::vec3 move;
	glm::vec3 position;
	glm::quat orientation;

	glm::vec3 targetPosition;
	glm::vec3 cameraOffset;
	
	float speed = 0;
	float speedModifier = 0;
	float timeDashBegin = 0.0;
	float mouseSensitivity = 0.003;

	const float MAX_SPEED = 100;//reccomended to not change max speed
	const float DASH_MULT = 1.0;//and change dash/movemult
	const float MOVE_MULT = 0.3;//as these don't affect graphs
	const float DASH_MOVE_SMOOTHING = 19.6;
	const float xRotation = -15;
	
	bool canMoveFlag = true;

	// GRAVITY CODE:
	glm::vec3 gravityVel;
	float GRAVITY = -9.8f;
	bool applyGravity = false;
	
	

};
