// detect.c

#include "detect.h"
#include <stdlib.h>  // for abs()
#include <stdio.h>
// ?????????? CHANGE?1: Per?bin noise floor from your AMBIENT?ONLY data ??????????
// These values came from scanning your ambient?only frames (no music, no shots)
// Take the true max you observed in bin?i and paste it here. E.g. if bin?3 never
// exceeded 7 in ambient, use 8 or 7. Adjust up if you still see false positives.
static const uint16_t noiseThreshold[NUM_BINS] = {
    /* bin0 (DC, ignored) */  0,
    /*  1 */                  8,   // max ambient was ~7
    /*  2 */                  7,
    /*  3 */                  7,
    /*  4 */                  8,
    /*  5 */                  9,
    /*  6 */                  8,
    /*  7 */                  8,
    /*  8 */                  7,
    /*  9 */                  7,
    /* 10 */                  7,
    /* 11 */                  6,
    /* 12 */                  7,
    /* 13 */                  7,
    /* 14 */                  8,
    /* 15 */                  8,
    /* 16 */                  6,
    /* 17 */                  6,
    /* 18 */                  7,
    /* 19 */                  7,
    /* 20 */                  6,
    /* 21 */                  6,
    /* 22 */                  7,
    /* 23 */                  7,
    /* 24 */                  5,
    /* 25 */                  5,
    /* 26 */                  6,
    /* 27 */                  6,
    /* 28 */                  7,
    /* 29 */                  7,
    /* 30 */                  7,
    /* 31 */                  7
};

// ?????????? CHANGE?2: Raise flux threshold based on MUSIC data ??????????
// so only the huge one?frame gun?shot jump passes.
static const uint32_t FLUX_THRESHOLD = 900;

// ?????????? CHANGE?3: Raise active?bin count threshold ??????????
// Music lit at most ~5?7 bins at once.  Require at least 12 bins to fire.
static const uint8_t BIN_COUNT_THRESHOLD = 29;

// history of last frame?s magnitudes
static int16_t prev_mag[NUM_BINS];

void detect_init(void) {
    for (uint8_t i = 0; i < NUM_BINS; i++) {
        prev_mag[i] = 0;
    }
}

bool detect_shot(const int16_t mag[NUM_BINS]) {
    // ?? Step?1: Spectral flux (big sudden jump?) ??
    uint32_t flux = 0;
    for (uint8_t i = 1; i < NUM_BINS; i++) {
        flux += abs(mag[i] - prev_mag[i]);
    }

    // ?? Step?2: Active?bin count (broadband?) ??
    uint8_t active = 0;
    for (uint8_t i = 1; i < NUM_BINS; i++) {
        if ((uint16_t)mag[i] > noiseThreshold[i]) {
            active++;
        }
    }

    // ?? Step?3: Decision ??
    bool detected = (flux > FLUX_THRESHOLD)
                 && (active > BIN_COUNT_THRESHOLD);

    // ?? Step?4: Update history for next flux calc ??
    for (uint8_t i = 0; i < NUM_BINS; i++) {
        prev_mag[i] = mag[i];
    }
    
      // ?? DEBUG PRINT ??
    // prints: flux=XXXX active=YY ? SHOT or ?no?
    printf("flux=%4lu  active=%2u  => %s\r\n",
        flux, active,
        detected ? "SHOT" : "no");

    return detected;
}
