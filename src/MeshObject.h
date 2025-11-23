#pragma once
#include "ofMain.h"

// Abstract function for basic mesh manipulation functions
class MeshObject {
public:
    MeshObject() {}
	MeshObject(const ofMesh& m) : mesh(m) {}
    virtual ~MeshObject() {}

    // Draw with current transform
    virtual void draw();
	virtual void setColor(const ofColor& c) { color = c; }

    // Getter and setter for transformation matrix
	virtual void setMatrix(const glm::mat4& mat) { matrix = mat; }
	virtual glm::mat4 getMatrix() const { return matrix; }

    // Translation
    virtual void translate(float dx, float dy, float dz);
	virtual void translate(const glm::vec3& delta) { translate(delta.x, delta.y, delta.z); }
    virtual void setPosition(float dx, float dy, float dz) { setPosition(glm::vec3(dx, dy, dz)); }
    virtual void setPosition(const glm::vec3& pos);
    virtual ofVec3f getPosition() const;

    // Rotation
    virtual void setRotation(float angleDeg, const glm::vec3& axis);
    virtual void setRotationMatrix(float angleDeg, const glm::vec3& axis);
	virtual void setOrbitMatrix(float angleDeg, const glm::vec3& axis, const glm::vec3& center); // Does the equation for orbiting around a point
    virtual glm::mat4 getRotationMatrix() const;

protected:
    ofMesh mesh;
    glm::mat4 matrix = glm::mat4(1.0f); // matrix used for transforms
    glm::vec3 position = { 0, 0, 0 };
    float rotationAngle = 0.0f;
    glm::vec3 rotationAxis = { 0, 1, 0 }; // default = Y axis
    glm::mat4 rotationMatrix = glm::mat4(1.0f); // matrix used for rotations + orbits
	ofColor color = ofColor::white; // default color
};