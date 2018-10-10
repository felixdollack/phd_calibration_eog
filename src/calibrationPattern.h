//
//  calibrationPattern.h
//  phd_calibration_eog
//
//  Created by Felix Dollack on 10.10.18.
//

#ifndef calibrationPattern_h
#define calibrationPattern_h

#include "ofMain.h"
#include "ofx_blinky.h"
#include "ofxXmlSettings.h"

class CalibrationPattern {
public:
    CalibrationPattern();
    void resizePattern(float window_width, float window_height);
private:
    const string _pattern_settings_filename = "calibrationSettings.xml";
    ofxXmlSettings *_pattern_settings;
    int _number_of_targets;
    vector<ofVec2f> _target_positions;
    vector<ofVec2f> _target_correction;

    void getPatternPositions(float pattern_width, float pattern_height);
    void writeDefaultSettings();
};

#endif /* calibrationPattern_h */
