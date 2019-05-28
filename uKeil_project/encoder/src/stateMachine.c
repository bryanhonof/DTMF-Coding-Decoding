#include "stateMachine.h"

#include <stdio.h>

#define VALID_KEYPAD_CODE  ((au8UartRxBuf[0] != NUL)    && \
                            (au8UartRxBuf[0] == ZERO    || \
                             au8UartRxBuf[0] == ONE     || \
                             au8UartRxBuf[0] == TWO     || \
                             au8UartRxBuf[0] == THREE   || \
                             au8UartRxBuf[0] == FOUR    || \
                             au8UartRxBuf[0] == FIVE    || \
                             au8UartRxBuf[0] == SIX     || \
                             au8UartRxBuf[0] == SEVEN   || \
                             au8UartRxBuf[0] == EIGHT   || \
                             au8UartRxBuf[0] == NINE    || \
                             au8UartRxBuf[0] == ASTERIX || \
                             au8UartRxBuf[0] == HASHTAG || \
                             au8UartRxBuf[0] == A       || \
                             au8UartRxBuf[0] == B       || \
                             au8UartRxBuf[0] == C       || \
                             au8UartRxBuf[0] == D          ))

extern freqPair_t *currentPair;

typedef enum states_e {
    ST_GETCHAR,
    ST_SEND_DTFM,
    ST_SEND_ERROR
} states_t;

void
startMachine(void)
{
    static states_t state     = ST_GETCHAR;
    static uint16_t readcount = 0u;
#if defined(DEBUG_)
    static char     buf[50]  = { '\0' };
#endif
    
    switch(state)
    {
        case ST_GETCHAR:
        {
            if (Ok != Mfs_Hl_Read(&MFS0, au8UartRxBuf, &readcount, 1, FALSE))
            {
                state = ST_SEND_ERROR;
            } 
            else if(VALID_KEYPAD_CODE) 
            {
                state = ST_SEND_DTFM;
                UART_write((const char *)au8UartRxBuf);
                currentPair = &pairTable[(keyPad_t)au8UartRxBuf[0]];
#if defined(DEBUG_)
                sprintf(buf, "\r\n.bass = %.2f\r\n.high = %.2f", currentPair->bass, currentPair->high);
                UART_write(buf);
#endif
            }
                
            break;
        }
        case ST_SEND_DTFM:
        {
            state = ST_GETCHAR;
            
            UART_write((const char *)"\r\nSending...");
            i2s_start();
            delay_ms(500u);
            i2s_stop();
            UART_write("\r\n>");
            
            au8UartRxBuf[0] = '\0';
            currentPair = &pairTable[NUL];
            
            break;
        }
        case ST_SEND_ERROR:
        {
            state = ST_GETCHAR;
            
            Mfs_Hl_Write(&MFS0, (uint8_t*) "Read Error\r\n", 12, FALSE, FALSE);
            UART_write("\r\n>");
            
            break;
        }
        default:
        {
            state = ST_GETCHAR;
            UART_write((const char *)"Unexpected state in state machine.\r\n");
            
            break;
        }
    }
    
    return;
}

/* EOF */
