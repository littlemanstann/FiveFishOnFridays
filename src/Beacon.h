#pragma once

#include "MeshObject.h"

class Beacon : public MeshObject {


public:
	static bool collisionCheck(glm::vec3 point1, glm::vec3 point2, float rad);
	// Constructor
	Beacon(float scale, glm::vec3 location);
	
	ofColor getColor() const { return color; };
	float getRadius() const { return radius; };

	// Beacon status
	void setCurrentBeacon(bool status) { color = ofColor::green; }
	bool isCurrentBeacon() const { return currentBeacon; }

	// Self-Delete
	void collect();

private:
	float radius;
	bool currentBeacon;
};
