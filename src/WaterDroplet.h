#pragma once
#include "ofMain.h"
#include "MeshObject Primitives/Sphere.h"

class WaterDroplet {

	public:
		WaterDroplet(float r, glm::vec3 pos, glm::vec3 col);
		bool isColliding(glm::vec3 playerPos);
		void draw(ofShader* myShader);
		glm::vec3 getPosition(void) { return mesh.getPosition(); }
		glm::vec3 getColor(void) { return color; }
		float getRadius(void) { return radius; }
	
	protected:
		Sphere mesh;
		float radius;
		ofTexture texture;
		glm::vec3 color;

};
