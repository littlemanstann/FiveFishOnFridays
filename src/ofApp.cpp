#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//  ------------------------------------------------------------------
	ofSetWindowTitle("Final Project - Five Fish On Fridays - 3501");
	ofSetBackgroundColor(0, 0, 0);


	//Setup Flags and Timers
	t = 0;


	//Setup Shaders
	shader.load("shader/lighting");
	shaderFBO.load("shader/mix.vert", "shader/mix.frag");
	shaderDepth.load("shader/depth");
	speedShader.load("shader/screen_space_quad.vert", "shader/screen_space_quad.frag");


	//Setup FrameBufferObject
	ofFbo::Settings settings;
	settings.width = ofGetWidth();
	settings.height = ofGetHeight();
	settings.internalformat = GL_RGBA;
	settings.textureTarget = GL_TEXTURE_2D; // now can pull the texture out and treat as a regular texture
	settings.useDepth = TRUE;
	fboLighting.allocate(settings);
	fboLDepth.allocate(settings);
	makeScreenQuad(); //setting up the quad that will be rendered in the shaderFBO pass


	//Setup the GLFW Window that contains functions to hide and recenter the mouse
	ofAppGLFWWindow * win = dynamic_cast<ofAppGLFWWindow *>(ofGetWindowPtr());
	if (win) {
		glfwWindow = win->getGLFWWindow();
	}

	//Setup a primitive with a texture (example)
	groundModel.set(1200, 1200);
	groundModel.setPosition(glm::vec3(0, GROUND_PLANE, 0));
	groundModel.rotateDeg(90, glm::vec3(1, 0, 0));
	ofDisableArbTex();
	ofLoadImage(groundTexture, "textures/sand.jpg");
	groundTexture.generateMipmap();

	skyboxModel.load("models/skybox.glb");
	skyboxModel.enableNormals();
	skyboxModel.setScale(20, -20, -20);
	ofDisableArbTex();
	ofLoadImage(skyboxTexture, "textures/skyboxClouds.png");
	skyboxTexture.generateMipmap();

	ofLoadImage(blankTexture, "textures/blank.png");

	//Setup Interactable NPC

	NPC1.set(1, 2);
	NPC1.move(25, 25, -2);
	posOfInteractableObjs.push_back(NPC1.getPosition());
	msgsOfInteractableObjs.push_back({ "I am in denial that it is friday.",
		"I am angry that it is friday!",
		"Can I change the fact that it is friday?",
		"I am sad that today is friday...",
		"I accept that today is friday" });
	dialogue.setup(100, 400, 600, 150);


	//Setup Emitter
	particleEmitter = std::make_unique<BubbleEmitter>(cam);
	particleEmitter->setPosition(glm::vec3(0, 0, 0));
	particleEmitter->setSize(1); // cube size

	// Setup voronoi mesh

	for (int i = 0; i < NUM_POINTS; i++) {
		std::unique_ptr<Point> p;

		p = std::make_unique<ThreeDPoint>();



		p->setup(1, ofColor::fromHsb(ofRandom(255), 200, 255));

		p->setPosition(ofRandom(-BOUND_X, BOUND_X), ofRandom(-BOUND_Y, BOUND_Y), ofRandom(-BOUND_Z, BOUND_Z));
		points.push_back(std::move(p));

	}

	findIntersectingPlanes();

	for (auto& p : points) {
		ThreeDPoint* t = dynamic_cast<ThreeDPoint*>(p.get());
		t->cylinders = t->createWireframeCylinders(t->vertices);
	}

	// SETUP: water droplets
	waterDroplets.push_back(WaterDroplet(200.0f, glm::vec3(0., 0, 0.), glm::vec3(0.24, 0.37, 0.56)));
	waterDroplets.push_back(WaterDroplet(125.0f, glm::vec3(500., 250., -255.), glm::vec3(0.345, 0.635, 0.647)));
}

//--------------------------------------------------------------
void ofApp::update() {
	t += 0.005;

	// calculate the light position's transformation in the update function
	lightPos = ofVec3f(600 * cos(t * 2), 1, 600 * sin(t * 2));
	lightPosViewSpace = ofVec3f(cam.getModelViewMatrix() * ofVec4f(lightPos.x, lightPos.y, lightPos.z, 1.0f));

	if (!cam.isTalking) {
		cam.update(ofGetLastFrameTime());
	}
	dialogue.update();
	if (!dialogue.isActive()) {
		cam.isTalking = false;
		
	}

	if (ofGetKeyPressed('e') && cam.isTalking == false) {
		for (size_t i = 0; i < posOfInteractableObjs.size(); i++) {
			glm::vec3 p = posOfInteractableObjs[i];
			float test = glm::length(cam.myCone.getPosition() - p);
			if (glm::length(cam.myCone.getPosition() - p) < cam.interactRange) {
				cam.isTalking = true;
				cam.betterLookAt(p);
				dialogue.setDialogue(msgsOfInteractableObjs[i]);
			}
		}
	}

	if (particleEmitter) particleEmitter->update(ofGetLastFrameTime());

	skyboxModel.setPosition(cam.getPlayerPosition().x, cam.getPlayerPosition().y, cam.getPlayerPosition().z);

	/// Update collision with water droplets
	cam.setGravity(true); // Reset gravity each frame

	for (auto& droplet : waterDroplets) {
		if (droplet.isColliding(cam.getPlayerPosition()))
			cam.setGravity(false);
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	// ------------ Lighting pass: Save the lighting pass to "fboLighting" -===========
	
	renderScene(&shader, &fboLighting);

	// ------------ Depth pass: Save the depth pass to "fboLDepth" -===========
	renderScene(&shaderDepth, &fboLDepth);
	

	// ---------- Second pass: Draw the scene with an additional shader if you are dashing --==========


	//when you exit the shpere you should be able to see clearly so we switch the depth texture to blank
	//We need to check all water spheres to see if the player is inside any of them
	//Each water sphere can be a different colour so we need to set the water colour to the colour of the sphere

	ofTexture tempShaderTexture;
	bool inWater = false;
	glm::vec3 waterColor = glm::vec3(0.0);


	for (int i = 0; i < waterDroplets.size(); i++) {
		if (glm::length(cam.getPlayerPosition() - waterDroplets[i].getPosition()) < waterDroplets[i].getRadius()) {
			inWater = true;
			waterColor = waterDroplets[i].getColor();
		}
	}

	if (inWater) { 
		tempShaderTexture = fboLDepth.getTexture();
	} else {
		tempShaderTexture = blankTexture;
	}

	//dashing shader
	if (cam.dash) {
		speedShader.begin();
		speedShader.setUniformTexture("tex0", fboLighting.getTexture(), 0);
		speedShader.setUniformTexture("tex1", tempShaderTexture, 1);
		speedShader.setUniform1f("timer", ofGetElapsedTimef());
		speedShader.setUniform1f("dashIntensity", 0.1);
		speedShader.setUniform3f("waterColor", waterColor);
		quad.draw();
		speedShader.end();
	} else {
		shaderFBO.begin();
		shaderFBO.setUniformTexture("tex0", fboLighting.getTexture(), 0);
		shaderFBO.setUniformTexture("tex1", tempShaderTexture, 1);
		shaderFBO.setUniform3f("waterColor", waterColor);

		
		quad.draw();
		shaderFBO.end();
	}

	dialogue.draw();

	//Game Over Screen
	if (cam.isDead) {
		for (int i = 0; i < quad.getNumVertices(); i++) {
			quad.setColor(i, ofFloatColor(1.0, 0.0, 0.0, 1.0)); // red
		}
		quad.draw();
	}
	
}

void ofApp::renderScene(ofShader * myShader, ofFbo * myFbo) {
	myFbo->begin();
	ofClear(0, 0, 0, 255);
	ofSetColor(ofColor::white);

	cam.begin();
	myShader->begin();

	myShader->setUniform1f("time", ofGetElapsedTimef());
	myShader->setUniform4f("globalColor", ofColor::blanchedAlmond);
	myShader->setUniformMatrix4f("viewMatrix", cam.getModelViewMatrix());
	myShader->setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());
	myShader->setUniformMatrix4f("projectionMatrix", cam.getProjectionMatrix());
	myShader->setUniform1f("specularPower", 16.0f);
	myShader->setUniform3f("lightPos", lightPosViewSpace);
	myShader->setUniform3f("viewPos", cam.getPosition());
	myShader->setUniform3f("lightColor", glm::vec3(1, 1, 1));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);

	//Draw Skybox
	ofDisableDepthTest();
	myShader->setUniformMatrix4f("worldMatrix", skyboxModel.getModelMatrix());
	myShader->setUniform1i("texBool", 1);
	myShader->setUniform1i("brightBool", 1);
	skyboxTexture.bind();
	skyboxModel.getMesh(0).draw();
	skyboxTexture.unbind();
	ofEnableDepthTest();

	//Draw Player
	myShader->setUniformMatrix4f("worldMatrix", cam.getMyGlobalTransformMatrix());
	myShader->setUniform1i("texBool", 1);
	cam.drawMeShaded();

	//Draw Cone (npc)
	myShader->setUniformMatrix4f("worldMatrix", NPC1.getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	NPC1.draw();
	/*
	//Draw Ground, if you want to add a texture, you must bind and unbind it around the draw.
	myShader->setUniformMatrix4f("worldMatrix", groundModel.getGlobalTransformMatrix());
	myShader->setUniform1i("texBool", 1);
	myShader->setUniform1i("brightBool", 0);
	groundTexture.bind();
	groundModel.draw();
	groundTexture.unbind();
	*/

	

	//// Draw World Water Droplets
	for (auto & droplet : waterDroplets) {
		droplet.draw(myShader);
	}

	for (auto & p : points) {
		p->draw(myShader, cam.myCone.getGlobalPosition());
	}

	//Draw Bubble Shader (draw last), it 
	myShader->setUniformMatrix4f("worldMatrix", particleEmitter->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 1);
	particleEmitter->draw();	



	myShader->end();

	cam.end();

	myFbo->end();

	ofDisableDepthTest();

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	dialogue.keyPressed(key);

	// Mouse lock toggle using 'L' key
	if (key == ofKey('l') || key == ofKey('L')) {
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
	
	mousePosition = glm::vec2(x,y);
	if (!cam.isTalking) {
	cam.camRotate(relative);
	}

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

glm::vec3 ofApp::sphere_sample() {
	// random sample from within a sphere
	glm::vec3 p;

	// use simple rejection sampling
	do {
		p = glm::vec3(
			ofRandom(-1.0f, 1.0f),
			ofRandom(-1.0f, 1.0f),
			ofRandom(-1.0f, 1.0f));
	} while (glm::length2(p) > 1.0f); // is length^2 > 1? Failed, try again

	// out of the loop, must have found a legal sample
	return p;
}

glm::vec3 ofApp::circle_sample() {
	// random sample from within a sphere
	glm::vec3 p;

	// use simple rejection sampling
	do {
		p = glm::vec3(
			ofRandom(-1.0f, 1.0f),
			0,
			ofRandom(-1.0f, 1.0f));
	} while (glm::length2(p) > 1.0f); // is length^2 > 1? Failed, try again

	// out of the loop, must have found a legal sample
	return p;
}

void ofApp::makeScreenQuad() {

	// Build a fullscreen quad with texture coordinates
	quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP); // fan can also work

	// bottom-left
	quad.addVertex(glm::vec3(0, ofGetHeight(), 0));
	quad.addTexCoord(glm::vec2(0.0, 0.0));

	// top-left
	quad.addVertex(glm::vec3(0, 0, 0));
	quad.addTexCoord(glm::vec2(0.0, 1.0));

	// bottom-right
	quad.addVertex(glm::vec3(ofGetWidth(), ofGetHeight(), 0));
	quad.addTexCoord(glm::vec2(1.0, 0.0));

	// top-right
	quad.addVertex(glm::vec3(ofGetWidth(), 0, 0));
	quad.addTexCoord(glm::vec2(1.0, 1.0));
}

void ofApp::findIntersectingPlanes() {
	for (auto& p1 : points) {
		for (auto& p2 : points) {
			if (p1 == p2) continue;

			//get the equation of the bisecting plane given the normal and a point on that plane.
			glm::vec3 P1P2 = p2->position - p1->position;
			glm::vec3 point = p1->position + P1P2 / 2;
			glm::vec3 normal = glm::normalize(P1P2);
			float d = -(normal.x * point.x + normal.y * point.y + normal.z * point.z);
			glm::vec4 BisectingPlane = glm::vec4(normal.x, normal.y, normal.z, d);
			//std::cout << "Bisecting plane found between points: " << p1->position << " and " << p2->position << "|| Bisecting plane: " << BisectingPlane << "\n";

			//find if that plane intersects one of the edges.
			//topologically you cannot intersect one edge of a convex polygon without intersecting the entire polygon. So only need to find one intersection
			ThreeDPoint* p = dynamic_cast<ThreeDPoint*>(p1.get());
			vector<std::pair<int, int>> allEdges = choose2(p->vertices.size());
			for (std::pair<int, int> pair : allEdges) {
				glm::vec3 edgeDirVector = glm::normalize(p->vertices[pair.second]->position - p->vertices[pair.first]->position);

				//taking the dot product of the plane's normal and the edge's direction vector will determine if this edge is parrallel to the plane.
				if (glm::dot(edgeDirVector, normal) != 0) {
					//their dot product is not 0 they intersect at one point.
					// Need to check if that point is within the bounds of the line segment given by the endpoints vertices[pair first and second]

					float t = -(BisectingPlane.x * p1->position.x + BisectingPlane.y * p1->position.y + BisectingPlane.z * p1->position.z + BisectingPlane.w)
						/ (BisectingPlane.x * edgeDirVector.x + BisectingPlane.y * edgeDirVector.y + BisectingPlane.z * edgeDirVector.z);

					//If the POI of the line and point is not in the line segment then ignore this pairing
					float test = glm::length((p->vertices[pair.second]->position - p->vertices[pair.first]->position));
					if (t < 0 || t > test) {
						continue;
					}
					//otherwise the plane does cut into the polyhedron
					else {
						p->vertices = p->cutPolyhedron(BisectingPlane);
						break;
					}
				}
			}
		}
	}
}

//helper function for selecting all pairs of vertices to form edges
std::vector<std::pair<int, int>> ofApp::choose2(int n) {
	std::vector<std::pair<int, int>> pairs;
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			pairs.emplace_back(i, j);
		}
	}
	return pairs;
}

