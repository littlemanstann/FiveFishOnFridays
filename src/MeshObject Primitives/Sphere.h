#pragma once
#include "ofMain.h"
#include "MeshObject.h"

class Sphere : public MeshObject {
public:
    // Constructors
    // Default constructor
	Sphere() { 
        ofSpherePrimitive sphere;
        sphere.set(20, 20); 
    } 
	Sphere(float radius, int resolution) {
        ofSpherePrimitive sphere;   // OpenFrameworks built-in sphere primitive
        sphere.set(radius, resolution); // Set radius and resolution
        mesh = sphere.getMesh(); // Initialize the mesh from the primitive
	}
};

