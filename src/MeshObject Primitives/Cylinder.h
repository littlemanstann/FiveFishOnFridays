#pragma once
#include "ofMain.h"
#include "MeshObject.h"

class Cylinder : public MeshObject {
public:
    // Constructors
    // Default constructor
	Cylinder() { 
        ofCylinderPrimitive cylinder;
        cylinder.set(10, 20); 
    } 
    Cylinder(float radius, float height) {
        ofCylinderPrimitive cylinder;
		cylinder.set(radius, height); // Set radius and height
		mesh = cylinder.getMesh(); // Initialize the mesh from the primitive
    }
};

