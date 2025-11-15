#include "MeshObject.h"

void MeshObject::draw() {
    ofPushMatrix();

	// Set the transformation matrix
    ofMultMatrix(matrix);
    ofSetColor(color);
    mesh.draw();

    ofPopMatrix();
}

// Translate by (dx, dy, dz)
void MeshObject::translate(float dx, float dy, float dz) {
    position += glm::vec3(dx, dy, dz);
	matrix = glm::translate(glm::mat4(1.0f), position);
}

// Set position to (x, y, z)
void MeshObject::setPosition(const glm::vec3& pos) {
	position = pos;
	matrix = glm::translate(glm::mat4(1.0f), position);
}

// Get current position
ofVec3f MeshObject::getPosition() const {
    return position;
}

// Set rotation by angle (degrees) around axis
void MeshObject::setRotation(float angleDeg, const glm::vec3& axis) {
	rotationAngle = angleDeg;
	rotationAxis = axis;
	matrix = glm::rotate(matrix, glm::radians(angleDeg), axis);
}

// Set rotation matrix by angle (degrees) around axis (uses RotationMatrix variable)
void MeshObject::setRotationMatrix(float angleDeg, const glm::vec3& axis) {
	rotationAngle += angleDeg;
	rotationAxis = axis;
	rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxis);
}

// Set orbit matrix by angle (degrees) around axis and center point (uses RotationMatrix variable, shared with setRotationMatrix)
void MeshObject::setOrbitMatrix(float angleDeg, const glm::vec3& axis, const glm::vec3& center) {
	rotationAngle += angleDeg;
	rotationAxis = axis;
	rotationMatrix = glm::translate(glm::mat4(1.0f), center) *
					 glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxis) *
					 glm::translate(glm::mat4(1.0f), -center);
}

// Get the current rotation/orbit matrix
glm::mat4 MeshObject::getRotationMatrix() const {
    return rotationMatrix;
}