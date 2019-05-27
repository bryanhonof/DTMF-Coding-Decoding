#if !defined(STATEMACHINE_H)
#define STATEMACHINE_H

#include <stdlib.h>
#include <stdint.h>

#include <mcu.h>
#include <audio.h>
#include <mfs_hl.h>
#include <s6e2cc.h>
#include <pdl_header.h>
#include <uart.h>

typedef struct freqPair_s {
    float32_t bass;
    float32_t high;
} freqPair_t;

typedef enum keyPad_e {
    NUL     = 0x00u,
    ZERO    = 0x30u,
    ONE     = 0x31u,
    TWO     = 0x32u,
    THREE   = 0x33u,
    FOUR    = 0x34u,
    FIVE    = 0x35u,
    SIX     = 0x36u,
    SEVEN   = 0x37u,
    EIGHT   = 0x38u,
    NINE    = 0x39u,
    ASTERIX = 0x2Au,
    HASHTAG = 0x23u,
    A       = 0x41u,
    B       = 0x42u,
    C       = 0x43u,
    D       = 0x44u
} keyPad_t;

static freqPair_t pairTable[] =
{
    [NUL]     = { .bass = 0.00f,   .high = 0.00f    },
    
    [ONE]     = { .bass = 697.00f, .high = 1209.00f },
    [TWO]     = { .bass = 697.00f, .high = 1336.00f },
    [THREE]   = { .bass = 697.00f, .high = 1477.00f },
    
    [FOUR]    = { .bass = 770.00f, .high = 1209.00f },
    [FIVE]    = { .bass = 770.00f, .high = 1336.00f },
    [SIX]     = { .bass = 770.00f, .high = 1477.00f },
    
    [SEVEN]   = { .bass = 852.00f, .high = 1209.00f },
    [EIGHT]   = { .bass = 852.00f, .high = 1336.00f },
    [NINE]    = { .bass = 852.00f, .high = 1477.00f },
    
    [ASTERIX] = { .bass = 941.00f, .high = 1209.00f },
    [ZERO]    = { .bass = 941.00f, .high = 1336.00f },
    [HASHTAG] = { .bass = 941.00f, .high = 1477.00f },

    [A]       = { .bass = 697.00f, .high = 1633.00f },
    [B]       = { .bass = 770.00f, .high = 1633.00f },
    [C]       = { .bass = 852.00f, .high = 1633.00f },
    [D]       = { .bass = 941.00f, .high = 1633.00f }
};

void startMachine(void);

#endif /*STATEMACHINE_H*/
