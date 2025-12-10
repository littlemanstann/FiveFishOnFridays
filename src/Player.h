#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "MeshObject.h"

class Player {

	public:
		Player();

		void update(float velocity);
		void draw();

		void setPosition(const glm::vec3& pos);



	protected:
		glm::vec3 targetPos;
		glm::vec3 cameraPos;

		// Load 5 segments of player model
		ofxAssimpModelLoader model[5];

		// Create MeshObjects for each segment
		MeshObject meshObj[5];


};
