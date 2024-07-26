#include "yaudio.h"

#include <Arduino.h>

namespace YAudio {

///////////////////////////////// Configuration Constants //////////////////////

static const int MAX_NOTES_IN_BUFFER = 4000;
static int tone_pin;

// This is the sequence of notes to play
static std::string notes;
static bool playing_notes = false;

// Notes state
static int beats_per_minute;
static int octave;
static int volume_notes;

typedef struct {
    unsigned int frequency;
    unsigned int duration;
} note_t;

// Note playing task
TaskHandle_t play_note_task_handle;
SemaphoreHandle_t notes_mutex;

//////////////////////////// Private Function Prototypes ///////////////////////
// Local private functions
static void play_note_task(void *params);
static note_t parse_next_note();
static void set_note_defaults();

////////////////////////////// Public Functions ///////////////////////////////
void setup(int pin) {
    // Initialize global variables
    tone_pin = pin;
    set_note_defaults();

    // Create the mutex for notes string
    notes_mutex = xSemaphoreCreateMutex();

    // Create task that will actually do the playing
    xTaskCreate(play_note_task, "play_note_task", 20000, NULL, 1, &play_note_task_handle);
}

bool add_notes(const std::string &new_notes) {
    if ((notes.length() + new_notes.length()) > MAX_NOTES_IN_BUFFER) {
        Serial.printf("Error adding notes: too many notes in buffer (%d + %d > %d).\n",
                      new_notes.length(), notes.length(), MAX_NOTES_IN_BUFFER);
        return false;
    }

    // Signal that we are playing notes
    playing_notes = true;

    // Append the new notes to the existing notes
    xSemaphoreTake(notes_mutex, portMAX_DELAY);
    notes += new_notes;
    xSemaphoreGive(notes_mutex);

    // Signal we need to play
    xTaskNotifyGive(play_note_task_handle);

    return true;
}

void stop() {
    xSemaphoreTake(notes_mutex, portMAX_DELAY);
    notes.clear();
    xSemaphoreGive(notes_mutex);
}

bool is_playing() { return playing_notes; }

////////////////////////////// Private Functions ///////////////////////////////

void set_note_defaults() {
    beats_per_minute = 120;
    octave = 2;
    volume_notes = 5;
}

void play_note_task(void *params) {
    while (1) {
        // Block waiting for notes to play
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Play all the notes until there are none left
        while (notes.length()) {
            xSemaphoreTake(notes_mutex, portMAX_DELAY);
            note_t note = parse_next_note();
            xSemaphoreGive(notes_mutex);

            Serial.printf("Playing note (frequency: %u, duration: %u)\n", note.frequency,
                          note.duration);

            // Play the tone and wait for it to finish
            tone(tone_pin, note.frequency, note.duration);
            vTaskDelay(note.duration / portTICK_PERIOD_MS);
        }

        // If all of the notes have been played, signal that we are done
        playing_notes = false;
    }
}

note_t parse_next_note() {
    static float note_freq;
    static float duration_s;

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

        duration_s = (60.0 / beats_per_minute); // Quarter note duration in seconds

        // A-G regular notes
        // R for rest
        // z for end rest, which is added internally to stop speaker crackle at the end
        if ((notes[0] >= 'A' && notes[0] <= 'G') || (notes[0] >= 'a' && notes[0] <= 'g') ||
            notes[0] == 'R' || notes[0] == 'r' || notes[0] == 'z') {
            switch (notes[0]) {
            case 'A':
            case 'a':
                note_freq = 440.0;
                break;
            case 'B':
            case 'b':
                note_freq = 493.88;
                break;
            case 'C':
            case 'c':
                note_freq = 523.25;
                break;
            case 'D':
            case 'd':
                note_freq = 587.33;
                break;
            case 'E':
            case 'e':
                note_freq = 659.25;
                break;
            case 'F':
            case 'f':
                note_freq = 698.46;
                break;
            case 'G':
            case 'g':
                note_freq = 783.99;
                break;
            case 'z':
                duration_s = 0.2;
                // Fallthrough
            case 'R':
            case 'r':
                note_freq = 0;
                break;
            }

            Serial.println("Parsing next note");

            // Adjust frequency for octave
            note_freq *= pow(2, octave - 4);
            notes.erase(0, 1);

            float dot_duration = duration_s;

            // Note modifiers
            while (1) {

                // Duration
                if (isdigit(notes[0])) {
                    size_t pos;
                    int frac_duration = std::stoi(notes, &pos);
                    notes = notes.substr(pos);
                    if (frac_duration >= 1 && frac_duration <= 2000) {
                        duration_s = duration_s * (4.0 / frac_duration);
                    }
                    continue;
                }

                // Dot
                if (notes[0] == '.') {
                    dot_duration /= 2;
                    duration_s += dot_duration;
                    notes.erase(0, 1);
                    continue;
                }

                // Octave
                if (notes[0] == '>') {
                    note_freq *= 2;
                    notes.erase(0, 1);
                    continue;
                }
                if (notes[0] == '<') {
                    note_freq /= 2;
                    notes.erase(0, 1);
                    continue;
                }

                // Sharp/flat
                if (notes[0] == '#' || notes[0] == '+' || notes[0] == '-') {
                    if (notes[0] == '#' || notes[0] == '+') {
                        note_freq *= pow(2, 1.0 / 12);
                    } else {
                        note_freq /= pow(2, 1.0 / 12);
                    }
                    notes.erase(0, 1);
                    continue;
                }

                break;
            }
            return {(unsigned int)round(note_freq), (unsigned int)(duration_s * 1000)};
        }

        // If we reach here then we have a syntax error
        Serial.printf("Syntax error in notes: %s\n", notes.c_str());
        notes.clear();
        break;
    }

    return {0, 0};
}

}; // namespace YAudio
