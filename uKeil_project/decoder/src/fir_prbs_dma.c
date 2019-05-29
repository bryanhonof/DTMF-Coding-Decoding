#include "audio.h"
#include "uart.h"
#include "window.h"
#include <stdint.h>
#include <arm_math.h>
#include <stdio.h>

static arm_rfft_instance_q31 fft                = { 0u };
static stc_mfs_hl_uart_config_t stcMfsHlUartCfg = { 0u };

void
DMA_HANDLER(void)
{
//    if(dstc_state(0u))
//    {
//        switch(tx_proc_buffer)
//        {
//            case PONG:
//                dstc_src_memory(0u, (uint32_t)(&dma_tx_buffer_pong));
//                tx_proc_buffer = PING; 
//                break;
//            case PING:
//                dstc_src_memory(0u, (uint32_t)(&dma_tx_buffer_ping));
//                tx_proc_buffer = PONG; 
//                break;
//            default:
//                break;
//        }
//        tx_buffer_empty = 1u;
//    }
    
    if (dstc_state(1u))
    {
        switch(rx_proc_buffer)
        {
            case PONG:
                dstc_dest_memory(1u, (uint32_t)(&dma_rx_buffer_pong));
                rx_proc_buffer = PING;
                break;
            case PING:
                dstc_dest_memory(1u, (uint32_t)(&dma_rx_buffer_ping));
                rx_proc_buffer = PONG;
                break;
            default:
                break;
        }
        rx_buffer_full = 1u;
    }
    
    dstc_reset(0u);
    dstc_reset(1u);
    
    return;
}

void
proces_buffer(void) 
{
    uint32_t  i     = 0u;
    //volatile uint32_t *txbuf = NULL;
    volatile uint32_t *rxbuf = NULL;
    static q31_t tempBuf[DMA_BUFFER_SIZE * 2] = { 0u };
    static q31_t magBuf[DMA_BUFFER_SIZE] = { 0u };
    char printBuf[128] = { '\0' };
    q31_t maxVal = 0u;
    uint32_t maxIndex = 0u;
    
    //txbuf = ((tx_proc_buffer == PING) ? (dma_tx_buffer_ping) : (dma_tx_buffer_pong));
    rxbuf = ((rx_proc_buffer == PING) ? (dma_rx_buffer_ping) : (dma_rx_buffer_pong));
    
    arm_rfft_q31(&fft, (q31_t *)rxbuf, tempBuf);
    //arm_abs_q31(tempBuf, tempBuf, (DMA_BUFFER_SIZE * 2));
    arm_max_q31(tempBuf, (DMA_BUFFER_SIZE * 2), &maxVal, &maxIndex);
    
    //sprintf(printBuf, "val = %d, index = %d\r\n", maxVal, maxIndex);
    //UART_write((const char *)printBuf);
    
    //for(i = 0u; i < DMA_BUFFER_SIZE; ++i)
    //{
    //    txbuf[i] = ((rxbuf[i] << 16u & 0xFFFF0000u) + (rxbuf[i] & 0x0000FFFFu));
    //}
    
    //tx_buffer_empty = 0u;
    rx_buffer_full  = 0u;
    
    return;
}

int32_t
main(void)
{
    arm_status status = ARM_MATH_SUCCESS;
    
    status = arm_rfft_init_q31(&fft, 256, 0u, 1u);
    if(status != ARM_MATH_SUCCESS)
    {
        while(1){};
    }
    
    //UART_init(&stcMfsHlUartCfg);
    audio_init(hz8000, line_in, dma, DMA_HANDLER);

    while(1)
    {
        while (!(rx_buffer_full)){};
        proces_buffer();
    }
    
    /* return 0u; */
}
