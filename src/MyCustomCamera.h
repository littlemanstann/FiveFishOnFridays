#pragma once
#include "ofCamera.h"

class MyCustomCamera : public ofCamera {
public:
    MyCustomCamera();

    void update(float deltaTime, float size);

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

    glm::vec3 move;
    float size;
	bool canMoveFlag = true;
};
