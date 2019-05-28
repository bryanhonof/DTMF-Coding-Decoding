#include "audio.h"
#include "window.h"
#include "arm_math.h"
#include "arm_const_structs.h"

volatile int16_t audio_chR = 0u;
volatile int16_t audio_chL = 0u;

#define TEST_LENGTH_SAMPLES  512

void
DMA_HANDLER(void)
{
    if(dstc_state(0u))
    {
        if(tx_proc_buffer == (PONG))
        {
            dstc_src_memory(0u, (uint32_t)(&dma_tx_buffer_pong));
            tx_proc_buffer = PING; 
        }
        else
        {
            dstc_src_memory(0u, (uint32_t)(&dma_tx_buffer_ping));
            tx_proc_buffer = PONG; 
        }
        tx_buffer_empty = 1u;
       
        dstc_reset(0u);
    }
    
    if (dstc_state(1u))
    {
        if(rx_proc_buffer == PONG)
        {
            dstc_dest_memory(1u, (uint32_t)(&dma_rx_buffer_pong));
            rx_proc_buffer = PING;
        }
        else
        {
            dstc_dest_memory(1u, (uint32_t)(&dma_rx_buffer_ping));
            rx_proc_buffer = PONG;
        }
        rx_buffer_full = 1u;
        
        dstc_reset(1u);
    }
    
    return;
}

static float32_t testOutput[TEST_LENGTH_SAMPLES];

void
proces_buffer(void) 
{
    uint32_t  i                  = 0u;
    float32_t *txbuf              = NULL;
    float32_t *rxbuf              = NULL;
    float32_t maxValue           = 0.00f;
    uint32_t maxValueIndex       = 0u;
    static uint32_t fftSize      = 512u;
    static uint32_t ifftFlag     = 0u;
    static uint32_t doBitReverse = 1u;
    arm_status status            = ARM_MATH_SUCCESS;
    
    txbuf = ((tx_proc_buffer == PING) ? (dma_tx_buffer_ping) : (dma_tx_buffer_pong));
    rxbuf = ((rx_proc_buffer == PING) ? (dma_rx_buffer_ping) : (dma_rx_buffer_pong));
    
    for(i = 0u; i < DMA_BUFFER_SIZE; i++)
    {
        rxbuf[i] *= window[i];
    }
    
    arm_cfft_f32(&arm_cfft_sR_f32_len512, rxbuf, ifftFlag, doBitReverse);
    arm_cmplx_mag_f32(rxbuf, testOutput, fftSize);
    arm_max_f32(testOutput, fftSize, &maxValue, &maxValueIndex);
    testOutput[maxValueIndex] = 0u;
    
    for(i = 0u; i < DMA_BUFFER_SIZE; i++)
    {
        *txbuf++ = (((uint16_t)(testOutput[i]) << 16u & 0xFFFF0000u)) + ((uint16_t)(testOutput[i]) & 0x0000FFFFu);
    }
    
    tx_buffer_empty = 0u;
    rx_buffer_full  = 0u;
    
    return;
}

int32_t
main(void)
{
    audio_init(hz8000, line_in, dma, DMA_HANDLER);

    while(1)
    {
        while (!(rx_buffer_full && tx_buffer_empty)){};
        proces_buffer();
    }
    
    /* return 0u; */
}
