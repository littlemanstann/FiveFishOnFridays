#include "3DPoint.h"
#include "ofApp.h"

ThreeDPoint::ThreeDPoint() {
    vertices = createCube();

}

std::vector<Vertex*> ThreeDPoint::createCube() {
    // Create 8 vertices for the cube corners
    Vertex* v000 = new Vertex(glm::vec3(-BOUND_X, -BOUND_Y, -BOUND_Z));
    Vertex* v001 = new Vertex(glm::vec3(-BOUND_X, -BOUND_Y, BOUND_Z));
    Vertex* v010 = new Vertex(glm::vec3(-BOUND_X, BOUND_Y, -BOUND_Z));
    Vertex* v011 = new Vertex(glm::vec3(-BOUND_X, BOUND_Y, BOUND_Z));
    Vertex* v100 = new Vertex(glm::vec3(BOUND_X, -BOUND_Y, -BOUND_Z));
    Vertex* v101 = new Vertex(glm::vec3(BOUND_X, -BOUND_Y, BOUND_Z));
    Vertex* v110 = new Vertex(glm::vec3(BOUND_X, BOUND_Y, -BOUND_Z));
    Vertex* v111 = new Vertex(glm::vec3(BOUND_X, BOUND_Y, BOUND_Z));

    // Connect edges (each vertex has 3 neighbors in a cube)
    // Bottom face (z = -BOUND_Z)
    v000->addNeighbor(v100); v100->addNeighbor(v000);
    v000->addNeighbor(v010); v010->addNeighbor(v000);
    v100->addNeighbor(v110); v110->addNeighbor(v100);
    v010->addNeighbor(v110); v110->addNeighbor(v010);

    // Top face (z = BOUND_Z)
    v001->addNeighbor(v101); v101->addNeighbor(v001);
    v001->addNeighbor(v011); v011->addNeighbor(v001);
    v101->addNeighbor(v111); v111->addNeighbor(v101);
    v011->addNeighbor(v111); v111->addNeighbor(v011);

    // Vertical edges
    v000->addNeighbor(v001); v001->addNeighbor(v000);
    v010->addNeighbor(v011); v011->addNeighbor(v010);
    v100->addNeighbor(v101); v101->addNeighbor(v100);
    v110->addNeighbor(v111); v111->addNeighbor(v110);

    return { v000, v001, v010, v011, v100, v101, v110, v111 };
}

void ThreeDPoint::draw(ofShader* myShader) {
    //Point::draw(myShader);

    
    for (ofCylinderPrimitive c : cylinders) {
        myShader->setUniformMatrix4f("worldMatrix", c.getGlobalTransformMatrix());
        myShader->setUniform3f("objectColor", glm::vec3(1.0, 1.0, 1.0));
        myShader->setUniform1i("texBool", 0);
		myShader->setUniform1i("brightBool", 0);
        c.draw();
    }



}

std::vector<ofCylinderPrimitive> ThreeDPoint::createWireframeCylinders(const std::vector<Vertex*>& vertices) {
    const float EPSILON = 0.001f;
    std::vector<ofCylinderPrimitive> cylinders;

    // Track which edges we've already created to avoid duplicates
    std::set<std::pair<Vertex*, Vertex*>> processedEdges;

    for (Vertex* v : vertices) {
        for (Vertex* neighbor : v->neighbors) {
            // Create ordered pair to check if edge was already processed
            auto edge = (v < neighbor) ? std::make_pair(v, neighbor) : std::make_pair(neighbor, v);

            // Only create cylinder if we haven't processed this edge yet
            if (processedEdges.find(edge) == processedEdges.end()) {
                glm::vec3 v1 = v->position;
                glm::vec3 v2 = neighbor->position;

                glm::vec3 edgeDir = v2 - v1;
                float edgeLength = glm::length(edgeDir);
                glm::vec3 midpoint = (v1 + v2) * 0.5f;

                // Create cylinder
                ofCylinderPrimitive cylinder;
                cylinder.set(8, edgeLength, 6, 1);

                // Position at midpoint
                cylinder.setPosition(midpoint.x, midpoint.y, midpoint.z);

                // Orient cylinder along edge
                glm::vec3 yAxis(0, 1, 0);
                glm::vec3 normalizedEdge = glm::normalize(edgeDir);

                // Calculate rotation
                glm::vec3 rotAxis = glm::cross(yAxis, normalizedEdge);
                float rotAngle = acos(glm::dot(yAxis, normalizedEdge));

                if (glm::length(rotAxis) > EPSILON) {
                    rotAxis = glm::normalize(rotAxis);
                    cylinder.rotate(glm::degrees(rotAngle), rotAxis.x, rotAxis.y, rotAxis.z);
                }
                else if (glm::dot(yAxis, normalizedEdge) < 0) {
                    // Edge points in -Y direction, rotate 180 degrees
                    cylinder.rotate(180, 1, 0, 0);
                }

                cylinders.push_back(cylinder);
                processedEdges.insert(edge);
            }
        }
    }

    return cylinders;
}

void ThreeDPoint::update(float deltatime) {}


std::vector<Vertex*> ThreeDPoint::cutPolyhedron(const glm::vec4& cuttingPlane) {
    const float EPSILON = 0.001f;

    //calculates signed distance from point to plane
    auto signedDistance = [&](const glm::vec3& point) -> float {
        return cuttingPlane.x * point.x + cuttingPlane.y * point.y +
            cuttingPlane.z * point.z + cuttingPlane.w;
        };

    //finds intersection point between edge and plane
    auto findIntersection = [&](const glm::vec3& p1, const glm::vec3& p2) -> glm::vec3 {
        float d1 = signedDistance(p1);
        float d2 = signedDistance(p2);
        float t = d1 / (d1 - d2);
        return p1 + t * (p2 - p1);
        };

    //checks if two positions are approximately equal
    auto positionsEqual = [&](const glm::vec3& p1, const glm::vec3& p2) -> bool {
        return glm::length(p1 - p2) < EPSILON;
        };

    // Determine which side to keep (side containing the point)
    float centerDist = signedDistance(position);
    bool keepNegative = (centerDist < 0);

    // Classify vertices
    std::vector<Vertex*> keptVertices;
    std::vector<Vertex*> removedVertices;

    for (Vertex* v : vertices) {
        float dist = signedDistance(v->position);
        // A vertex is on the same side as center if their distances have the same sign
        bool sameSide = (dist * centerDist >= 0);
        if (sameSide || abs(dist) < EPSILON) { // Keep if same side OR on the plane
            keptVertices.push_back(v);
        }
        else {
            removedVertices.push_back(v);
        }
    }

    // Find all cut edges and create new vertices at intersections.
    // MOre generally if the plane intersects a edge formed by POINTS AB then create a new vertex on that line and keep/cut either A or B
    std::vector<Vertex*> newVertices;
    std::vector<std::pair<Vertex*, Vertex*>> edgeMappings; // (keptVertex, newVertex)

    //for each verx you kept iterate through them to find out which neighbours to remove.
    for (Vertex* kept : keptVertices) {
        std::vector<Vertex*> neighborsToRemove;
        std::vector<std::pair<Vertex*, glm::vec3>> newVerticesToCreate;

        for (Vertex* neighbor : kept->neighbors) {
            // Check if this neighbor is being removed
            if (std::find(removedVertices.begin(), removedVertices.end(), neighbor) != removedVertices.end()) {
                // Edge is cut - calculate intersection position
                glm::vec3 intersectionPos = findIntersection(kept->position, neighbor->position);
                newVerticesToCreate.push_back({ neighbor, intersectionPos });
                neighborsToRemove.push_back(neighbor);
            }
        }

        // Now create/connect new vertices
        for (auto& [removedNeighbor, intersectionPos] : newVerticesToCreate) {
            // Check if we already created a vertex at this position
            Vertex* newVertex = nullptr;
            for (Vertex* nv : newVertices) {
                if (positionsEqual(nv->position, intersectionPos)) {
                    newVertex = nv;
                    break;
                }
            }

            // Create new vertex if it doesn't exist
            if (newVertex == nullptr) {
                newVertex = new Vertex(intersectionPos);
                newVertices.push_back(newVertex);
            }

            // Connect kept vertex to new vertex
            kept->addNeighbor(newVertex);
            newVertex->addNeighbor(kept);

            // Store mapping for later face creation
            edgeMappings.push_back({ kept, newVertex });
        }

        // Remove connections to removed vertices
        for (Vertex* toRemove : neighborsToRemove) {
            kept->removeNeighbor(toRemove);
        }
    }

    // Connect new vertices to form the cut face
    if (newVertices.size() >= 3) {
        // Calculate centroid of new vertices
        // by taking a weighted avg of the position of all new vertices
        glm::vec3 centroid(0, 0, 0);
        for (Vertex* nv : newVertices) {
            centroid += nv->position;
        }
        centroid /= static_cast<float>(newVertices.size());

        // The normal of the cutting plane will be the same one as the surface normal of the centroid
        glm::vec3 planeNormal(cuttingPlane.x, cuttingPlane.y, cuttingPlane.z);
        planeNormal = glm::normalize(planeNormal);

        // Create a coordinate system on the plane
        // pick an arbitrary vector not parallel to normal
        glm::vec3 arbitrary = (abs(planeNormal.x) < 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        glm::vec3 u = glm::normalize(glm::cross(planeNormal, arbitrary));
        glm::vec3 v = glm::cross(planeNormal, u);

        // Calculate angle for each new vertex relative to centroid
        std::vector<std::pair<float, Vertex*>> angledVertices;
        for (Vertex* nv : newVertices) {
            glm::vec3 offset = nv->position - centroid;
            float angle = atan2(glm::dot(offset, v), glm::dot(offset, u));
            angledVertices.push_back({ angle, nv });
        }

        // Sort by angle
        std::sort(angledVertices.begin(), angledVertices.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });

        // Connect vertices in cyclic order
        // This way we can connect our new polygonal shape face without crossing nay edges.
        for (size_t i = 0; i < angledVertices.size(); i++) {
            Vertex* current = angledVertices[i].second;
            Vertex* next = angledVertices[(i + 1) % angledVertices.size()].second;

            current->addNeighbor(next);
            next->addNeighbor(current);
        }
    }

    // Ensure no kept vertex has edges to removed vertices
    for (Vertex* kept : keptVertices) {
        std::vector<Vertex*> toRemove;
        for (Vertex* neighbor : kept->neighbors) {
            if (std::find(removedVertices.begin(), removedVertices.end(), neighbor) != removedVertices.end()) {
                toRemove.push_back(neighbor);
            }
        }
        for (Vertex* r : toRemove) {
            kept->removeNeighbor(r);
        }
    }

    // Build result vector
    std::vector<Vertex*> result;
    result.insert(result.end(), keptVertices.begin(), keptVertices.end());
    result.insert(result.end(), newVertices.begin(), newVertices.end());

    return result;
}
