#pragma once

#include "ofMain.h"
#include "MyCustomCamera.h"
#include "Beacon.h"
#include "Powerup.h"
#include "EnemyObject.h"

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

	ofConePrimitive cone;
	ofBoxPrimitive box;

	int asteroids;
	float health = PLAYER_MAX_HP;
	// Variable to show winscreen or not
	bool gameWon = false;
	bool gameLost = false;
	int enemyActivation;

	// Variable for third-person camera player position
	glm::vec3 playerPos;

	vector<Beacon> beacons;
	vector<HealthPowerup *> healthPowerups;
	vector<SpeedPowerup *> speedPowerups;
	vector<EnemyObject> enemies;
	vector<ofNode> asteroidVector;

	ofNode body[500];

	// Set up font
	ofTrueTypeFont msgFont;
	ofTrueTypeFont calibri;

	ofTexture mTex;

	MyCustomCamera cam;

	float size = 1.0f;
	const float MAX_SIZE = 2.0f;
	const float MIN_SIZE = 0.5f;
};
