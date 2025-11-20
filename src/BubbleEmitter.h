#pragma once
#include "ofMain.h"

struct Bubble {
    glm::vec3 pos;
    glm::vec3 vel;
    float age = 0;
    float lifetime = 1.0f;
    float phase = 0;
};

class BubbleEmitter {
public:
    BubbleEmitter(ofCamera& camRef);

    void update(float dt);
    void draw();

    void setPosition(const glm::vec3& pos) { position = pos; box.move(pos); }
    void setSize(float s) { cubeSize = s; }

private:
    ofCamera& cam;
    ofBoxPrimitive box;

    glm::vec3 position = glm::vec3(0);
    float cubeSize = 50;

    std::vector<Bubble> bubbles;
    std::vector<glm::vec3> vboPositions;
    std::vector<float> vboAlpha;

    ofVbo vbo;
    ofShader bubbleShader;
    ofImage fire;

    glm::vec3 getCornerWorldPosition();
};
