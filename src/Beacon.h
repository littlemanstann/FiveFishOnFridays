#pragma once

#include "MeshObject.h"

class Beacon : public MeshObject {


public:
	static bool collisionCheck(glm::vec3 point1, glm::vec3 point2, float rad);
	// Constructor
	Beacon(float scale, glm::vec3 location);
	Beacon(float scale, glm::vec3 location, float rX, float rY, float rZ, int maRes, int miRes, float factor); // factor is how much larger/smaller the second blob is
	
	ofMesh createEllipsoidMesh(float rx, float ry, float rz, int majorRes, int minorRes);
	ofMesh createEllipsoidMesh2(float rx, float ry, float rz, int majorRes, int minorRes);

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
	ofMesh mesh2;
};
