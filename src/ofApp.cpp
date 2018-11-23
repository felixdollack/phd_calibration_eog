#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    pattern = new CalibrationPattern();
}

//--------------------------------------------------------------
void ofApp::update(){
    pattern->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    pattern->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 32) { // spacebar
        if (pattern->isRunning() == false) {
            ofHideCursor();
            pattern->recordEyeTracker();
            pattern->startCalibration();
        } else {
            pattern->stopCalibration();
            pattern->stopRecordingEyeTracker();
            ofShowCursor();
        }
    }

    if (key == 's') { // setup
        pattern->setupEyeTracker();
    }
    if (key == 'c') { // calibrate
        pattern->calibrateEyeTracker();
    }
    if (key == 'q') { // end
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    pattern->resizePattern(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}
