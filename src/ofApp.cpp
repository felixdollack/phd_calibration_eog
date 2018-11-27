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

    if (key == '1') {
        pattern->connectEyeTracker();
    }
    if (key == '2') { //
        pattern->setupProjectEyeTracker();
    }
    if (key == '3') { //
        pattern->setupSubjectEyeTracker();
    }
    if (key == '4') {
        pattern->streamEyeTracker();
    }
    if (key == 'c') { // calibrate
        pattern->calibrateEyeTracker();
    }
    if (key == 'q') { // end
        pattern->stopRecordingEyeTracker();
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
