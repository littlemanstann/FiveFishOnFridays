#include "Beacon.h"

Beacon::Beacon(float scale, glm::vec3 location) {
	// Create a simple beacon mesh (e.g., a cone)
	mesh = ofMesh::cone(10 * scale, 30 * scale, 16, 1, true);
	setPosition(location.x, location.y, location.z);
	setColor(ofColor::yellow);
	radius = 10 * scale;
	currentBeacon = false;
}

bool Beacon::collisionCheck(glm::vec3 point1, glm::vec3 point2, float rad) {
	if (glm::length(glm::vec3(point1 - point2)) < (rad + rad)) {
		printf("Collision!\n");
		return true;
	}
	return false;
}

void Beacon::collect() {
	
}


