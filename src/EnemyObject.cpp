#include "EnemyObject.h"

EnemyObject::EnemyObject(float scale, glm::vec3 location) {
	mesh = ofMesh::box(10 * scale, 10 * scale, 10 * scale);
	setPosition(location.x + 10, location.y, location.z - 10);
	setColor(ofColor::grey);
	radius = 5 * scale;
	//currentBeacon = false;
}

bool EnemyObject::collisionCheck(glm::vec3 point, float radius) {
	if (glm::length(glm::vec3(getPosition() - point)) < (this->radius + radius)) {

		printf("Enemy collision!\n");
		return true;
	}
	return false;
}

void EnemyObject::chase(const glm::vec3& playerPosition, float speed) {
	// Calculate the direction vector from the object to the player
	glm::vec3 direction = glm::normalize(playerPosition - getPosition());
	glm::vec3 movement = direction * speed;
	position += movement;
}