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
	//fbo.allocate(settings);
	//fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
	ofFbo::Settings settings;
	settings.width = ofGetWidth();
	settings.height = ofGetHeight();
	settings.internalformat = GL_RGBA;
	settings.useDepth = true;  // THIS IS CRITICAL!
	settings.depthStencilAsTexture = false;
	settings.textureTarget = GL_TEXTURE_2D;
	settings.numSamples = 4;  // Optional: anti-aliasing
	fbo.allocate(settings);

	speedShader.load("shader/screen_space_quad.vert", "shader/screen_space_quad.frag");

	if (!speedShader.isLoaded()) {
		ofLogError() << "Spped Shader failed to load!";
		ofExit();
	}

	ofSetSphereResolution(50);

	ofTrueTypeFont::setGlobalDpi(72);

	ofDisableArbTex();
	//ofLoadImage(mTex, "earth.jpg");
	/*generateMipmap();
	mTex.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);*/

	//PLAYER
	//cone.set(1, 3);

	modelPlayer.load("models/FISH.glb");
	modelPlayer.enableNormals();

	player = new Player();

	modelFox.loadModel("models/Fox_05.fbx");
	mousePosition = glm::vec2(0.0,0.0);

	// Camera setup

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

	// Set up font
	msgFont.loadFont("trixiepro_heavy.otf", 128);

	makeScreenQuad();


	//fbo.getTexture().setTextureWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	ofEnableAlphaBlending();
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


	cam.update(SIXTY_FPS); // 60 fps

	if (emitter) emitter->update(SIXTY_FPS);
	// Set playerPos to be the object in third-person view
	playerPos = cam.getPosition() + ((cam.getqForward() * 10) + glm::vec3(0, 1, 0) * -3);

	
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
	/// PLAYER ANIMATION UPDATE
	player->update(30);

}

void ofApp::makeScreenQuad() {

	// Build a fullscreen quad with texture coordinates
	quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP); // fan can also work

	// bottom-left
	quad.addVertex(glm::vec3(0, ofGetHeight(), 0));
	quad.addTexCoord(glm::vec2(0.0, 1.0));

	// top-left
	quad.addVertex(glm::vec3(0, 0, 0));
	quad.addTexCoord(glm::vec2(0.0, 0.0));

	// bottom-right
	quad.addVertex(glm::vec3(ofGetWidth(), ofGetHeight(), 0));
	quad.addTexCoord(glm::vec2(1.0, 1.0));

	// top-right
	quad.addVertex(glm::vec3(ofGetWidth(), 0, 0));
	quad.addTexCoord(glm::vec2(1.0, 0.0));
}

/*	//// Crosshairs (Yansen Inplementation)
	float sx = ofGetWidth() / 2.0f;
	float sy = ofGetHeight() / 2.0f;

	float w = 15;
	float h = 2;

	ofDrawRectangle(sx - w / 2, sy - h / 2, w, h);
	w = 2;
	h = 15;

	////Speed and Health UI

	ofDrawRectangle(sx - w / 2, sy - h / 2, w, h);


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
*/

//--------------------------------------------------------------
void ofApp::draw() {

	fbo.begin();
	ofClear(0, 0, 0, 255);
	cam.begin();
	myLight.enable();
	
	ofEnableDepthTest();


	ofSetColor(255);


	mTex.bind();
	////modelPlayer.drawFaces();
	cam.drawMe();
	player->draw();
	mTex.unbind(); 
	
	//-------------------------------------------------------------
	//Environment, while coding the movement I needed a simple ground plane

	//Floor
	ofPushMatrix();
	ofRotateXDeg(90);
	ofSetColor(42, 50, 59);
	ofDrawPlane(0, 0, 0, 500, 500);
	ofPopMatrix();

	//Tree Simple
	glm::vec3 treePos(200, 0, -180);
	ofPushMatrix();
	ofSetColor(82, 37, 7);
	ofTranslate(treePos.x, 30, treePos.z);
	ofDrawCylinder(15, 60);
	ofPopMatrix();

	for (int i = 0; i < 5; i++) {
		ofPushMatrix();
		ofTranslate(treePos.x, (35 + i * 15) * 3, treePos.z);
		ofRotateX(180);
		ofScale(100 - i * 6);
		ofSetColor(18, 115, 44);
		ofDrawCone(0.6 - i * 0.1, 1);
		ofPopMatrix();
	}

	///-----END DRAWING OF 3D OBJECTS, ONLY UI ELEMENTS AND TEXT BEYOND THIS POINT-----
	ofDisableDepthTest(); //With this disabled, UI (below) no longer gets clipped in 3D space
	myLight.disable();
	cam.end();
	fbo.end();

	



	// ---------- Second pass: draw FBO with shader --==========


	speedShader.begin();
	speedShader.setUniformTexture("tex0", fbo.getTexture(), 0);
	speedShader.setUniform1f("timer", ofGetElapsedTimef());
	speedShader.setUniform1f("dashIntensity", 0.1);
	// any other uniforms here

	// Draw fullscreen quad
	//fbo.getTexture().bind();
	quad.draw();
	//fbo.getTexture().unbind();

	speedShader.end();

	//fbo.draw(0, 0); // draw outside shader to see the content of the fbo directly
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
