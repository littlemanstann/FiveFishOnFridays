#include "DialogueBox.h"

DialogueBox::DialogueBox() {
    borderWidth = 3.0f;
    charDelay = 0.05f; // 50ms per character
    textPadding = 20.0f;
    lineHeight = 20.0f;
    dialogueActive = false;
    textComplete = false;
    currentMessageIndex = 0;
    charIndex = 0;
    lastCharTime = 0;
}

void DialogueBox::setup(float x, float y, float width, float height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    //Setup SFXs
    animalCrossingSFX.load("sounds/AC_SFX_Loopable.wav");
    animalCrossingSFX.setLoop(true);
    animalCrossingSFX.setVolume(0.8);
}

void DialogueBox::update() {
    if (!dialogueActive) return;

    // Animate text character by character
    if (!textComplete && currentMessageIndex < messages.size()) {
        if(!animalCrossingSFX.isPlaying()) animalCrossingSFX.play();
        
        float currentTime = ofGetElapsedTimef();

        if (currentTime - lastCharTime >= charDelay) {
            if (charIndex < messages[currentMessageIndex].length()) {
                displayedText += messages[currentMessageIndex][charIndex];
                charIndex++;
                lastCharTime = currentTime;
            }
            else {
                textComplete = true;
                
            }
        }
    }
    else {
        animalCrossingSFX.stop();
    }
}

void DialogueBox::draw() {
    if (!dialogueActive) return;

    // Draw black box
    ofSetColor(0, 0, 0);
    ofDrawRectangle(x, y, width, height);

    // Draw white border
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofSetLineWidth(borderWidth);
    ofDrawRectangle(x, y, width, height);
    ofFill();

    // Draw text
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(displayedText, x + textPadding, y + textPadding + lineHeight);
}

void DialogueBox::setDialogue(vector<string> msgs, std::function<void(MyCustomCamera*)> onComplete, MyCustomCamera* cam) {
    messages = msgs;
    currentMessageIndex = 0;
    charIndex = 0;
    displayedText = "";
    textComplete = false;
    dialogueActive = true;
    lastCharTime = ofGetElapsedTimef();

    onCompleteCallback = onComplete;
    targetCamera = cam;
}

void DialogueBox::keyPressed(int key) {
    if (!dialogueActive || key != ' ') return;

    // If text is still animating, finish it immediately
    if (!textComplete) {
        finishCurrentMessage();
    }
    // If text is complete and there are more messages, advance
    else if (currentMessageIndex < messages.size() - 1) {
        advanceToNextMessage();
    }
    // If this is the last message and text is complete, close dialogue
    else if (currentMessageIndex == messages.size() - 1 && textComplete) {
        dialogueActive = false;
        animalCrossingSFX.stop();
        if (onCompleteCallback && targetCamera) {
            onCompleteCallback(targetCamera);
            targetCamera->isDead = false;
        }
    }
}

void DialogueBox::advanceToNextMessage() {
    currentMessageIndex++;
    charIndex = 0;
    displayedText = "";
    textComplete = false;
    lastCharTime = ofGetElapsedTimef();
    animalCrossingSFX.play();
}

void DialogueBox::finishCurrentMessage() {
    displayedText = messages[currentMessageIndex];
    charIndex = messages[currentMessageIndex].length();
    textComplete = true;
    animalCrossingSFX.stop();
}

bool DialogueBox::isActive() {
    return dialogueActive;
}

bool DialogueBox::isComplete() {
    return textComplete && currentMessageIndex == messages.size() - 1;
}
