#include "Player.h"

Player::Player() {
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

void Player::update(float velocity) {
	// Animate fish swimming based on velocity
	if (velocity > 0) {
		meshObj[2].setPosition(glm::vec3(0, 5, 0));

		/*
		// Head part
		meshObj[1].setRotationMatrix(sin(ofGetElapsedTimef() * 3), glm::vec3(0, 0, 1));

		meshObj[0].setRotationMatrix(-sin(ofGetElapsedTimef() * 3), glm::vec3(0, 0, 1));

		// Tail part
		meshObj[3].setRotationMatrix(-sin(ofGetElapsedTimef() * 3), glm::vec3(0, 0, 1));

		meshObj[4].setRotationMatrix(sin(ofGetElapsedTimef() * 3), glm::vec3(0, 0, 1));


		// Set child matrices
		meshObj[1].setMatrix(meshObj[2].getMatrix() * meshObj[1].getRotationMatrix());
		meshObj[3].setMatrix(meshObj[2].getMatrix() * meshObj[3].getRotationMatrix());

		meshObj[0].setMatrix(meshObj[1].getMatrix() * meshObj[0].getRotationMatrix());
		meshObj[4].setMatrix(meshObj[3].getMatrix() * meshObj[4].getRotationMatrix());
		*/

		float t = ofGetElapsedTimef() * 4.0;
		float segmentLength = 1.0; // Distance between segments

		// Define rotations with increasing amplitude toward the tail
		float angle0 = sin(t) * 0.1;        // Head
		float angle1 = sin(t) * 0.4;        // Upper Neck
		float angle3 = cos(t + 2) * 0.4;  // Lower Body
		float angle4 = cos(t + 2) * 0.2;  // Tail Tip

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
}

void Player::draw(ofShader* myShader) {

	ofSetColor(1.0);
	for (int i = 0; i < 5; i++) {
		myShader->setUniformMatrix4f("worldMatrix", meshObj[i].getMatrix());
		myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
		myShader->setUniform1i("texBool", 0);
		meshObj[i].draw();
	}
}
