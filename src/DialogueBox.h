#pragma once
#include "ofMain.h"
#include "MyCustomCamera.h"

class DialogueBox {
public:
    DialogueBox();

    void setup(float x, float y, float width, float height);
    void update();
    void draw();
    void setDialogue(vector<string> msgs, std::function<void(MyCustomCamera*)> onComplete = nullptr, MyCustomCamera* cam = nullptr);
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

    // Callback
    std::function<void(MyCustomCamera*)> onCompleteCallback = nullptr;
    MyCustomCamera* targetCamera = nullptr;

    // SFX
    ofSoundPlayer animalCrossingSFX;

    void advanceToNextMessage();
    void finishCurrentMessage();
};