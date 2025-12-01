#pragma once
#include "ofMain.h"

class DialogueBox {
public:
    DialogueBox();

    void setup(float x, float y, float width, float height);
    void update();
    void draw();
    void setDialogue(vector<string> msgs);
    void keyPressed(int key);

    bool isActive();
    bool isComplete();

private:
    // Box properties
    float x, y, width, height;
    float borderWidth;

    // Dialogue properties
    vector<string> messages;
    int currentMessageIndex;
    string displayedText;
    int charIndex;
    float charDelay;
    float lastCharTime;
    bool textComplete;
    bool dialogueActive;

    // Text properties
    float textPadding;
    float lineHeight;

    void advanceToNextMessage();
    void finishCurrentMessage();
};