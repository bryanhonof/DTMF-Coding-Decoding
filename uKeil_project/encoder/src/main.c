#include <stdlib.h>
#include <stdint.h>

#include <mcu.h>
#include <audio.h>
#include <mfs_hl.h>
#include <s6e2cc.h>
#include <pdl_header.h>
#include <uart.h>
#include "stateMachine.h"

#define SAMPLING_FREQ  (8000u)
#define N_THETA_ARRAY  (2u)

float32_t frequency_table[2] = { 697.0f, 1209.0f };
float32_t amplitude          = 5000.00f;
freqPair_t *currentPair = &pairTable[NUL];

/****** I2S Interruption Handler *****/
void
I2S_HANDLER(void)
{
    static volatile int16_t   audio_chR                      = 0u;    
    static volatile int16_t   audio_chL                      = 0u;
    static          float32_t theta[N_THETA_ARRAY]           = { 0.0f };
                    uint16_t  i                              = 0u;
    
    audio_IN  = i2s_rx();    
    audio_chL = (audio_IN          & 0x0000FFFF);       
    audio_chR = ((audio_IN >> 16u) & 0x0000FFFF); 
    
    theta[0] += ((2 * PI * currentPair->bass) / SAMPLING_FREQ);
    theta[1] += ((2 * PI * currentPair->high) / SAMPLING_FREQ);
    
    for (i = 0; i < N_THETA_ARRAY; i++) {
        if (theta[i] > (2 * PI)) {
            theta[i] -= (2 * PI);
        }
    }
    
    audio_chL = (int16_t)(amplitude * (arm_sin_f32(theta[0]) + arm_sin_f32(theta[1])));
    audio_chR = audio_chL;
        
    audio_OUT = ((audio_chR<<16 & 0xFFFF0000)) + (audio_chL & 0x0000FFFF);    
    
    i2s_tx(audio_OUT);
    
    return;
}

int32_t
main(void)
{
    stc_mfs_hl_uart_config_t stcMfsHlUartCfg;
    
    UART_init(&stcMfsHlUartCfg);
    audio_init(hz8000, line_in, intr, I2S_HANDLER);
    i2s_stop();
    
    UART_write((const char *)"Start of application\r\n");
    UART_write((const char *)"Please enter one of the following characters:\r\n");
    UART_write((const char *)"1  2  3  A\r\n");
    UART_write((const char *)"4  5  6  B\r\n");
    UART_write((const char *)"7  8  9  C\r\n");
    UART_write((const char *)"*  0  #  D\r\n");
    UART_write((const char *)">");
        
    for (;;) {
        startMachine();
    }
    
    /* Should not reach this. */
    /* return 0; */
}
