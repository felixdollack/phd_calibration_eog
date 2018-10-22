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
#include "ofx_udp_trigger.h"

enum CalibrationStates {
    OFF,
    TARGET,
    REFERENCE
};

class CalibrationPattern {
public:
    CalibrationPattern();
    void resizePattern(float window_width, float window_height);
    void draw();
    void update();
    bool isRunning();
    void startCalibration();
    void stopCalibration();
private:
    CalibrationStates _state;
    const string _pattern_settings_filename = "calibrationSettings.xml";
    ofxXmlSettings *_pattern_settings;
    int _number_of_targets, _reference_target, _current_target;
    float _marker_radius, _current_target_start_time, _time_per_target;
    ofColor _marker_color, _marker_background_color;
    vector<ofVec2f> _target_positions;
    vector<ofVec2f> _target_correction;
    vector<int> _target_order;
    vector<Blinky> _calibration_targets;

    void getPatternPositions(float pattern_width, float pattern_height);
    void updatePatternPositions();
    void loadSettings();
    void writeDefaultSettings();
    void nextTarget();
    void backToReference();

    UdpTrigger *_trigger;
    string _host_address;
};

#endif /* calibrationPattern_h */
