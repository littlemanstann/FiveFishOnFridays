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
	groundModel.setPosition(glm::vec3(0, -10, 0));
	groundModel.rotateDeg(90, glm::vec3(1, 0, 0));
	ofDisableArbTex();
	ofLoadImage(groundTexture, "textures/sand.jpg");
	groundTexture.generateMipmap();


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

	// ------------------------------------------------------------------------------------------
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


}

//--------------------------------------------------------------
void ofApp::draw() {

	// ------------ Lighting pass: Save the lighting pass to "fboLighting" -===========
	ofEnableDepthTest();
	renderScene(&shader, &fboLighting);

	// ------------ Depth pass: Save the depth pass to "fboLDepth" -===========
	renderScene(&shaderDepth, &fboLDepth);
	ofDisableDepthTest();

	// ---------- Second pass: Draw the scene with an additional shader if you are dashing --==========
	if (cam.dash) {
		speedShader.begin();
		speedShader.setUniformTexture("tex0", fboLighting.getTexture(), 0);
		speedShader.setUniformTexture("tex1", fboLDepth.getTexture(), 1);
		speedShader.setUniform1f("timer", ofGetElapsedTimef());
		speedShader.setUniform1f("dashIntensity", 0.1);
		quad.draw();
		speedShader.end();
	} else {
		shaderFBO.begin();
		shaderFBO.setUniformTexture("tex0", fboLighting.getTexture(), 0);
		shaderFBO.setUniformTexture("tex1", fboLDepth.getTexture(), 1);
		quad.draw();
		shaderFBO.end();
	}

	dialogue.draw();
	
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

	//Draw Player
	myShader->setUniformMatrix4f("worldMatrix", cam.getMyGlobalTransformMatrix());
	myShader->setUniform1i("texBool", 1);
	cam.drawMeShaded();

	//Draw Cone (npc)
	myShader->setUniformMatrix4f("worldMatrix", NPC1.getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	NPC1.draw();

	//Draw Ground, if you want to add a texture, you must bind and unbind it around the draw.
	myShader->setUniformMatrix4f("worldMatrix", groundModel.getGlobalTransformMatrix());
	myShader->setUniform1i("texBool", 1);
	groundTexture.bind();
	groundModel.draw();
	groundTexture.unbind();

	//Draw Bubble Shader
	myShader->setUniformMatrix4f("worldMatrix", particleEmitter->getBox().getGlobalTransformMatrix());
	myShader->setUniform3f("objectColor", glm::vec3(0.5, 0.08, 0.90));
	myShader->setUniform1i("texBool", 0);
	particleEmitter->draw();	

	myShader->end();


	cam.end();

	myFbo->end();

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

