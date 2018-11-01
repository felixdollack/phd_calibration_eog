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
#include "ofxNetwork.h"

enum CalibrationStates {
    OFF,
    TARGET,
    PAUSE2REFERENCE,
    REFERENCE,
    PAUSE2TARGET
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
    float _marker_radius, _current_target_start_time, _time_per_target, _pause_start_time, _pause_duration;
    ofColor _marker_color, _marker_background_color;
    vector<ofVec2f> _target_positions;
    vector<ofVec2f> _target_correction;
    vector<int> _target_order;
    vector<ofSoundPlayer*> _target_command;
    Blinky *_calibration_target;
    bool _use_beep, _use_beeps, _use_commands, _use_reference, _is_recording;

    void getPatternPositions(float pattern_width, float pattern_height);
    void updatePatternPositions(int index);
    void loadSettings();
    void writeDefaultSettings();
    void nextTarget();
    void backToReference();
    void pause();

    UdpTrigger *_trigger;
    string _host_address;

    bool _use_remote_sound;
    ofxUDPManager _udp;
    string _remote_ip = "192.168.1.1";
    int _remote_port = 12345;
};

#endif /* calibrationPattern_h */
