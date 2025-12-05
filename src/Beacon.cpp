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


// amoeba mesh
Beacon::Beacon(float scale, glm::vec3 location, float rX, float rY, float rZ, int maRes, int miRes, float factor) {
	setPosition(location.x, location.y, location.z);
	mesh = createEllipsoidMesh(rX, rY, rZ, maRes, miRes);
	mesh2 = createEllipsoidMesh(rX-5.0f, rY-4.0f, rZ-3.0f, maRes-10, miRes-15);
	mesh2.draw();
}

ofMesh Beacon::createEllipsoidMesh(float rx, float ry, float rz, int majorRes, int minorRes) {
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);

	for (int i = 0; i < majorRes; i++) {
		float theta = TWO_PI * i / majorRes; // angle around y-axis

		for (int j = 0; j < minorRes; j++) {
			float phi = PI * j / (minorRes - 1); // angle top to bottom

			float x = rx * sin(phi) * cos(theta);
			float y = ry * sin(phi) * sin(theta);
			float z = rz * cos(phi);

			glm::vec3 pos(x, y, z);

			glm::vec3 N = glm::normalize(glm::vec3(
				x / rx,
				y / ry,
				z / rz));

			mesh.addVertex(pos);
			mesh.addNormal(N);

			mesh.addColor(ofColor(0, 300, 0));
		}
	}

	for (int i = 0; i < majorRes; i++) {
		int iNext = (i + 1) % majorRes;

		for (int j = 0; j < minorRes - 1; j++) {
			int id0 = i * minorRes + j;
			int id1 = i * minorRes + (j + 1);
			int id2 = iNext * minorRes + (j + 1);
			int id3 = iNext * minorRes + j;

			mesh.addIndex(id0);
			mesh.addIndex(id1);
			mesh.addIndex(id2);

			mesh.addIndex(id0);
			mesh.addIndex(id2);
			mesh.addIndex(id3);
		}
	}

	return mesh;
}

ofMesh Beacon::createEllipsoidMesh2(float rx, float ry, float rz, int majorRes, int minorRes) {
	mesh2 = createEllipsoidMesh(rx, ry, rz, majorRes, minorRes);

	// move it sideways to connect to first blob
	glm::vec3 offset(rx + 50.0f, 0.0f, 0.0f);

	for (int i = 0; i < mesh2.getNumVertices(); i++) {
		glm::vec3 v = mesh2.getVertex(i);
		mesh2.setVertex(i, v + offset);
	}

	return mesh2;
}