#include "WaterDroplet.h"

WaterDroplet::WaterDroplet(float r, glm::vec3 pos) : radius(r) {
	mesh = Sphere(radius, 30); // Create sphere mesh with given radius and resolution
	mesh.setPosition(pos); // Set initial position

	// TEMPORARY BLUE TEXTURE
	mesh.setColor(ofColor(0, 150, 255, 128)); // Semi-transparent blue
}

void WaterDroplet::draw() {
	mesh.draw();
}
