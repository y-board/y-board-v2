#ifndef YAUDIO_H
#define YAUDIO_H

#include <stdint.h>
#include <string>

namespace YAudio {

void setup();
void loop();
bool add_notes(const std::string &new_notes);
void stop();
bool is_playing();
}; // namespace YAudio

#endif /* YAUDIO_H */
