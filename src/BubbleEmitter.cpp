#include "BubbleEmitter.h"

BubbleEmitter::BubbleEmitter(ofCamera& camRef)
    : cam(camRef)
{
    box.set(1);

    int numParticles = 1000;
    bubbles.resize(numParticles);
    vboPositions.resize(numParticles);
    vboAlpha.resize(numParticles);

    fire.load("images/firesheet4x4.jpg");

    bubbleShader.load("shader/bubble.vert",
        "shader/bubble.frag",
        "shader/bubble.geom");

    vbo.setVertexData(vboPositions.data(), numParticles, GL_DYNAMIC_DRAW);
    vbo.updateAttributeData(1, vboAlpha.data(), vboAlpha.size());
}

glm::vec3 BubbleEmitter::getCornerWorldPosition() {
    // pick default corner (-0.5,-0.5,-0.5)
    glm::vec3 localCorner = glm::vec3(0, cubeSize / 2, cubeSize / 2);
    return box.getGlobalTransformMatrix() * glm::vec4(localCorner, 1.0f);
}

void BubbleEmitter::update(float dt) {
    glm::vec3 corner = getCornerWorldPosition();

    for (auto& b : bubbles) {
        if (b.age == 0 || b.age > b.lifetime) {
            b.pos = corner + glm::vec3(ofRandom(-0.5f, 0.5f), ofRandom(0, 1), ofRandom(-0.5f, 0.5f));
            b.age = 0;
            b.lifetime = ofRandom(2, 4);
            b.phase = ofRandom(TWO_PI);
            b.vel = glm::vec3(0, ofRandom(0.5, 1.0), 0);
        }

        b.pos += b.vel * dt;
        b.age += dt;

        int idx = &b - &bubbles[0];
        vboPositions[idx] = b.pos;
        vboAlpha[idx] = 1.0f - (b.age / b.lifetime);
    }

    vbo.updateVertexData(vboPositions.data(), vboPositions.size());
    vbo.updateAttributeData(1, vboAlpha.data(), vboAlpha.size());
}

void BubbleEmitter::draw() {
    box.draw();
    //ofMatrix4x4 modelMatrix = box.getGlobalTransformMatrix();
    ofMatrix4x4 viewMatrix = cam.getModelViewProjectionMatrix();
    ofMatrix4x4 mvp = viewMatrix;

    bubbleShader.begin();
    bubbleShader.setUniformTexture("texture0", fire.getTextureReference(), 1);
    bubbleShader.setUniform1f("t", ofGetElapsedTimef()); // time
    bubbleShader.setUniformMatrix4f("MVP", mvp);
    bubbleShader.setUniform1f("pSize", 2);

    vbo.draw(GL_POINTS, 0, bubbles.size());

    
    bubbleShader.end();
}
