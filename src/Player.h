#pragma once
#include "ofMain.h"
#include "MeshObject.h"


class Player : public MeshObject {

	public:
		Player();


	protected:
		float radius;
		float height;

		glm::vec3 targetPos;
		glm::vec3 cameraPos;





};
