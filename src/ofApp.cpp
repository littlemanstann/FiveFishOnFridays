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
	skybox1.set(500);
	skybox1.setPosition(0,0,0);
	skybox2.set(500);
	skybox2.setPosition(505,0,0);

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
	cone1.move(25, 25, -2);

	posOfInteractableObjs.push_back(cone1.getPosition());
	msgsOfInteractableObjs.push_back({ "I am in denial that it is friday.",
		"I am angry that it is friday!",
		"Can I change the fact that it is friday?",
		"I am sad that today is friday...",
		"I accept that today is friday"
		});
	dialogue.setup(100, 400, 600, 150);

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

	// Mouse Lock Setup
	mouseLocked = false; // Start with the mouse unlocked
	ofShowCursor(); // Ensure the cursor is visible initially
	// Get the GLFW window pointer
	ofAppGLFWWindow* win = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr());
	if (win) {
		glfwWindow = win->getGLFWWindow();
	}
	mouseLocked = false;
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
	if (!cam.isTalking) {
		cam.update(SIXTY_FPS);
	}
	dialogue.update();
	if (!dialogue.isActive()) {
		cam.isTalking = false;
	}
	 // 60 fps

	//Jansen: Adding the interact button here. Will iterate over all things it can talk to and if it is close enough to one of them will set talking to true
	if (ofGetKeyPressed('e') && cam.isTalking == false) {
		for (size_t i = 0; i < posOfInteractableObjs.size(); i++) {
			glm::vec3 p = posOfInteractableObjs[i];
			float test = glm::length(cam.myCone.getPosition() - p);
			if (glm::length(cam.myCone.getPosition() - p) < cam.interactRange) {
				//printf("Not a number!");
				cam.isTalking = true;
				cam.betterLookAt(p);
				dialogue.setDialogue(msgsOfInteractableObjs[i]);
			}
		}
			

		//cam.isTalking = true;
	}

	if (emitter) emitter->update(SIXTY_FPS);
	// Set playerPos to be the object in third-person view
	playerPos = cam.getPosition() + ((cam.getqForward() * 10) + glm::vec3(0, 1, 0) * -3);

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


//--------------------------------------------------------------
void ofApp::draw() {

	fbo.begin();
	ofClear(0, 0, 0, 255);
	cam.begin();
	myLight.enable();
	
	ofEnableDepthTest();
	
	ofSetColor(0, 200, 220);
	skybox1.draw();
	ofSetColor(50, 50, 255);
	skybox2.draw();

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

	//Cone you can talk to
	cone1.drawFaces();

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

	if (cam.dash) {
		speedShader.begin();
		speedShader.setUniformTexture("tex0", fbo.getTexture(), 0);
		speedShader.setUniform1f("timer", ofGetElapsedTimef());
		speedShader.setUniform1f("dashIntensity", 0.4);
		// any other uniforms here

		// Draw fullscreen quad
		//fbo.getTexture().bind();
		quad.draw();
		//fbo.getTexture().unbind();

		speedShader.end();

		//fbo.draw(0, 0); // draw outside shader to see the content of the fbo directly
	}
	else {
		fbo.draw(0, 0);
	}

	dialogue.draw();
	
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	dialogue.keyPressed(key);

	// Mouse lock toggle using 'L' key
	if (key == ofKey('l') || key == ofKey('l')) {
		printf("Toggling Mouse Lock\n");
		mouseLocked = !mouseLocked;

		if (mouseLocked) {
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			ofHideCursor();
		}
		else {
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ofShowCursor();
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	// Center of screen coordinates
	float centerX = ofGetWidth() / 2.;
	float centerY = ofGetHeight() / 2.;
	glm::vec2 center = glm::vec2(centerX, centerY);

	glm::vec2 relative = glm::vec2(x, y) - center;
	float length = sqrt(pow(relative.x, 2) + pow(relative.y, 2));

	// Testing for Interpolation; delete fast
	// glm::vec2 lerp = glm::lerp(glm::vec2(0, 0), relative, 0.2f);
	// Lerp between relative and center
	// relativeVec.getInterpolated(center, 0.5);
	//printf("Polling Mouse Movements: \n X: %.2f Y: %.2f LENGTH: %.2f\n", relative.x, relative.y, length);
	
	mousePosition = glm::vec2(x,y);
	cam.camRotate(relative);

	// Mouse Lock Update (Move camera to center if locked)
	if (mouseLocked && glfwWindow) {
		glfwSetCursorPos(glfwWindow, centerX, centerY);
	}
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
