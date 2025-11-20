#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class Player {

	public:
		Player();
		void draw();

	protected:
		float radius;
		float height;

		glm::vec3 targetPos;
		glm::vec3 cameraPos;


		ofxAssimpModelLoader model;


};
