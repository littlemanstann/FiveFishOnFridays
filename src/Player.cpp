#include "Player.h"

Player::Player() {

		model.load("models/FISH.glb");
		model.enableNormals();
		model.setScale(0.03, -0.03, -0.03);//this is while trying to approximate scale to current movement


}

void Player::draw() {

	ofSetColor(1.0);
	model.drawFaces();
}
