#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    pattern = new CalibrationPattern();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    pattern->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
