//
//  calibrationPattern.cpp
//  phd_calibration_eog
//
//  Created by Felix Dollack on 10.10.18.
//

#include "calibrationPattern.h"

CalibrationPattern::CalibrationPattern() {
    // write default settings (if necessary) and load settings
    this->_pattern_settings = new ofxXmlSettings();
    this->_pattern_settings->loadFile(this->_pattern_settings_filename);
    if (this->_pattern_settings->bDocLoaded == false) {
        writeDefaultSettings();
    }
    loadSettings();

    getPatternPositions(ofGetWindowWidth(), ofGetWindowHeight());
}

void CalibrationPattern::resizePattern(float window_width, float window_height) {
    getPatternPositions(window_width, window_height);
}

void CalibrationPattern::loadSettings() {
    this->_pattern_settings->pushTag("calibration");
    this->_pattern_settings->pushTag("target");
    this->_marker_radius = this->_pattern_settings->getValue("radius", 2.0f);

    float r,g,b;
    this->_pattern_settings->pushTag("color");
    r = this->_pattern_settings->getValue("red",   0);
    g = this->_pattern_settings->getValue("green", 0);
    b = this->_pattern_settings->getValue("blue",  0);
    this->_marker_color = ofColor(r, g, b);
    this->_pattern_settings->popTag();

    this->_pattern_settings->popTag();

    this->_pattern_settings->pushTag("order");
    int positions_in_pattern = this->_pattern_settings->getValue("size", 0);
    for (int i = 0; i < positions_in_pattern; i++) {
        this->_pattern_settings->pushTag("item", i);
        this->_target_order.push_back(this->_pattern_settings->getValue("n",  0));
        this->_pattern_settings->popTag();
    }
    this->_pattern_settings->popTag();
    this->_pattern_settings->popTag();
}

void CalibrationPattern::writeDefaultSettings() {
    this->_pattern_settings->addTag("calibration");
    this->_pattern_settings->pushTag("calibration");

    this->_pattern_settings->addTag("target");
    this->_pattern_settings->pushTag("target");
    this->_pattern_settings->addValue("radius", 12.0f);
    this->_pattern_settings->addTag("color");
    this->_pattern_settings->pushTag("color");
    this->_pattern_settings->addValue("red", 128);
    this->_pattern_settings->addValue("green", 0);
    this->_pattern_settings->addValue("blue", 128);
    this->_pattern_settings->popTag();
    this->_pattern_settings->popTag();

    this->_pattern_settings->addTag("order");
    this->_pattern_settings->pushTag("order");
    this->_pattern_settings->addValue("size", 13);
    for (int i = 0; i < 13; i++) {
        this->_pattern_settings->addTag("item");
        this->_pattern_settings->pushTag("item", i);
        this->_pattern_settings->addValue("n",  i);
        this->_pattern_settings->popTag();
    }
    this->_pattern_settings->popTag();

    this->_pattern_settings->popTag();
    this->_pattern_settings->saveFile(this->_pattern_settings_filename);
}

void CalibrationPattern::getPatternPositions(float pattern_width, float pattern_height) {
    float w = pattern_width;
    float h = pattern_height;
    /*
     * 1    2    3
     *    9   10
     * 8    0    4
     *   12   11
     * 7    6    5
     */
    this->_target_positions.push_back(ofVec2f(w/2,   h/2  )); // center center
    this->_target_positions.push_back(ofVec2f(0,     0    )); // top    left   corner
    this->_target_positions.push_back(ofVec2f(w/2,   0    )); // top    center
    this->_target_positions.push_back(ofVec2f(w,     0    )); // top    right  corner
    this->_target_positions.push_back(ofVec2f(w,     h/2  )); // right  center
    this->_target_positions.push_back(ofVec2f(w,     h    )); // bottom right  corner
    this->_target_positions.push_back(ofVec2f(w/2,   h    )); // bottom center
    this->_target_positions.push_back(ofVec2f(0,     h    )); // bottem left   corner
    this->_target_positions.push_back(ofVec2f(0,     h/2  )); // left   center
    this->_target_positions.push_back(ofVec2f(w/4,   h/4  ));
    this->_target_positions.push_back(ofVec2f(w*3/4, h/4  ));
    this->_target_positions.push_back(ofVec2f(w*3/4, h*3/4));
    this->_target_positions.push_back(ofVec2f(w/4,   h*3/4));

    // correction factor to put the pattern centered is dependent on the target size
    this->_target_correction.push_back(ofVec2f(-1, -1));
    this->_target_correction.push_back(ofVec2f( 1,  1));
    this->_target_correction.push_back(ofVec2f(-1,  1));
    this->_target_correction.push_back(ofVec2f(-2,  1));
    this->_target_correction.push_back(ofVec2f(-2, -1));
    this->_target_correction.push_back(ofVec2f(-2, -2));
    this->_target_correction.push_back(ofVec2f(-1, -2));
    this->_target_correction.push_back(ofVec2f( 1, -2));
    this->_target_correction.push_back(ofVec2f( 1, -1));
    this->_target_correction.push_back(ofVec2f(-0.5f, -0.5f));
    this->_target_correction.push_back(ofVec2f(-1.5f, -0.5f));
    this->_target_correction.push_back(ofVec2f(-1.5f, -1.5f));
    this->_target_correction.push_back(ofVec2f(-0.5f, -1.5f));

    // total number of targets in pattern
    this->_number_of_targets = this->_target_positions.size();
}
