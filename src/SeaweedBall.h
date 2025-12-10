#pragma once
#include "ofMain.h"

struct Seaweed {
	glm::vec3 pos;
	glm::vec3 vel;
	float age = 0;
	float lifetime = 1.0f;
	float phase = 0;
};

class SeaweedBall {
public:
	// Constructor
	SeaweedBall(ofCamera& camRef);

	// Draw function
	void draw();
	void update(float dt);

	// Clover sample
	glm::vec3 clover_sample();

	void setPosition(const glm::vec3& pos) { position = pos, positionMatrix = glm::translate(glm::mat4(1.0f), pos); }

	// Self-Delete
	void collect();

private:
	// Camera reference
	ofCamera& cam;
	glm::mat4 positionMatrix;
	glm::vec3 position;
	std::vector<Seaweed> seaweed;
	//std::vector<float> vboAlpha;

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
