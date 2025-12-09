#pragma once
#include "ofMain.h"
#include "Point.h"

class Vertex {
public:
    glm::vec3 position;
    std::vector<Vertex*> neighbors;

    Vertex(const glm::vec3& pos) : position(pos) {}

    // Helper to add neighbor if not already present
    void addNeighbor(Vertex* v) {
        if (std::find(neighbors.begin(), neighbors.end(), v) == neighbors.end()) {
            neighbors.push_back(v);
        }
    }

    // Helper to remove neighbor
    void removeNeighbor(Vertex* v) {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), v), neighbors.end());
    }

    // Check if this vertex is a neighbor
    bool isNeighbor(Vertex* v) const {
        return std::find(neighbors.begin(), neighbors.end(), v) != neighbors.end();
    }
};


class ThreeDPoint : public Point {
public:
    ThreeDPoint();
    void draw(ofShader* myShader, glm::vec3 pos) override;
    void update(float deltatime) override;

    vector<Vertex*> vertices;
    vector<ofCylinderPrimitive> cylinders;
    std::vector<Vertex*> cutPolyhedron(const glm::vec4& cuttingPlane);
    std::vector<Vertex*> createCube();

    std::vector<ofCylinderPrimitive> createWireframeCylinders(const std::vector<Vertex*>& vertices);
    bool isPointNearCylinder(const glm::vec3& point, const ofCylinderPrimitive& cylinder, float radius);




};


