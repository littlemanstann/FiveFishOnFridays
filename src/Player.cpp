#include "Player.h"

Player::Player() {
	animationTime = 0.0f;
	currentAnimationSpeed = 0.0f;
	baseSpeed = 4.0f;
	
	for (int i = 0; i < 5; i++) {
		model[i].load("models/Fish_Segmented.fbx");
		model[i].enableNormals();
		model[i].setScale(0.03, -0.03, -0.03);//this is while trying to approximate scale to current movement
		
		// Get mesh from model and create MeshObject
		meshObj[i] = MeshObject(model[i].getMesh(i));
	}

	//setPosition(glm::vec3(0, 20, 0));
}

void Player::setPosition(const glm::vec3& pos) {
	for (int i = 0; i < 5; i++) {
		meshObj[i].setPosition(pos);
	}
}

void Player::setOrientation(const glm::quat& ori) {
	meshObj[2].setRotation(ori);
	// Flip rotation to face forward
	meshObj[2].setRotation(90, glm::vec3(-1, 0, 0));
}

void Player::update(float velocity) {
	float speedMultiplier = (abs(velocity) / 40.0f);
	// Add baseline when moving
	if (abs(velocity) > 0.1f) {
		speedMultiplier += 0.5f;
	}
	speedMultiplier = ofClamp(speedMultiplier, 0.1f, 10.0f);
	float targetAnimationSpeed = baseSpeed * speedMultiplier;
	// Smoothly interpolate current animation speed toward target
	currentAnimationSpeed = glm::mix(currentAnimationSpeed, targetAnimationSpeed, 0.05f);
	
	// Animate fish swimming based on velocity
	animationTime += ofGetLastFrameTime() * currentAnimationSpeed;
	float t = animationTime;

	float segmentLength = 1.0; // Distance between segments

	// Define rotations with increasing amplitude toward the tail
	float amplitude = glm::clamp(speedMultiplier, 0.5f, 1.5f);
	float angle0 = sin(t) * 0.1 * amplitude;        // Head
	float angle1 = sin(t) * 0.3 * amplitude;        // Upper Neck
	float angle3 = cos(t + 2) * 0.4 * amplitude;	// Lower Body
	float angle4 = cos(t + 2) * 0.2 * amplitude;    // Tail Tip

	// HEAD SIDE (meshObj[1] and meshObj[0])
	// meshObj[1] - first segment from center toward head
	glm::mat4 rot1 = glm::rotate(angle1, glm::vec3(0, 0, 1));
	meshObj[1].setMatrix(meshObj[2].getMatrix() * rot1);

	// meshObj[0] - head tip
	meshObj[0].setOrbitMatrix(angle0, glm::vec3(0, 0, 1), glm::vec3(0, -segmentLength, 0));
	glm::mat4 rot0 = glm::rotate(angle0, glm::vec3(0, 0, 1));
	meshObj[0].setMatrix(meshObj[1].getMatrix() * meshObj->getRotationMatrix() * rot0);

	// TAIL SIDE (meshObj[3] and meshObj[4])
	// meshObj[3] - first segment from center toward tail
	glm::mat4 rot3 = glm::rotate(angle3, glm::vec3(0, 0, 1));
	meshObj[3].setMatrix(meshObj[2].getMatrix() * rot3);

	// meshObj[4] - tail tip
	glm::mat4 rot4 = glm::rotate(angle4, glm::vec3(0, 0, 1));
	meshObj[4].setMatrix(meshObj[3].getMatrix() * rot4);

	ofMatrix4x4 m = meshObj[4].getMatrix();
}

void Player::draw(ofShader* myShader) {
	for (int i = 0; i < 5; i++) {
		myShader->setUniformMatrix4f("worldMatrix", meshObj[i].getMatrix());
		myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
		myShader->setUniform1i("texBool", 0);
		meshObj[i].draw();
	}
}

void Player::draw(ofShader* myShader, ofTexture* fishTexture) {
	for (int i = 0; i < 5; i++) {
		myShader->setUniformMatrix4f("worldMatrix", meshObj[i].getMatrix());
		myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));

		// Bind texture and set uniform
		if (fishTexture != nullptr) {
			fishTexture->bind();
			myShader->setUniformTexture("tex", *fishTexture, 0);
			myShader->setUniform1i("texBool", 1); // Enable texture in shader
		}
		else {
			myShader->setUniform1i("texBool", 0); // No texture
		}

		meshObj[i].draw();

		// Unbind texture
		if (fishTexture != nullptr) {
			fishTexture->unbind();
		}
	}
}
