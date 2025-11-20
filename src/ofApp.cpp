#include "ofApp.h"

ofVbo vbo; // vertex buffer object to store all the particle data
ofNode particleNode; // ofNode for using oF transform functions; will apply the node's transform to the particle system

vector<glm::vec3> positions;
vector<glm::vec3> normals;
ofShader shader;

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Final Project - Five Fish On Fridays - 3501");
	ofBackground(0);
	ofSetFrameRate(60); // caps the framerate at 60FPS

	ofSetSphereResolution(50);

	ofTrueTypeFont::setGlobalDpi(72);

	ofDisableArbTex();
	ofLoadImage(mTex, "earth.jpg");
	mTex.generateMipmap();
	mTex.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);

	//PLAYER
	cone.set(1, 3);

	modelPlayer.load("models/FISH.glb");
	modelPlayer.enableNormals();

	player = new Player();

	modelFox.loadModel("models/Fox_05.fbx");
	mousePosition = glm::vec2(0.0,0.0);

	// Camera setup
	cam.setNearClip(0.1f);
	cam.setFarClip(1000.0f);
	cam.setPosition(0, 0, 10);
	cam.lookAt(glm::vec3(0, 0, 0));

	emitter = std::make_unique<BubbleEmitter>(cam);
	emitter->setPosition(glm::vec3(100, 100, -200));
	emitter->setSize(1); // cube size

	box1.set(1);
	cone1.set(1, 2);
	box1.move(100, 101, -202);
	cone1.move(101, 101, -203);

	box2.set(1);
	cone2.set(1, 2);
	box2.move(0, -4, 13);
	cone2.move(1, -5, 12);

	int numParticles = 2000;
	positions.resize(numParticles);
	normals.resize(numParticles);

	for (int i = 0; i < numParticles / 2; i++) {
		positions[i] = sphere_sample();

		normals[i] = glm::normalize(positions[i]); // normalize puts them on the surface of a sphere
		//        normals[i] = positions[i]; // plain position provides variable velocity
		//        normals[i] = positions[i]*2*length(positions[i]); // even less uniform, ones on the outside travelling even faster
	}
	for (int i = numParticles / 2; i < numParticles; i++) {
		positions[i] = circle_sample() * ofRandom(1.3f, 1.5f);
		normals[i] = glm::normalize(positions[i]);
	}
	vbo.setVertexData(positions.data(), numParticles, GL_STATIC_DRAW);
	vbo.setNormalData(normals.data(), numParticles, GL_STATIC_DRAW);

	shader.load("shader/particle.vert", "shader/particle.frag", "shader/particle.geom");


	//LIGHTING
	ofEnableSmoothing();
	ofSetGlobalAmbientColor(ofColor(155, 155, 155));
	ofSetSmoothLighting(true); //no documentation
	ofSetSphereResolution(100);
	ofEnableLighting(); // use lighting calculations
	myLight.setPointLight(); // it is a point light


	calibri.load("calibri.ttf", 32, true, true);
	calibri.setLineHeight(18.0f);
	calibri.setLetterSpacing(1.037);
	


	asteroids = 200;
	for (int i = 0; i < asteroids; i++)
	{
		body[i].setPosition(
			glm::vec3(
				ofRandom(-800, 800),
				ofRandom(-800, 800),
				ofRandom(-800, 800))); // random somewhere in space
		asteroidVector.push_back(body[i]);
	}

	// Setting up beacons at the cardinal points
	beacons.push_back(Beacon(1.0f, glm::vec3(0, 50, 200)));
	beacons.back().setCurrentBeacon(true); // First beacon is the current beacon
	beacons.push_back(Beacon(1.0f, glm::vec3(200, -40, 150)));
	beacons.push_back(Beacon(1.0f, glm::vec3(300, 0, 0)));
	beacons.push_back(Beacon(1.0f, glm::vec3(20, 50, -5)));
	beacons.push_back(Beacon(1.0f, glm::vec3(-250, -100, -5)));
	beacons.push_back(Beacon(1.0f, glm::vec3(-250, 300, -40)));

	enemies.push_back(EnemyObject(1.0f, glm::vec3(0, 70, 200)));
	enemies.push_back(EnemyObject(1.0f, glm::vec3(200, -20, 150)));
	enemies.push_back(EnemyObject(1.0f, glm::vec3(300, 20, 0)));
	enemies.push_back(EnemyObject(1.0f, glm::vec3(20, 70, -5)));
	enemies.push_back(EnemyObject(1.0f, glm::vec3(-250, -80, -5)));
	enemies.push_back(EnemyObject(1.0f, glm::vec3(-250, 320, -40)));
	enemyActivation = 0;

	healthPowerups.push_back(new HealthPowerup(20, glm::vec3(0,0,200), 1));
	healthPowerups.push_back(new HealthPowerup(20, glm::vec3(0, 0, -200), 1));
	healthPowerups.push_back(new HealthPowerup(20, glm::vec3(0, 100, 100), 1));
	healthPowerups.push_back(new HealthPowerup(20, glm::vec3(0, 100, -100), 1));

	speedPowerups.push_back(new SpeedPowerup(20, glm::vec3(100, 0, 200), POWERUP_BOOST_FORCE));
	speedPowerups.push_back(new SpeedPowerup(20, glm::vec3(100, 0, -200), POWERUP_BOOST_FORCE));
	speedPowerups.push_back(new SpeedPowerup(20, glm::vec3(100, 100, 100), POWERUP_BOOST_FORCE));
	speedPowerups.push_back(new SpeedPowerup(20, glm::vec3(100, 100, -100), POWERUP_BOOST_FORCE));


	// Set up font
	msgFont.loadFont("trixiepro_heavy.otf", 128);
}

glm::vec3 ofApp::sphere_sample()
{
	// random sample from within a sphere
	glm::vec3 p;

	// use simple rejection sampling
	do {
		p = glm::vec3(
			ofRandom(-1.0f, 1.0f),
			ofRandom(-1.0f, 1.0f),
			ofRandom(-1.0f, 1.0f)
		);
	} while (glm::length2(p) > 1.0f); // is length^2 > 1? Failed, try again

	// out of the loop, must have found a legal sample

	return p;
}

glm::vec3 ofApp::circle_sample()
{
	// random sample from within a sphere
	glm::vec3 p;

	// use simple rejection sampling
	do {
		p = glm::vec3(
			ofRandom(-1.0f, 1.0f),
			0,
			ofRandom(-1.0f, 1.0f)
		);
	} while (glm::length2(p) > 1.0f); // is length^2 > 1? Failed, try again

	// out of the loop, must have found a legal sample

	return p;
}

//--------------------------------------------------------------
void ofApp::update() {

	
	float dayTime = 3;
	myLight.setGlobalPosition(sin(ofGetElapsedTimef() / dayTime) * 1000, 1000, cos(ofGetElapsedTimef() / dayTime) * 1000); // where it is


	cam.update(SIXTY_FPS, size); // 60 fps

	if (emitter) emitter->update(SIXTY_FPS);
	if (health == 0) {
		enemies.clear();
		asteroidVector.clear();
		// Stop player movement
		cam.canMove(false);
		gameLost = true;
	}

	// Set playerPos to be the object in third-person view
	playerPos = cam.getPosition() + ((cam.getqForward() * 10) + glm::vec3(0, 1, 0) * -3);

	//Check for Collisions Between HealthPowerups and the Player
	for (int i = 0; i < healthPowerups.size(); i++) {
		if (Beacon::collisionCheck(healthPowerups[i]->getPosition(), playerPos, PLAYER_RADIUS * size + healthPowerups[i]->getRadius())) {
			healthPowerups[i]->setDead();

			//Collision!
			printf("Collision with Health Powerup!\n");
			printf("Prev Health: %.0f\n", health);

			//update the health variable
			health += healthPowerups[i]->getHealth();

			//print the new health
			printf("New Health: %.0f\n\n", health);
		}
	}	glm::vec3 test = playerPos;

	//Check for Collisions Between SpeedPowerups and the Player
	for (int i = 0; i < speedPowerups.size(); i++) {
		if (Beacon::collisionCheck(speedPowerups[i]->getPosition(), playerPos, PLAYER_RADIUS*size + speedPowerups[i]->getRadius())){
			speedPowerups[i]->setDead();

			//Collision!
			printf("Collision with Speed Powerup!\n");

			//update the speed variable
			cam.addAcceleration(speedPowerups[i]->getSpeed());
		}
	}


	// Check for Colllisions between the first beacon and the Player
	if (!beacons.empty() && Beacon::collisionCheck(beacons.front().getPosition(), playerPos, PLAYER_RADIUS*size +beacons.front().getRadius())) {
		beacons.erase(beacons.begin());
		// Handle win condition
		if (beacons.size() == 0) {
			printf("You Win!\n");
			// Handle asteroid clearing and enemy clearing
			printf("All enemies and asteroids erased.");
			enemies.clear();
			asteroidVector.clear();
			gameWon = true;
			//ofExit();
		}
		else {
			beacons.front().setCurrentBeacon(true);
			enemyActivation++;
		}
	}

	// Check for collisions Between Enemies and the Player
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i].collisionCheck(playerPos, PLAYER_RADIUS)) {
			enemies.erase(enemies.begin() + i);
			printf("Enemy %i defeated!", i);
			health --;
			printf("New Health: %.0f\n\n", health);
		}
	}

	// Delete Objects Marked for Deletion
	healthPowerups.erase(std::remove_if(healthPowerups.begin(), healthPowerups.end(), [](HealthPowerup * obj) { return obj->getDead(); }), healthPowerups.end());
	speedPowerups.erase(std::remove_if(speedPowerups.begin(), speedPowerups.end(), [](SpeedPowerup * obj) { return obj->getDead(); }), speedPowerups.end());
	//Grow Shrink Player Size (Jansen Implementation)
	if (ofGetKeyPressed('e')) {
		size += SIXTY_FPS;
		if(size > MAX_SIZE) {
			size = MAX_SIZE;
		}
	}
	if (ofGetKeyPressed('q')) {
		size -= SIXTY_FPS;
		if (size < MIN_SIZE) {
			size = MIN_SIZE;
		}
	}

	// Enemy closest to current beacon starts moving towards it, stacks
	if (!enemies.empty()) {
		enemies[0].chase(cone.getPosition(), 0.5);

		if (enemies.size() >= 2 && enemyActivation >= 1) {
			enemies[1].chase(cone.getPosition(), 0.5);
		}
		if (enemies.size() >= 3 && enemyActivation >= 2) {
			enemies[2].chase(cone.getPosition(), 0.5);
		}
		if (enemies.size() >= 4 && enemyActivation >= 3) {
			enemies[3].chase(cone.getPosition(), 0.5);
		}
		if (enemies.size() >= 5 && enemyActivation >= 4) {
			enemies[4].chase(cone.getPosition(), 0.5);
		}
		if (enemies.size() == 6 && enemyActivation >= 5) {
			enemies[5].chase(cone.getPosition(), 0.5);
		}
	}

	//// Respawn enemies if all are defeated
	//if (enemies.empty() && !gameWon && !gameLost) {
	//	enemies.push_back(EnemyObject(1.0f, playerPos + glm::vec3(300, 0, -20)));
	//	enemies.push_back(EnemyObject(1.0f, playerPos + glm::vec3(-100, 0, -20)));
	//	enemies.push_back(EnemyObject(1.0f, playerPos + glm::vec3(100, 200, -20)));
	//	enemies.push_back(EnemyObject(1.0f, playerPos + glm::vec3(100, -200, -20)));
	//	enemies.push_back(EnemyObject(1.0f, playerPos + glm::vec3(-300, 200, -20)));
	//	enemies.push_back(EnemyObject(1.0f, playerPos + glm::vec3(-100, 300, -20)));
	//	enemyActivation = 0;
	//}

	// Asteroid collision with the Player also does damage
	for (int i = 0; i < asteroidVector.size(); i++) {
		bool crash = false;
		if (glm::length(glm::vec3(asteroidVector[i].getPosition() - cone.getPosition())) < (5 + PLAYER_RADIUS)) {

			printf("Asteroid collision!\n");
			crash = true;
		}
		if (crash) {
			asteroidVector.erase(asteroidVector.begin() + i);
			printf("Asteroid %i destroyed!", i);
			health--;
			printf("New Health: %.0f\n\n", health);
			crash = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	cam.begin();
	myLight.enable();
	
	ofEnableDepthTest();


	ofSetColor(255);
	for (int i = 0; i < asteroids; i++)
		body[i].draw();

	for (Beacon beacon : beacons) {
		beacon.draw();
	}

	for (EnemyObject enemy : enemies) {
		enemy.draw();
	}

	mTex.bind();
	for (int i = 0; i < healthPowerups.size(); i++) {
		healthPowerups[i]->draw();
	}
	mTex.unbind();

	for (int i = 0; i < speedPowerups.size(); i++) {
		speedPowerups[i]->draw();
	}
	
	//3rd person Camera (Jansen Implementation)
	cone.setScale(size, size, size);
	//Can use the cam.getqUp vector to keep the cone static right below the crosshair but it looks cooler this way.
	cone.setPosition(cam.getPosition() + ((cam.getqForward()*10) + glm::vec3(0,1,0) * -3));
	cone.lookAt(cam.getPosition() + (cam.getqForward() * 15), cam.getqUp());
	cone.rotateDeg(90, cam.getqSide());


	cone.draw();


	//DrawPlayerModel

	//ofSetColor(1.0);
	////modelPlayer.drawFaces();
	player->draw();
	

		// Draw Player Collision Sphere (Yansen Implementation) (For debugging) (drive beside the collidable object then turn to see if cone will collide with it)
	//ofDrawSphere(cam.getPosition() + ((cam.getqForward() * 10) + glm::vec3(0, 1, 0) * -3), PLAYER_RADIUS);
	
	ofDisableDepthTest(); //With this disabled, UI (below) no longer gets clipped in 3D space
	myLight.disable();
	cam.end();

	//// Crosshairs (Yansen Inplementation)
	float sx = ofGetWidth() / 2.0f;
	float sy = ofGetHeight() / 2.0f;

	float w = 15;
	float h = 2;

	ofDrawRectangle(sx - w / 2, sy - h / 2, w, h);
	w = 2;
	h = 15;

	////Speed and Health UI
	ofSetColor(0, 255, 0, 255);
	calibri.drawString(("Health: " + ofToString(health)), 32, 50);
	ofSetColor(0, 0, 255, 255);
	calibri.drawString("Speed: " + ofToString(glm::length(cam.getSpeed())), 32, 100);

	ofDrawRectangle(sx - w / 2, sy - h / 2, w, h);

	//// Display Messages
	if (gameWon) {
		ofSetColor(ofColor::red);
		msgFont.drawString("You Win!", 150, 400);
	}
	if (gameLost) {
		ofSetColor(ofColor::red);
		msgFont.drawString("GAME OVER.\nTry Again.", 150, 400);
	}

	bool bText = true;
	bool placeholderBool = true;
	if (bText) {
		ofSetColor(255, 255, 255);
		string str;
		str += "Placeholder Bool Shading: ";
		if (placeholderBool)
			str += "On\n";
		else
			str += "Off\n";

		str += "(r): x Shading Toggle\n";
		str += "(e): x Toggle\n";
		ofDrawBitmapString(str, 20, 120);
	}

	ofEnableDepthTest();
	ofSetColor(255);
	cam.begin();
	ofSetColor(100);
	cone1.drawFaces();
	box1.drawFaces();
	ofSetColor(255);
	if (emitter) emitter->draw();
	cam.end();


	ofSetColor(255);
	ofDrawBitmapString("BubbleEmitter Test", 20, 20);

	particleNode.setPosition(glm::vec3(0.1, 0.1, 0)); // or whatever -- move particle system here

	ofPushMatrix();

	cam.begin();
	cone2.drawFaces();
	box2.drawFaces();
	shader.begin();



	shader.setUniform1f("pSize", 0.02); // particle point size
	shader.setUniform1f("t", ofGetElapsedTimef()); // time


	ofMatrix4x4 modelMatrix = particleNode.getGlobalTransformMatrix();
	ofMatrix4x4 viewProjMatrix = cam.getModelViewProjectionMatrix();

	ofMatrix4x4 mvp = viewProjMatrix * modelMatrix; // combine camera and ofNode transform

	shader.setUniformMatrix4f("MVP", mvp); // full transformation for particle system
	shader.setUniform1f("speed", 0.05);

	vbo.draw(GL_POINTS, 0, positions.size() / 2 - 1); // draw all particles

	shader.setUniform1f("speed", 0.3);
	vbo.draw(GL_POINTS, positions.size() / 2, positions.size());

	ofPopMatrix();

	particleNode.setPosition(glm::vec3(-0.3, -0.45, 0)); // or whatever -- move particle system here

	ofPushMatrix();

	cam.begin();
	shader.begin();

	shader.setUniform1f("pSize", 0.02); // particle point size
	shader.setUniform1f("t", ofGetElapsedTimef()); // time


	modelMatrix = particleNode.getGlobalTransformMatrix();
	viewProjMatrix = cam.getModelViewProjectionMatrix();

	mvp = viewProjMatrix * modelMatrix; // combine camera and ofNode transform

	shader.setUniformMatrix4f("MVP", mvp); // full transformation for particle system
	shader.setUniform1f("speed", 0.05);

	vbo.draw(GL_POINTS, 0, positions.size() / 2 - 1); // draw all particles

	shader.setUniform1f("speed", 0.3);
	vbo.draw(GL_POINTS, positions.size() / 2, positions.size());

	ofPopMatrix();

	particleNode.setPosition(glm::vec3(0.3, -0.6, 0)); // or whatever -- move particle system here

	ofPushMatrix();

	cam.begin();
	shader.begin();

	shader.setUniform1f("pSize", 0.02); // particle point size
	shader.setUniform1f("t", ofGetElapsedTimef()); // time


	modelMatrix = particleNode.getGlobalTransformMatrix();
	viewProjMatrix = cam.getModelViewProjectionMatrix();

	mvp = viewProjMatrix * modelMatrix; // combine camera and ofNode transform

	shader.setUniformMatrix4f("MVP", mvp); // full transformation for particle system
	shader.setUniform1f("speed", 0.05);

	vbo.draw(GL_POINTS, 0, positions.size() / 2 - 1); // draw all particles

	shader.setUniform1f("speed", 0.3);
	vbo.draw(GL_POINTS, positions.size() / 2, positions.size());
	shader.end();
	cam.end();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	glm::vec2 relative = glm::vec2(x, y) - mousePosition;
	float length = sqrt(pow(relative.x, 2) + pow(relative.y, 2));
	if (length > 100) {
		relative = glm::vec2(0, 0);
	}
	//printf("Polling Mouse Movements: \n X: %.2f Y: %.2f LENGTH: %.2f\n", relative.x, relative.y, length);
	mousePosition = glm::vec2(x,y);
	cam.camRotate(relative);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
