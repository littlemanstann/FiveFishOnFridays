#pragma once

#include "ofxAssimpModelLoader.h"
#include "ofMain.h"
#include "MyCustomCamera.h"
#include "BubbleEmitter.h"
#include "MyCustomCamera.h"
#include "Player.h"
#include "DialogueBox.h"

#define PLAYER_RADIUS 1
#define PLAYER_MAX_HP 5
#define SIXTY_FPS 0.016
#define PLAYER_ACCELERATION_FORCE 1
#define POWERUP_BOOST_FORCE 1

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofBoxPrimitive box;


	ofConePrimitive cone1;
	ofBoxPrimitive box2;
	ofConePrimitive cone2;
	ofBoxPrimitive box1;

	glm::vec3 sphere_sample();
	glm::vec3 circle_sample();

	int asteroids;
	float health = PLAYER_MAX_HP;
	// Variable to show winscreen or not
	bool gameWon = false;
	bool gameLost = false;
	int enemyActivation;

	// Variable for third-person camera player position
	glm::vec3 playerPos;


	ofNode body[500];

	// Set up font
	ofTrueTypeFont msgFont;
	ofTrueTypeFont calibri;

	ofTexture mTex;

	MyCustomCamera cam;

	float size = 1.0f;
	const float MAX_SIZE = 2.0f;
	const float MIN_SIZE = 0.5f;

	ofLight myLight;

	ofxAssimpModelLoader modelPlayer;

	ofxAssimpModelLoader modelFox;

	Player * player;

	glm::vec2 mousePosition;

	std::unique_ptr<BubbleEmitter> emitter;

	void makeScreenQuad();


	ofFbo fbo;          // framebuffer for first pass
	ofShader speedShader;    // postprocessing shader
	ofMesh quad;

	//For the dialogue system

	//Make sure that the indexes are same for pos and msgs. i.e fish one's pos is at the same index as fish one's msgs
	vector<glm::vec3> posOfInteractableObjs;
	vector<vector<string>> msgsOfInteractableObjs;
	DialogueBox dialogue;
};
