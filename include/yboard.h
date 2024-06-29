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
     *  This function continues to play a sound on the speaker after the
     * play_notes_background function is called. This function must be called
     * often to playback the sound on the speaker.
     */
    void loop_speaker();

    /* Plays the specified sequence of notes. The function will return once the notes
     * have finished playing.
     *
     * A–G	                Specifies a note that will be played.
     * R                    Specifies a rest (no sound for the duration of the note).
     * + or # after a note  Raises the preceding note one half-step (sharp).
     * - after a note	      Lowers the preceding note one half-step.
     * > after a note	      Plays the note one octave higher (multiple >’s can be used, eg: C>>)
     * < after a note	      Plays the note one octave lower (multiple <’s can be used, eg: C<<)
     * 1–2000 after a note	Determines the duration of the preceding note. For example,
     *                      C16 specifies C played as a sixteenth note, B1 is B played as a whole
     *                      note. If no duration is specified, the note is played as a quarter note.
     * O followed by a #    Changes the octave. Valid range is 4-7. Default is 5.
     * T followed by a #    Changes the tempo. Valid range is 40-240. Default is 120.
     * !                    Resets octave, tempo, and volume to default values.
     * spaces               Spaces can be placed between notes or commands for readability,
     *                      but not within a note or command (eg: "C4# D4" is valid, "C 4 # D 4" is
     *                      not. "T120 A B C" is valid, "T 120 A B C" is not).
     */
    bool play_notes(const std::string &notes);

    /* This is similar to the function above, except that it will start playing the notes
     * in the background and return immediately. The notes will continue to play in the
     * background until they are stopped with the stop_audio function or the notes finish.
     * If you call this function again before the notes finish, the the new notes will be
     * appended to the end of the current notes.  This allows you to call this function
     * multiple times to build up multiple sequences of notes to play. After this function
     * is called, the loop_speaker function must be called often to playback the sound on
     * the speaker.
     */
    bool play_notes_background(const std::string &notes);

    /*
     * This function stops the audio from playing (either a song or a sequence of notes)
     */
    void stop_audio();

    /*
     *  This function returns whether audio is playing.
     */
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
    TaskHandle_t background_sound_task = NULL;

    void setup_leds();
    void setup_switches();
    void setup_buttons();
};

extern YBoardV2 Yboard;

#endif /* YBOARDV2_H */
