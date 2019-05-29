// fir_prbs_CMSIS_intr.c 
#include "audio.h"
#include "uart.h"
#include "window.h"
#include <stdint.h>
#include <arm_math.h>
#include <stdio.h>

#define N_SAMPLES  (512u)
#define FFT_SIZE   (N_SAMPLES * 2u)

static arm_rfft_instance_q31 fft                = { 0u };
static uint32_t samples                         = 0u;
static q31_t  sampleBuf[N_SAMPLES]              = { 0u };
static q31_t  tempBuf[FFT_SIZE]                 = { 0u };

void
I2S_HANDLER(void)
{
    audio_IN  = i2s_rx();
    
    sampleBuf[samples] = audio_IN;
    ++samples;
    
    if(samples >= N_SAMPLES)
    {
        i2s_stop();
    }
    
    i2s_tx(audio_IN);
    
    return;
}

static char Value[2] = { '\0' };

int32_t
main(void)
{
    stc_mfs_hl_uart_config_t stcMfsHlUartCfg;
    static q31_t maxVal = 0u;
    static uint32_t maxIndex[2] = { 0u };
    arm_status status = ARM_MATH_SUCCESS;
    uint32_t i = 0u;
    
    
    status = arm_rfft_init_q31(&fft, N_SAMPLES, 0u, 1u);
    if(status != ARM_MATH_SUCCESS)
    {
        while(1){};
    }
    
    UART_init(&stcMfsHlUartCfg);
    audio_init(hz8000, line_in, intr, I2S_HANDLER); 
 
    while(1)
    {
        if(samples >= N_SAMPLES)
        {
            for(i = 0u; i < N_SAMPLES; ++i)
            {
                sampleBuf[i] = (q31_t)(window[i] * sampleBuf[i]);
            }
            
            arm_rfft_q31(&fft, sampleBuf, tempBuf);
            
            arm_max_q31(tempBuf, FFT_SIZE, &maxVal, &maxIndex[0]);
            maxIndex[0] /= 2u;
            
            for(i = 0u; i < FFT_SIZE; ++i)
            {
                if(tempBuf[i] == maxVal)
                {
                    tempBuf[i] = 0u;
                }
            }
            
            arm_max_q31(tempBuf, FFT_SIZE, &maxVal, &maxIndex[1]);
            maxIndex[1] /= 2u;
            
            switch((maxIndex[0] + maxIndex[1]))
            {
                case 121:
                    /* Got 1 */
                    Value[0] = '1';
                    break;
                case 129:
                    /* Got 2 */
                    Value[0] = '2';
                    break;
                case 136:
                    /* Got 3 */
                    Value[0] = '3';
                    break;
                
                case 126:
                    //got 4
                    Value[0] = '4';
                    break;
                case 134:
                    //got 5
                    Value[0] = '5';
                    break;
                case 169:
                    //got 6
                    Value[0] = '6';
                    break;
                
                case 131:
                    //got 7
                    Value[0] = '7';
                    break;
                case 139:
                    //got 8
                    Value[0] = '8';
                    break;
                case 146:
                    //got 9
                    Value[0] = '9';
                    break;
                
                case 137:
                    //got *
                    Value[0] = 'A';
                    break;
                case 145:
                    //got 0
                    Value[0] = '0';
                    break;
                case 152:
                    //got #
                    Value[0] = 'H';
                    break;
                    
                default:
                    break;
            }
            
            UART_write((const char *)Value);
            
            samples = 0u;
            i2s_start();
        }
    }
}

