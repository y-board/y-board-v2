#ifndef YBOARDV2_H
#define YBOARDV2_H

#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "yaudio.h"

class YBoardV2 {
  public:
    YBoardV2();
    virtual ~YBoardV2();

    /*
     *  This function initializes the YBoard. This function must be called before using any of the
     * YBoard features.
     */
    void setup();

    ////////////////////////////// LEDs ///////////////////////////////////////////
    /*
     *  This function sets the color of an individual LED.
     *  The index is an integer between 1 and 20, representing the number of the
     * target LED (for example, 1 corresponds to the first LED on the board).
     *  The red, green, and blue values are integers between 0 and 255, representing
     * the intensity of the corresponding color. For example, if you want to set the
     * LED to red, you would set red to 255 and green and blue to 0.
     */
    void set_led_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);

    /*
     *  This function sets the brightness of all the LEDs on the board.
     *  The brightness is an integer between 0 and 255, representing the intensity
     * of the LEDs. A brightness of 0 is off, and a brightness of 255 is full
     * brightness.
     */
    void set_led_brightness(uint8_t brightness);

    /*
     *  This function sets the color of all the LEDs on the board.
     *  The red, green, and blue values are integers between 0 and 255, representing
     * the intensity of the corresponding color. For example, if you want to set all
     * the LEDs to red, you would set red to 255 and green and blue to 0.
     */
    void set_all_leds_color(uint8_t red, uint8_t green, uint8_t blue);

    ////////////////////////////// Switches/Buttons ///////////////////////////////
    /*
     *  This function returns the state of a switch.
     *  The switch_idx is an integer between 1 and 2, representing the number of the
     * target switch (for example, 1 corresponds to switch 1 on the board). The bool
     * return type means that this function returns a boolean value (true or false).
     *  True corresponds to the switch being on, and false corresponds to the switch
     * being off.
     */
    bool get_switch(uint8_t switch_idx);

    /*
     *  This function returns the state of a button.
     *  The button_idx is an integer between 1 and 3, representing the number of the
     * target button (for example, 1 corresponds to button 1 on the board). The bool
     * return type means that this function returns a boolean value (true or false).
     *  True corresponds to the button being pressed, and false corresponds to the
     * button being released.
     */
    bool get_button(uint8_t button_idx);

    /*
     *  This function returns the value of the knob.
     *  The return type is an integer between 0 and 100, representing the position
     * of the knob. A value of 0 corresponds to the knob being turned all the way to
     * the left, and a value of 100 corresponds to the knob being turned all the way
     * to the right.
     */
    int get_knob();

    ////////////////////////////// Speaker/Tones //////////////////////////////////
    /*
     *  This function plays a note on the speaker.
     *  The freq parameter is an integer representing the frequency of the note to be
     * played. The duration parameter is a long integer representing the duration of
     * the note in milliseconds.
     */
    // void play_note_background(unsigned int freq, unsigned long duration);

    void loop_speaker();
    bool play_notes(const std::string &notes);
    bool play_notes_background(const std::string &notes);
    void stop_audio();
    bool is_audio_playing();

    // LEDs
    static constexpr int led_pin = 5;
    static constexpr int led_count = 20;

    // Controls
    static constexpr int knob_pin = 14;
    static constexpr int switch1_pin = 9;
    static constexpr int switch2_pin = 10;
    static constexpr int button1_pin = 11;
    static constexpr int button2_pin = 12;
    static constexpr int button3_pin = 13;

    // Speaker
    static constexpr int tone_pin = 33;

  private:
    Adafruit_NeoPixel strip;

    void setup_leds();
    void setup_switches();
    void setup_buttons();
};

extern YBoardV2 Yboard;

#endif /* YBOARDV2_H */
