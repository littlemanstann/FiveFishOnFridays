#pragma once

#include "ofxAssimpModelLoader.h"
#include "ofMain.h"
#include "MyCustomCamera.h"
#include "BubbleEmitter.h"
#include "Player.h"
#include "DialogueBox.h"
#include "Beacon.h"
#include "GLFW/glfw3.h"
#include "3DPoint.h"
#include "WaterDroplet.h"

#define PLAYER_RADIUS 1
#define PLAYER_MAX_HP 5
#define SIXTY_FPS 0.016
#define PLAYER_ACCELERATION_FORCE 1
#define POWERUP_BOOST_FORCE 1
#define GROUND_PLANE -200
// This is the config for the voronoi mesh (Jansen)

#define BOUND_X 150
#define BOUND_Y 150
#define BOUND_Z 150
#define DELTA 1
#define NUM_POINTS 15


class ofApp : public ofBaseApp {

public:
	//Default Functions
	void setup();
	void update();
	void draw();
	void renderScene(ofShader * myShader, ofFbo * myFbo);
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


	//Custom Functions
	glm::vec3 sphere_sample();
	glm::vec3 circle_sample();
	void makeScreenQuad();


	//*3D Objects
	ofConePrimitive NPC1;
	MyCustomCamera cam;
	ofPlanePrimitive groundModel;
	ofxAssimpModelLoader skyboxModel;

	//Textures
	ofTexture groundTexture;
	ofTexture skyboxTexture;

	ofTexture blankTexture;

	//Player Information
	Player * player;
	glm::vec3 playerPos;


	//Mouse Positions
	glm::vec2 mousePosition;
	bool mouseLocked;
	int lastMouseX, lastMouseY;
	GLFWwindow * glfwWindow;

	

	// [Shaders] 
	ofShader shader;
	ofShader shaderDepth;
	ofShader shaderFBO;
	ofShader speedShader;

	// [Shader Variables]
	int lines = 1;
	int masking = 1;

	// [FBO's]
	ofFbo fboLighting;
	ofFbo fboLDepth;
	ofMesh quad;

	// [Light Information]
	ofVec3f lightPos;
	ofVec3f lightPosViewSpace;


	// [Particles and Emitters]
	std::unique_ptr<BubbleEmitter> particleEmitter;
	vector<glm::vec3> particlePositions;
	vector<glm::vec3> particleNormals;
	ofShader particleShader;
	ofNode particleNode; // ofNode for using oF transform functions; will apply the node's transform to the particle system
	ofVbo particleVbo; // vertex buffer object to store all the particle data
	int numParticles = 2000;

	// [Fonts]
	ofTrueTypeFont msgFont;
	ofTrueTypeFont calibri;
	ofTexture mTex;


	// [Dialogue System]
	//Make sure that the indexes are same for pos and msgs. i.e fish one's pos is at the same index as fish one's msgs
	
	vector<glm::vec3> posOfInteractableObjs;
	vector<vector<string>> msgsOfInteractableObjs;
	DialogueBox dialogue;

	// [Voronoi Mesh]
	void findIntersectingPlanes();
	std::vector<std::pair<int, int>> choose2(int n);

	vector<std::unique_ptr<Point>> points;

	float t;

	// World Water Droplets
	vector<WaterDroplet> waterDroplets;

	// [Game Over Screen]
	ofTrueTypeFont rockSalt;


};
