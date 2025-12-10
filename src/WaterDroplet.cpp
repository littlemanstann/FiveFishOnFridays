#include "WaterDroplet.h"

WaterDroplet::WaterDroplet(float r, glm::vec3 pos, glm::vec3 col) : radius(r), color(col) {
	mesh = Sphere(radius, 30); // Create sphere mesh with given radius and resolution
	mesh.setPosition(pos); // Set initial position


	// TEMPORARY BLUE TEXTURE
	mesh.setColor(ofColor(0, 150, 255, 128)); // Semi-transparent blue
}

bool WaterDroplet::isColliding(glm::vec3 playerPos) {
	float distance = glm::distance(playerPos, glm::vec3(mesh.getMatrix()[3]));
	return distance < radius;
}

void WaterDroplet::draw(ofShader * myShader) {
	myShader->setUniformMatrix4f("worldMatrix", mesh.getMatrix());
	myShader->setUniform3f("objectColor", color);
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);
	mesh.draw();
}
