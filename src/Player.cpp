#include "Player.h"

Player::Player() {

	radius = 12;
	height = 12;
	mesh = ofMesh::cone(radius, height, 12);
	position = glm::vec3(0, 0, 0);



}
