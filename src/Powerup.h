#pragma once
#include "ofMain.h"
#include "MeshObject.h"

class Powerup : public MeshObject {

	public:
		Powerup(float r, glm::vec3 pos);
		float getRadius() { return radius; }
		void setDead() { dead = true; };
		bool getDead() { return dead; }
	protected:
		float radius;
		glm::vec3 position;
		bool dead;


};

class HealthPowerup : public Powerup {
public:
	HealthPowerup(float r, glm::vec3 pos, float hp);
	float getHealth() { return health; }

protected:
	float health;
};

class SpeedPowerup : public Powerup {
public:
	SpeedPowerup(float r, glm::vec3 pos, float s);
	float getSpeed() { return speed; }

protected:
	float speed;
};
