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

	ofFbo::Settings s;
	s.width = ofGetWidth();
	s.height = ofGetHeight();
	s.internalformat = GL_RGBA;     // IMPORTANT: includes alpha channel
	s.useDepth = false;
	s.useStencil = false;

	fboParticle.allocate(s);


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

	//Setup skybox
	skyboxModel.load("models/skybox.glb");
	skyboxModel.enableNormals();
	skyboxModel.setScale(20, -20, -20);
	ofDisableArbTex();
	ofLoadImage(skyboxTexture, "textures/skyboxClouds.png");
	skyboxTexture.generateMipmap();

	//Setup Shader Outside Bubble Depth Map Texture (white image)
	ofLoadImage(blankTexture, "textures/blank.png");

	//Setup Assortment of Models around the world
	tardigrade.load("models/tardigrade.glb");
	tardigrade.enableNormals();
	tardigrade.setScale(0.02, -0.02, -0.02);
	tardigrade.setPosition(6, 4, 0);

	amoebaGreen1.load("models/amoebaGreen1.glb");
	amoebaGreen1.enableNormals();
	amoebaGreen1.setScale(0.02, -0.02, -0.02);
	amoebaGreen1.setPosition(0, 8, 0);

	//mossPatch.load("models/moss");
	//mossPatch.enableNormals();
	//mossPatch.setScale(0.02, -0.02, -0.02);
	//mossPatch.setPosition(0, 0, 6);

	amoebaBlue1.load("models/amoebaGreen1.glb");
	amoebaBlue1.enableNormals();
	amoebaBlue1.setScale(0.02, -0.02, -0.02);
	amoebaBlue1.setPosition(-6, 0, 0);

	amoebaBlue2.load("models/amoebaGreen1.glb");
	amoebaBlue2.enableNormals();
	amoebaBlue2.setScale(0.02, -0.02, -0.02);
	amoebaBlue2.setPosition(0, 0, 6);

	rockPatch.load("models/rocks.glb");
	rockPatch.enableNormals();
	rockPatch.setScale(0.02, -0.02, -0.02);
	rockPatch.setPosition(0, -4, 0);

	//Setup Interactable NPC
	fish1 = new Player();
	fish1->setPosition(glm::vec3(25, 45, -30));
	posOfInteractableObjs.push_back(glm::vec3(25, 45, -30));
	msgsOfInteractableObjs.push_back({ "Where did you come from?",
		"Was it Friday there, too?"});

	fish2 = new Player();
	fish2->setPosition(glm::vec3(80, 0, 60));
	posOfInteractableObjs.push_back(glm::vec3(80, 0, 60));
	msgsOfInteractableObjs.push_back({ "love Friday!!",
		"Friday is the best day of the year!!!!!",
		"As long as it's Friday, we will live forever!",
		});

	fish3 = new Player();
	fish3->setPosition(glm::vec3(12, 66, 32));
	posOfInteractableObjs.push_back(glm::vec3(12, 66, 32));
	msgsOfInteractableObjs.push_back({ "I miss ma wife...",
		});



	NPC1.set(1, 2);
	NPC1.move(25, 25, -2);
	posOfInteractableObjs.push_back(NPC1.getPosition());
	msgsOfInteractableObjs.push_back({ "I am in denial that it is friday.",
		"I am angry that it is friday!",
		"Can I change the fact that it is friday?",
		"I am sad that today is friday...",
		"I accept that today is friday" });
	dialogue.setup(100, 400, 600, 150);


	rockSalt.load("RockSalt-Regular.ttf", 64, true, true);
	calibri.setLineHeight(18.0f);
	calibri.setLetterSpacing(1.037);


	//Setup Emitter
	patricleEmitter1 = std::make_unique<BubbleEmitter>(cam);
	patricleEmitter1->setPosition(glm::vec3(500, 150, -255));
	patricleEmitter1->setSize(5); // cube size

	patricleEmitter2 = std::make_unique<BubbleEmitter>(cam);
	patricleEmitter2->setPosition(glm::vec3(525, 150, -250));
	patricleEmitter2->setSize(5); // cube size

	patricleEmitter3 = std::make_unique<BubbleEmitter>(cam);
	patricleEmitter3->setPosition(glm::vec3(490, 165, -240));
	patricleEmitter3->setSize(5); // cube size

	patricleEmitter4 = std::make_unique<BubbleEmitter>(cam);
	patricleEmitter4->setPosition(glm::vec3(500, 150, -265));
	patricleEmitter4->setSize(5); // cube size

	patricleEmitter5 = std::make_unique<BubbleEmitter>(cam);
	patricleEmitter5->setPosition(glm::vec3(500, 165, -260));
	patricleEmitter5->setSize(5); // cube size

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

	//the 20 bubbles in a line
	glm::vec3 tempCol = glm::vec3(0.24, 0.37, 0.56);


	//1-5
	tempCol = glm::vec3(0.02, 0.49, 0.72);
	waterDroplets.push_back(WaterDroplet(27.9/2, glm::vec3(633, -7, -213), tempCol));
	tempCol = glm::vec3(0.028, 0.486, 0.642);
	waterDroplets.push_back(WaterDroplet(33.0/2, glm::vec3(598, 4.9, -110.96), tempCol));
	tempCol = glm::vec3(0.037, 0.482, 0.575);
	waterDroplets.push_back(WaterDroplet(45.16 / 2, glm::vec3(550, 15.3, -16), tempCol));
	tempCol = glm::vec3(0.047, 0.478, 0.511);
	waterDroplets.push_back(WaterDroplet(54 / 2, glm::vec3(454, 42, 65), tempCol));
	tempCol = glm::vec3(0.06, 0.473, 0.448);
	waterDroplets.push_back(WaterDroplet(54 / 2, glm::vec3(339, 87, 46), tempCol));

	//2-10
	tempCol = glm::vec3(0.078, 0.468, 0.378);
	waterDroplets.push_back(WaterDroplet(54 / 2, glm::vec3(265, 124, -61), tempCol));
	tempCol = glm::vec3(0.095, 0.464, 0.329);
	waterDroplets.push_back(WaterDroplet(39 / 2, glm::vec3(305, 103, -179), tempCol));
	tempCol = glm::vec3(0.115, 0.459, 0.275);
	waterDroplets.push_back(WaterDroplet(33 / 2, glm::vec3(365, 64, -290), tempCol));
	tempCol = glm::vec3(0.14, 0.455, 0.225);
	waterDroplets.push_back(WaterDroplet(30 / 2, glm::vec3(394, 25, -401), tempCol));
	tempCol = glm::vec3(0.17, 0.449, 0.179);
	waterDroplets.push_back(WaterDroplet(38 / 2, glm::vec3(341, 0, -515), tempCol));

	//11 - 15
	tempCol = glm::vec3(0.262, 0.421, 0.193);
	waterDroplets.push_back(WaterDroplet(38 / 2, glm::vec3(202, 15, -590), tempCol));
	tempCol = glm::vec3(0.25, 0.40, 0.311);
	waterDroplets.push_back(WaterDroplet(38 / 2, glm::vec3(85, 2, -575), tempCol));
	tempCol = glm::vec3(0.24, 0.37, 0.56);
	waterDroplets.push_back(WaterDroplet(38 / 2, glm::vec3(-34, -41, -505), tempCol));
	tempCol = glm::vec3(0.513, 0.367, 0.23);
	waterDroplets.push_back(WaterDroplet(38 / 2, glm::vec3(-133, -48, -414), tempCol));
	tempCol = glm::vec3(0.642, 0.346, 0.246);
	waterDroplets.push_back(WaterDroplet(44 / 2, glm::vec3(-211, -19, -302), tempCol));

	//16 - 20
	tempCol = glm::vec3(0.691, 0.388, 0.371);
	waterDroplets.push_back(WaterDroplet(55 / 2, glm::vec3(-280, 51, -124), tempCol));
	tempCol = glm::vec3(0.753, 0.442, 0.56);
	waterDroplets.push_back(WaterDroplet(50 / 2, glm::vec3(-287, 120, 33), tempCol));
	tempCol = glm::vec3(0.807, 0.49, 0.751);
	waterDroplets.push_back(WaterDroplet(42 / 2, glm::vec3(-223, 173, 163), tempCol));
	tempCol = glm::vec3(0.69, 0.486, 0.768);
	waterDroplets.push_back(WaterDroplet(30 / 2, glm::vec3(-163, 254, 191), tempCol));
	tempCol = glm::vec3(0.566, 0.491, 0.788);
	waterDroplets.push_back(WaterDroplet(27 / 2, glm::vec3(-102, 332, 172), tempCol));

	// Setup Seaweed Ball stuff
	weed = std::make_unique<SeaweedBall>(cam);
	weed->setPosition(glm::vec3(10, 10, 0));
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

	if (patricleEmitter1) patricleEmitter1->update(ofGetLastFrameTime());
	if (patricleEmitter2) patricleEmitter2->update(ofGetLastFrameTime());
	if (patricleEmitter3) patricleEmitter3->update(ofGetLastFrameTime());
	if (patricleEmitter4) patricleEmitter4->update(ofGetLastFrameTime());
	if (patricleEmitter5) patricleEmitter5->update(ofGetLastFrameTime());

	skyboxModel.setPosition(cam.getPlayerPosition().x, cam.getPlayerPosition().y, cam.getPlayerPosition().z);

	/// Update collision with water droplets
	cam.setGravity(true); // Reset gravity each frame

	for (auto& droplet : waterDroplets) {
		if (droplet.isColliding(cam.getPlayerPosition()))
			cam.setGravity(false);
	}

	fish1->update(30);
	fish2->update(30);
	fish3->update(30);
}

//--------------------------------------------------------------
void ofApp::draw() {

	// Draw Particle Systems
	fboParticle.begin();
	ofClear(0, 0, 0, 0);
	cam.begin();
	patricleEmitter1->draw();
	patricleEmitter2->draw();
	patricleEmitter3->draw();
	patricleEmitter4->draw();
	patricleEmitter5->draw();
	weed->draw();
	cam.end();
	fboParticle.end();


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
	fboParticle.draw(0, 0);
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

	//Game Over Screen
	if (cam.isDead) {
		ofSetColor(0, 0, 0);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		
		ofSetColor(255, 255, 255);
		string text = "GAME OVER";

		ofRectangle bbox = rockSalt.getStringBoundingBox(text, 0, 0);

		float x = (ofGetWidth() - bbox.getWidth()) * 0.5;
		//float y = (ofGetHeight() - bbox.getHeight()) * 0.5 + bbox.getHeight();

		rockSalt.drawString(text, x, ofGetHeight()/2 - 64);
		if (!dialogue.isActive()) {
			cam.isTalking = true;
			dialogue.setDialogue({ "You were so close yet so far...", "All this struggle and it is still Friday. You can't stop now can you?" , "Press 'Space' to continue" }, [](MyCustomCamera* cam) {printf("callback");}, &cam);
		}
		else {
			if (dialogue.isComplete()) {
				
			}
		}
		
		
	}
	//ofDisableDepthTest();
	dialogue.draw();
	
	//ofEnableDepthTest();
	
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
	myShader->setUniform1f("opacity", 1.0f);

	//Draw Skybox
	ofDisableDepthTest();
	myShader->setUniformMatrix4f("worldMatrix", skyboxModel.getModelMatrix());
	myShader->setUniform1i("texBool", 1);
	myShader->setUniform1i("brightBool", 1);
	skyboxTexture.bind();
	skyboxModel.getMesh(0).draw();
	skyboxTexture.unbind();
	ofEnableDepthTest();

	//Draw Environmental Objects
	myShader->setUniformMatrix4f("worldMatrix", tardigrade.getModelMatrix());
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);
	for (int i = 0; i < tardigrade.getNumMeshes(); i++) {
		if(i % 2 == 0) {
			myShader->setUniform3f("objectColor", glm::vec3(0.97, 0.4, 0.98));
		}
		else{
			myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.2, 0.8));
		}
		tardigrade.getMesh(i).draw();
	}

	myShader->setUniformMatrix4f("worldMatrix", amoebaGreen1.getModelMatrix());
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);
	for (int i = 0; i < amoebaGreen1.getNumMeshes(); i++) {
		myShader->setUniform3f("objectColor", glm::vec3(0.25, 0.8 + 0.2 * i, 0.4));
		amoebaGreen1.getMesh(i).draw();
	}

	//myShader->setUniformMatrix4f("worldMatrix", mossPatch.getModelMatrix());
	//myShader->setUniform3f("objectColor", glm::vec3(0.8, 0.3, 2.0));
	//myShader->setUniform1i("texBool", 0);
	//myShader->setUniform1i("brightBool", 0);
	//for (int i = 0; i < mossPatch.getNumMeshes(); i++) {
	//	mossPatch.getMesh(i).draw();
	//}

	myShader->setUniformMatrix4f("worldMatrix", amoebaBlue1.getModelMatrix());
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);
	for (int i = 0; i < amoebaBlue1.getNumMeshes(); i++) {
		myShader->setUniform3f("objectColor", glm::vec3(0.25, 0.6, 0.8 + 0.2*i));
		amoebaBlue1.getMesh(i).draw();
	}

	myShader->setUniformMatrix4f("worldMatrix", amoebaBlue2.getModelMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.8, 0.3, 2.0));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);
	for (int i = 0; i < amoebaBlue2.getNumMeshes(); i++) {
		myShader->setUniform3f("objectColor", glm::vec3(0.25, 0.3, 0.8 + 0.2 * i));
		amoebaBlue2.getMesh(i).draw();
	}

	myShader->setUniformMatrix4f("worldMatrix", rockPatch.getModelMatrix());
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 0);
	for (int i = 0; i < rockPatch.getNumMeshes(); i++) {
		myShader->setUniform3f("objectColor", glm::vec3(i * 0.08 + 0.5, i * 0.08 + 0.5, i * 0.08 + 0.5));
		rockPatch.getMesh(i).draw();
	}


	//Draw Player
	myShader->setUniformMatrix4f("worldMatrix", cam.getMyGlobalTransformMatrix());
	myShader->setUniform1i("texBool", 1);

	cam.drawMeShaded(myShader);

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

	// Draw Fish NPCs
	// Move Fish NPCs
	fish1->draw(myShader);
	fish2->draw(myShader);
	fish3->draw(myShader);

	for (auto & p : points) {
		p->draw(myShader, cam.myCone.getGlobalPosition());
	}

	
	


	// Make water droplets semi-transparent
	glDepthMask(GL_FALSE);
	ofEnableAlphaBlending();
	myShader->setUniform1f("opacity", 0.5f);
	//// Draw World Water Droplets
	for (auto& droplet : waterDroplets) {
		droplet.draw(myShader);
	}
	myShader->setUniform1f("opacity", 1.0f);
	ofDisableAlphaBlending();
	glDepthMask(GL_TRUE);

	
	//Draw Bubble Shader (draw last), it breaks it
	myShader->setUniformMatrix4f("worldMatrix", patricleEmitter1->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 1);
		

	/*myShader->setUniformMatrix4f("worldMatrix", patricleEmitter2->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 1);
	patricleEmitter2->draw();

	myShader->setUniformMatrix4f("worldMatrix", patricleEmitter3->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 1);
	patricleEmitter3->draw();

	myShader->setUniformMatrix4f("worldMatrix", patricleEmitter4->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 1);
	patricleEmitter4->draw();

	myShader->setUniformMatrix4f("worldMatrix", patricleEmitter5->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	myShader->setUniform1i("brightBool", 1);
	*/

	

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

