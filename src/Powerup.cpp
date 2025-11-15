#include "Powerup.h"

Powerup::Powerup(float r, glm::vec3 pos) {
	radius = r;
	position = pos;
	dead = false;
	mesh = ofMesh::sphere(r);
	setPosition(pos.x ,pos.y, pos.z);
}

HealthPowerup::HealthPowerup(float r, glm::vec3 pos, float hp)
	: Powerup(r, pos) {
	health = hp;
}

SpeedPowerup::SpeedPowerup(float r, glm::vec3 pos, float s)
	: Powerup(r, pos) {
	speed = s;
	setColor(ofColor(0, 0, 255, 255));
}
