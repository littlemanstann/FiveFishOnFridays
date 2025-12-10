#pragma once
#include "ofMain.h"

class SeaweedBall {
public:
	// Constructor
	SeaweedBall(ofCamera& camRef);

	// Draw function
	void draw();

	// Clover sample
	glm::vec3 clover_sample();

	void setPosition(const glm::vec3& pos) { positionMatrix = glm::translate(glm::mat4(1.0f), pos); }

	// Self-Delete
	void collect();

private:
	// Camera reference
	ofCamera& cam;
	glm::mat4 positionMatrix;

	ofShader seaweedBall;
	ofVbo vbo;
	ofImage seaweedImage;

	int numParticles = 2000;

	// Variables to hold particle data
	// Variables to hold particle data
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	// Variables for corkscrew trail particle attributes
	vector<glm::float32> phases;
	vector<glm::float32> ids;
};
