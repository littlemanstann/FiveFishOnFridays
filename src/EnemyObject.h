#pragma once

#include "MeshObject.h"
#include "Powerup.h"

class EnemyObject : public MeshObject {
public:
	// Constructor
	EnemyObject(float scale, glm::vec3 enemyLocation);
	bool collisionCheck(glm::vec3 point, float radius);
	const glm::vec3 getEnemyLocation() { return enemyLocation; }
	void chase(const glm::vec3& playerPosition, float speed);

private:
	float radius;
	glm::vec3 enemyLocation;
};