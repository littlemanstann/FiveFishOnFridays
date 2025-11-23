#pragma once
#include "ofMain.h"
#include "MeshObject.h"

class Cone : public MeshObject {
public:
    // Constructors
    // Default constructor
    Cone() { 
        ofConePrimitive cone;
        cone.set(20, 20); 
    } 
	Cone(float radius, int resolution) {
        ofConePrimitive cone;   // OpenFrameworks built-in cone primitive
        cone.set(radius, resolution); // Set radius and resolution
        mesh = cone.getMesh(); // Initialize the mesh from the primitive
	}
};

