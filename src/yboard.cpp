#include "yboard.h"

YBoardV2 Yboard;

YBoardV2::YBoardV2() : strip(led_count, led_pin, NEO_GRB + NEO_KHZ800) {}

YBoardV2::~YBoardV2() {}

void YBoardV2::setup() {
    setup_leds();
    setup_switches();
    setup_buttons();
    YAudio::setup(this->tone_pin);
}

////////////////////////////// LEDs ///////////////////////////////
void YBoardV2::setup_leds() {
    strip.begin();
    strip.clear();
    set_led_brightness(50);
}

void YBoardV2::set_led_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue) {
    strip.setPixelColor(index - 1, red, green, blue);
    strip.show();
}

void YBoardV2::set_led_brightness(uint8_t brightness) { strip.setBrightness(brightness); }

void YBoardV2::set_all_leds_color(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < this->led_count; i++) {
        strip.setPixelColor(i, red, green, blue, false);
    }
    strip.show();
}

////////////////////////////// Switches ///////////////////////////////
void YBoardV2::setup_switches() {
    pinMode(this->switch1_pin, INPUT);
    pinMode(this->switch2_pin, INPUT);
}

bool YBoardV2::get_switch(uint8_t switch_idx) {
    switch (switch_idx) {
    case 1:
        return digitalRead(this->switch1_pin);
    case 2:
        return digitalRead(this->switch2_pin);
    default:
        return false;
    }
}

////////////////////////////// Buttons ///////////////////////////////
void YBoardV2::setup_buttons() {
    pinMode(this->button1_pin, INPUT);
    pinMode(this->button2_pin, INPUT);
    pinMode(this->button3_pin, INPUT);
}

bool YBoardV2::get_button(uint8_t button_idx) {
    switch (button_idx) {
    case 1:
        return !digitalRead(this->button1_pin);
    case 2:
        return !digitalRead(this->button2_pin);
    case 3:
        return !digitalRead(this->button3_pin);
    default:
        return false;
    }
}

////////////////////////////// Knob ///////////////////////////////
int YBoardV2::get_knob() {
    int value = map(analogRead(this->knob_pin), 0, 4095, 0, 100);
    value = max(0, value);
    value = min(100, value);
    return value;
}

////////////////////////////// Speaker/Tones /////////////////////////////////////

bool YBoardV2::play_notes_background(const std::string &notes) { return YAudio::add_notes(notes); }

void YBoardV2::loop_speaker() { YAudio::loop(); }
