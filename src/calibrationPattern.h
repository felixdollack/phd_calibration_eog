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
#include "ofxOsc.h"

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

    void setupProjectEyeTracker();
    void setupSubjectEyeTracker();
    void connectEyeTracker();
    void streamEyeTracker();
    void cleanupEyeTracker();
    void calibrateEyeTracker();
    void recordEyeTracker();
    void stopRecordingEyeTracker();
    void sendEyeTrackerEvent(string message);

private:
    CalibrationStates _state;
    const string _settings_filename = "calibrationSettings.xml";
    string _pattern_settings_filename;
    ofxXmlSettings *_pattern_settings, *_settings;
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
    void loadPatternSettings();
    void writeDefaultPatternSettings();
    void nextTarget();
    void backToReference();
    void pause();

    UdpTrigger *_trigger;
    string _host_address;

    bool _use_remote_sound;
    ofxUDPManager _udp;
    string _remote_ip;
    int _remote_port;

    ofxOscSender *_osc;
    string _osc_ip, _codeword;
};

#endif /* calibrationPattern_h */
