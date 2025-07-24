#ifndef DETECT_H
#define DETECT_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_BINS 32

// Call once at startup to clear history
void detect_init(void);

// Call after your magnitude step (realNumbers[0..31]).
// Returns true if a shot?like event is detected.
bool detect_shot(const int16_t mag[NUM_BINS]);

#endif // DETECT_H
