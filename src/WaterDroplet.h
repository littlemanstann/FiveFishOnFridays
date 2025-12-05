#pragma once
#include "ofMain.h"
#include "MeshObject Primitives/Sphere.h"

class WaterDroplet {

	public:
		WaterDroplet(float r, glm::vec3 pos);
		bool isColliding(glm::vec3 playerPos);
		void draw();
	
	protected:
		Sphere mesh;
		float radius;
		ofTexture texture;

};