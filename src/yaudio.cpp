#include "yaudio.h"

#include <Arduino.h>

namespace YAudio {

///////////////////////////////// Configuration Constants //////////////////////

static const int MAX_NOTES_IN_BUFFER = 4000;
static int tone_pin;

////// Notes //////
static bool notes_running;

// This is the sequence of notes to play
static std::string notes;

// Notes state
static int beats_per_minute;
static int octave;
static int volume_notes;

// Next note to play
static bool next_note_parsed;
static float next_note_freq;
static float next_note_duration_s;

//////////////////////////// Private Function Prototypes ///////////////////////
// Local private functions
static void play_note_task(void *params);
static void parse_next_note();
static void set_note_defaults();

////////////////////////////// Public Functions ///////////////////////////////
bool add_notes(const std::string &new_notes) {
    if ((notes.length() + new_notes.length()) > MAX_NOTES_IN_BUFFER) {
        Serial.printf("Error adding notes: too many notes in buffer (%d + %d > %d).\n",
                      new_notes.length(), notes.length(), MAX_NOTES_IN_BUFFER);
        return false;
    }

    // Append the new notes to the existing notes
    notes += new_notes;

    // Signal we need to play
    notes_running = true;

    return true;
}

void loop() {
    if (notes_running) {
        // Parse the next note
        if (notes.length() && !next_note_parsed) {
            parse_next_note();
        }
    }
}

void stop() {
    if (notes_running) {
        notes_running = false;
        notes = "";
    }
}

bool is_playing() { return notes.length() > 0 || next_note_parsed; }

////////////////////////////// Private Functions ///////////////////////////////

void set_note_defaults() {
    beats_per_minute = 120;
    octave = 2;
    volume_notes = 5;
}

void reset_audio_buf() {
    next_note_parsed = false;
    notes = "";
}

void setup(int pin) {
    // Initialize global variables
    tone_pin = pin;
    reset_audio_buf();
    set_note_defaults();
    notes_running = false;
    xTaskCreate(play_note_task, "play_note_task", 20000, NULL, 1, NULL);
}

void play_note_task(void *params) {
    while (1) {
        // If there is nothing to do, then wait
        if (!next_note_parsed) {
            delay(1);
            continue;
        }

        // Convert duration from seconds to miliseconds
        unsigned long duration_ms = next_note_duration_s * 1000;

        Serial.printf("Playing note (frequency: %f, duration: %d)\n", next_note_freq, duration_ms);

        // Play the tone and wait for it to finish
        tone(tone_pin, next_note_freq, duration_ms);
        delay(duration_ms);

        // Signal that the note has been played
        next_note_parsed = false;
    }
}

void parse_next_note() {
    while (notes.length()) {
        // If first character is white space, remove it and continue
        if (isspace(notes[0])) {
            notes.erase(0, 1);
            continue;
        }

        // Octave
        if (notes[0] == 'O' || notes[0] == 'o') {
            int new_octave = notes[1] - '0';
            if (new_octave >= 4 && new_octave <= 7) {
                octave = new_octave;
            }
            notes.erase(0, 2);
            continue;
        }

        // Tempo
        if (notes[0] == 'T' || notes[0] == 't') {
            notes.erase(0, 1);
            size_t pos;
            int new_tempo = std::stoi(notes, &pos);
            notes = notes.substr(pos);
            if (new_tempo >= 40 && new_tempo <= 240) {
                beats_per_minute = new_tempo;
            }
            continue;
        }

        // Reset
        if (notes[0] == '!') {
            set_note_defaults();
            notes.erase(0, 1);
            continue;
        }

        // Volume
        if (notes[0] == 'V' || notes[0] == 'v') {
            notes.erase(0, 1);
            size_t pos;
            int new_volume = std::stoi(notes, &pos);
            notes = notes.substr(pos);
            if (new_volume >= 1 && new_volume <= 10) {
                volume_notes = new_volume;
            }
            continue;
        }

        next_note_duration_s = (60.0 / beats_per_minute); // Quarter note duration in seconds

        // A-G regular notes
        // R for rest
        // z for end rest, which is added internally to stop speaker crackle at the end
        if ((notes[0] >= 'A' && notes[0] <= 'G') || (notes[0] >= 'a' && notes[0] <= 'g') ||
            notes[0] == 'R' || notes[0] == 'r' || notes[0] == 'z') {
            switch (notes[0]) {
            case 'A':
            case 'a':
                next_note_freq = 440.0;
                break;
            case 'B':
            case 'b':
                next_note_freq = 493.88;
                break;
            case 'C':
            case 'c':
                next_note_freq = 523.25;
                break;
            case 'D':
            case 'd':
                next_note_freq = 587.33;
                break;
            case 'E':
            case 'e':
                next_note_freq = 659.25;
                break;
            case 'F':
            case 'f':
                next_note_freq = 698.46;
                break;
            case 'G':
            case 'g':
                next_note_freq = 783.99;
                break;
            case 'z':
                next_note_duration_s = 0.2;
                // Fallthough
            case 'R':
            case 'r':
                next_note_freq = 0;
                break;
            }

            Serial.println("Parsing next note");

            // Adjust frequency for octave
            next_note_freq *= pow(2, octave - 4);
            notes.erase(0, 1);

            float dot_duration = next_note_duration_s;

            // Note modifiers
            while (1) {

                // Duration
                if (isdigit(notes[0])) {
                    size_t pos;
                    int frac_duration = std::stoi(notes, &pos);
                    notes = notes.substr(pos);
                    if (frac_duration >= 1 && frac_duration <= 2000) {
                        next_note_duration_s = next_note_duration_s * (4.0 / frac_duration);
                    }
                    continue;
                }

                // Dot
                if (notes[0] == '.') {
                    dot_duration /= 2;
                    next_note_duration_s += dot_duration;
                    notes.erase(0, 1);
                    continue;
                }

                // Octave
                if (notes[0] == '>') {
                    next_note_freq *= 2;
                    notes.erase(0, 1);
                    continue;
                }
                if (notes[0] == '<') {
                    next_note_freq /= 2;
                    notes.erase(0, 1);
                    continue;
                }

                // Sharp/flat
                if (notes[0] == '#' || notes[0] == '+' || notes[0] == '-') {
                    if (notes[0] == '#' || notes[0] == '+') {
                        next_note_freq *= pow(2, 1.0 / 12);
                    } else {
                        next_note_freq /= pow(2, 1.0 / 12);
                    }
                    notes.erase(0, 1);
                    continue;
                }

                break;
            }
            break;
        }

        // X<>M<> format for specific frequency (X) and duration (Milliseconds)
        if (notes[0] == 'X' || notes[0] == 'x') {
            notes.erase(0, 1);
            size_t x_pos;
            next_note_freq = std::stof(notes, &x_pos);
            notes = notes.substr(x_pos);

            if (!(next_note_freq >= 20 && next_note_freq <= 20000)) {
                next_note_freq = 0;
            }

            if (notes[0] == 'M' || notes[0] == 'm') {
                notes.erase(0, 1);
                size_t m_pos;
                next_note_duration_s = std::stof(notes, &m_pos) / 1000.0;
                notes = notes.substr(m_pos);
            }
            break;
        }

        // If we reach here then we have a syntax error
        Serial.printf("Syntax error in notes: %s\n", notes.c_str());
        notes = "";
        break;
    }
    next_note_parsed = true;
}

}; // namespace YAudio
