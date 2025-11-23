#include "Player.h"

Player::Player() {



	for (int i = 0; i < 5; i++) {
		model[i].load("models/FISH.glb");
		model[i].enableNormals();
		model[i].setScale(0.03, -0.03, -0.03);//this is while trying to approximate scale to current movement
		
		meshObj[i] = MeshObject(model[i].getMesh(i));
	}

	
}

void Player::draw() {

	ofSetColor(1.0);
	for (int i = 0; i < 5; i++) {
		model[i].drawFaces();
	}
}
