#include "SeaweedBall.h"


SeaweedBall::SeaweedBall(ofCamera& camRef) 
    : cam(camRef)
{
    positions.resize(numParticles);
    normals.resize(numParticles);
    phases.resize(numParticles);
    ids.resize(numParticles);
    seaweedImage.load("images/firesheet4x4.jpg");
    seaweedBall.load("shader/trailShader/trail.vert", "shader/trailShader/trail.frag", "shader/trailShader/trail.geom");

    for (int i = 0; i < numParticles; i++) {
        positions[i] = clover_sample(); // start at origin
        normals[i] = positions[i];
        ids[i] = floor(ofRandom(16.0)); // assign random texture ID out of 16 available options (4x4 sprite sheet)
        phases[i] = ofRandom(0, 1);
    }
    vbo.setVertexData(positions.data(), numParticles, GL_STATIC_DRAW);
    vbo.setAttributeData(1, &normals[0].x, 3, numParticles, GL_STATIC_DRAW);
    vbo.setAttributeData(2, phases.data(), 1, numParticles, GL_STATIC_DRAW);
    vbo.setAttributeData(3, ids.data(), 1, numParticles, GL_STATIC_DRAW);
}

void SeaweedBall::draw() {
    ofMatrix4x4 modelMatrix = positionMatrix;
    ofMatrix4x4 viewMatrix = cam.getModelViewProjectionMatrix();
    ofMatrix4x4 mvp = viewMatrix;

    seaweedBall.begin();

    seaweedBall.setUniform1f("pSize", 0.09); // particle point size
    seaweedBall.setUniform1f("t", ofGetElapsedTimef()); // time
    seaweedBall.setUniformTexture("texIndex", seaweedImage.getTextureReference(), 1); // Change texture based on IDs

    seaweedBall.setUniformMatrix4f("MVP", mvp); // full transformation for particle system

    vbo.draw(GL_POINTS, 0, positions.size()); // draw all particles

    seaweedBall.end();
}

glm::vec3 SeaweedBall::clover_sample() {
    // Clover shape parameters
    const float rIn = 0.1f; // Interior radius of clover
    const float rExt = 0.3f; // Exterior radius of clover
    const int N = 5;  // Number of leaves on the clover

    // Pick random angle
    float theta = ofRandom(0, TWO_PI);

    // Boundary radius at this angle (in polar coordinates)
    float r = rIn + (rExt - rIn) * fabs(cos(N * theta * 0.5f));

    // Convert to cartesian coordinates
    float x = r * cos(theta);
    float y = r * sin(theta);

    // Return the point in the XY plane
    return glm::vec3(x, y, 0);
}