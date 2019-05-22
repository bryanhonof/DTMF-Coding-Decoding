#if !defined(UART_H)
#define UART_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <s6e2cc.h>
#include <mcu.h>
#include <mfs_hl.h>

#define UART_TX_BUFFSIZE         (128u)
#define UART_RX_BUFFSIZE         (256u)
#define UART_RX_BUFF_FILL_LVL    (1u) // one character to read

static uint8_t au8UartTxBuf[UART_TX_BUFFSIZE];
static uint8_t au8UartRxBuf[UART_RX_BUFFSIZE];

void UART_write(char* message);
void UART_init(stc_mfs_hl_uart_config_t *cfgStruct);

#endif /*UART_H*/
