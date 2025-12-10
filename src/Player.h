#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "MeshObject.h"

class Player {

	public:
		Player();

		void update(float velocity);
		void draw(ofShader* myShader);
		void draw(ofShader* myShader, ofTexture* fishTexture);
		void setColor(glm::vec3 rgb);

		void setPosition(const glm::vec3& pos);
		void setOrientation(const glm::quat& ori);



	protected:
		glm::vec3 targetPos;
		glm::vec3 cameraPos;

		// Load 5 segments of player model
		ofxAssimpModelLoader model[5];

		// Create MeshObjects for each segment
		MeshObject meshObj[5];

		float animationTime;
		float currentAnimationSpeed;
		float baseSpeed;
		glm::vec3 color;

};
