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
    bool success = this->_pattern_settings->loadFile(this->_pattern_settings_filename);
    if (success == false) {
        writeDefaultSettings();
        this->_pattern_settings->loadFile(this->_pattern_settings_filename);
    }
    loadSettings();

    this->_is_recording = false;

    if (this->_reference_target > -1) {
        this->_use_reference = true;
    } else {
        this->_use_reference = false;
    }
    getPatternPositions(ofGetWindowWidth(), ofGetWindowHeight());
    BeepMode mode;
    if (_use_beep == true) {
        mode = BeepMode::BEEP_ON_START;
    } else {
        mode = BeepMode::BEEP_OFF;
    }
    if (_use_beeps == true) {
        mode = BeepMode::BEEP_ON_END;
    }
    // swithc beep mode off on this local machine if remote is used
    if (this->_use_remote_sound == true) {
        mode = BeepMode::BEEP_OFF;
    }
    this->_calibration_target = new Blinky(this->_marker_radius, this->_marker_color, this->_marker_background_color, mode, false, 0.0f);
    updatePatternPositions(this->_current_target);

    this->_current_target = -1;
    this->_current_target_start_time = 0;
    this->_state = OFF;
    this->_pause_start_time = 0;

    this->_trigger = new UdpTrigger(this->_host_address);
    this->_trigger->connectToHost();

    this->_udp.Create();
    this->_udp.Connect(this->_remote_ip.c_str(), this->_remote_port);
    this->_udp.SetNonBlocking(true);
}

void CalibrationPattern::resizePattern(float window_width, float window_height) {
    getPatternPositions(window_width, window_height);
    updatePatternPositions(this->_current_target);
}

void CalibrationPattern::draw() {
    ofClear(ofColor::black);
    this->_calibration_target->draw();
}

void CalibrationPattern::update() {
    if (this->_state != OFF) {
        // stop pattern after last target
        if (this->_current_target >= this->_number_of_targets) {
            stopCalibration();
        }

        // reset pause time when pause duration is over
        if ((this->_state == PAUSE2REFERENCE) || (this->_state == PAUSE2TARGET)) {
            if ((ofGetElapsedTimef() - this->_pause_start_time) > this->_pause_duration) {
                this->_pause_start_time = 0;
                if ((this->_state == PAUSE2REFERENCE) && (this->_use_reference == true)) {
                    backToReference();
                    if (this->_use_remote_sound == true) {
                        string msg = ofToString(9999);
                        _udp.Send(msg.c_str(), msg.length());
                    }
                    this->_calibration_target->setBlinkyOn(true);
                } else {
                    nextTarget();
                    if (this->_use_remote_sound == true) {
                        string msg = ofToString(9999);
                        _udp.Send(msg.c_str(), msg.length());
                    }
                    this->_calibration_target->setBlinkyOn(true);
                }
            }
        } else {
            // stop blinking the curret target and shift to the next
            if ((ofGetElapsedTimef() - this->_current_target_start_time) > this->_time_per_target) {
                if ((this->_state == REFERENCE) && (this->_pause_start_time == 0)) {
                    if ((this->_use_remote_sound == true) && (this->_use_beeps == true)) {
                        string msg = ofToString(9999);
                        _udp.Send(msg.c_str(), msg.length());
                    }
                    this->_calibration_target->setBlinkyOn(false);
                    pause();
                }
                if ((this->_state == TARGET) && (this->_pause_start_time == 0)) {
                    if ((this->_use_remote_sound == true) && (this->_use_beeps == true)) {
                        string msg = ofToString(9999);
                        _udp.Send(msg.c_str(), msg.length());
                    }
                    this->_calibration_target->setBlinkyOn(false);
                    pause();
                }
            }
        }
    } else {
        if (this->_current_target_start_time > 0) {
            this->_calibration_target->setBlinkyOn(false);
            if ((this->_use_remote_sound == true) && (this->_use_beeps == true)) {
                string msg = ofToString(9999);
                _udp.Send(msg.c_str(), msg.length());
            }
            if (this->_is_recording == true) {
                this->_trigger->stopRecording();
            }
            this->_is_recording = false;
        }
    }

    this->_calibration_target->update();
}

void CalibrationPattern::pause() {
    if ((this->_state == TARGET) && (this->_use_reference == true)) {
        this->_state = PAUSE2REFERENCE;
        this->_trigger->sendTrigger(ofToString(this->_current_target));
        if (this->_use_commands) {
            if (this->_use_remote_sound == true) {
                string msg = ofToString(this->_reference_target);
                _udp.Send(msg.c_str(), msg.length());
            } else {
                if (this->_target_command[this->_reference_target] != NULL) {
                    this->_target_command[this->_reference_target]->play();
                }
            }
        }
    } else {
        this->_state = PAUSE2TARGET;
        this->_trigger->sendTrigger(ofToString(this->_reference_target));
        if ((this->_current_target+1) < this->_number_of_targets) {
            if (this->_use_commands) {
                if (this->_use_remote_sound == true) {
                    string msg = ofToString(this->_current_target+1);
                    _udp.Send(msg.c_str(), msg.length());
                } else {
                    if (this->_target_command[this->_current_target+1] != NULL) {
                        this->_target_command[this->_current_target+1]->play();
                    }
                }
            }
        }
    }
    this->_pause_start_time = ofGetElapsedTimef();
}

void CalibrationPattern::nextTarget() {
    this->_state = TARGET;
    this->_current_target++;
    updatePatternPositions(this->_current_target);
    this->_current_target_start_time = ofGetElapsedTimef();
    this->_trigger->sendTrigger(ofToString(this->_current_target));
}

void CalibrationPattern::backToReference() {
    this->_state = REFERENCE;
    updatePatternPositions(this->_reference_target);
    this->_current_target_start_time = ofGetElapsedTimef();
    this->_trigger->sendTrigger(ofToString(this->_reference_target));
}

void CalibrationPattern::startCalibration() {
    this->_trigger->startRecording();
    this->_state = REFERENCE;
    pause();
    this->_is_recording = true;
}

void CalibrationPattern::stopCalibration() {
    this->_state = OFF;
    this->_current_target = -1;
    this->_trigger->stopRecording();
    this->_is_recording = false;
}

bool CalibrationPattern::isRunning() {
    return this->_is_recording;
}

void CalibrationPattern::loadSettings() {
    this->_pattern_settings->pushTag("calibration");
    {
        this->_host_address = this->_pattern_settings->getValue("host", "192.168.1.1");
        this->_pattern_settings->pushTag("target");
        {
            this->_time_per_target = this->_pattern_settings->getValue("duration", 1.0f);
            this->_pause_duration = this->_pattern_settings->getValue("pause", 0.0f);
            this->_marker_radius = this->_pattern_settings->getValue("radius", 2.0f);
            this->_use_remote_sound = this->_pattern_settings->getValue("remote_sound", 0);
            this->_pattern_settings->pushTag("beep");
            {
                this->_use_beep = this->_pattern_settings->getValue("once", 2.0f);
                this->_use_beeps = this->_pattern_settings->getValue("always", 2.0f);
            }
            this->_pattern_settings->popTag();

            float r,g,b;
            this->_pattern_settings->pushTag("color");
            {
                this->_pattern_settings->pushTag("foreground");
                r = this->_pattern_settings->getValue("red",   0);
                g = this->_pattern_settings->getValue("green", 0);
                b = this->_pattern_settings->getValue("blue",  0);
                this->_marker_color = ofColor(r, g, b);
                this->_pattern_settings->popTag();
                this->_pattern_settings->pushTag("background");
                r = this->_pattern_settings->getValue("red",   0);
                g = this->_pattern_settings->getValue("green", 0);
                b = this->_pattern_settings->getValue("blue",  0);
                this->_marker_background_color = ofColor(r, g, b);
                this->_pattern_settings->popTag();
            }
            this->_pattern_settings->popTag();
        }
        this->_pattern_settings->popTag();

        this->_pattern_settings->pushTag("order");
        {
            this->_use_commands = this->_pattern_settings->getValue("verbal_commands", 0);
            this->_reference_target = this->_pattern_settings->getValue("reference", -1);
            this->_number_of_targets = this->_pattern_settings->getValue("size", 0);
            string filename;
            for (int i = 0; i < this->_number_of_targets; i++) {
                this->_pattern_settings->pushTag("item", i);
                {
                    this->_target_order.push_back(this->_pattern_settings->getValue("n",  0));
                    filename = this->_pattern_settings->getValue("command",  "");
                    ofSoundPlayer *command = NULL;
                    if (filename != "") {
                        command = new ofSoundPlayer();
                        command->load(filename);
                    }
                    this->_target_command.push_back(command);
                }
                this->_pattern_settings->popTag();
            }
        }
        this->_pattern_settings->popTag();

        this->_pattern_settings->pushTag("remote");
        {
            this->_remote_ip = this->_pattern_settings->getValue("ip", "");
            this->_remote_port = this->_pattern_settings->getValue("port", 0);
        }
        this->_pattern_settings->popTag();
    }
    this->_pattern_settings->popTag();
}

void CalibrationPattern::writeDefaultSettings() {
    this->_pattern_settings->addTag("calibration");
    this->_pattern_settings->pushTag("calibration");
    {
        this->_pattern_settings->addValue("host", "192.168.1.1");
        this->_pattern_settings->addTag("target");
        this->_pattern_settings->pushTag("target");
        {
            this->_pattern_settings->addValue("duration", 2.0f);
            this->_pattern_settings->addValue("pause", 0.25f);
            this->_pattern_settings->addValue("radius", 12.0f);
            this->_pattern_settings->addValue("remote_sound", 0);
            this->_pattern_settings->addTag("beep");
            this->_pattern_settings->pushTag("beep");
            {
                this->_use_beep = this->_pattern_settings->addValue("once", true);
                this->_use_beeps = this->_pattern_settings->addValue("always", false);
            }
            this->_pattern_settings->popTag();
            this->_pattern_settings->addTag("color");
            this->_pattern_settings->pushTag("color");
            {
                this->_pattern_settings->addTag("foreground");
                this->_pattern_settings->pushTag("foreground");
                {
                    this->_pattern_settings->addValue("red", 128);
                    this->_pattern_settings->addValue("green", 0);
                    this->_pattern_settings->addValue("blue", 128);
                }
                this->_pattern_settings->popTag();
                this->_pattern_settings->addTag("background");
                this->_pattern_settings->pushTag("background");
                {
                    this->_pattern_settings->addValue("red",   0);
                    this->_pattern_settings->addValue("green", 0);
                    this->_pattern_settings->addValue("blue",  0);
                }
                this->_pattern_settings->popTag();
            }
            this->_pattern_settings->popTag();
        }
        this->_pattern_settings->popTag();

        this->_pattern_settings->addTag("order");
        this->_pattern_settings->pushTag("order");
        {
            this->_pattern_settings->addValue("verbal_commands", 0);
            this->_pattern_settings->addValue("reference", 0);
            this->_pattern_settings->addValue("size", 13);
            for (int i = 0; i < 13; i++) {
                this->_pattern_settings->addTag("item");
                this->_pattern_settings->pushTag("item", i);
                {
                    this->_pattern_settings->addValue("n",  i);
                    this->_pattern_settings->addValue("command",  "");
                }
                this->_pattern_settings->popTag();
            }
        }
        this->_pattern_settings->popTag();
        this->_pattern_settings->addTag("remote");
        this->_pattern_settings->pushTag("remote");
        {
            this->_pattern_settings->addValue("ip",   "192.168.1.1");
            this->_pattern_settings->addValue("port", 12345);
        }
        this->_pattern_settings->popTag();
    }
    this->_pattern_settings->popTag();
    this->_pattern_settings->saveFile(this->_pattern_settings_filename);
}

void CalibrationPattern::updatePatternPositions(int index) {
    if ((index > -1) && (index < this->_number_of_targets)) {
        this->_calibration_target->setPosition((this->_target_correction[this->_target_order[index]] * this->_marker_radius) + this->_target_positions[this->_target_order[index]] + this->_marker_radius/2);
    }
}

void CalibrationPattern::getPatternPositions(float pattern_width, float pattern_height) {
    float w = pattern_width;
    float h = pattern_height;
    // clear in case we call this not for the first time
    this->_target_positions.clear();
    this->_target_correction.clear();
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
}
