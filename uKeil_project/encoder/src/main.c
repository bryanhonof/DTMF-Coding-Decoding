// sine_intr.c 

#include "audio.h"

#define SAMPLING_FREQ  (8000u)
#define N_THETA_ARRAY  (2u)

float32_t frequency_table[2] = { 697.0f, 1209.0f };
float32_t amplitude          = 5000.0;

/****** I2S Interruption Handler *****/
void
I2S_HANDLER(void)
{
		static volatile int16_t   audio_chR                      = 0u;    
		static volatile int16_t   audio_chL                      = 0u;
		static          float32_t theta_increment[N_THETA_ARRAY] = { 0u };
		static          float32_t theta[N_THETA_ARRAY]           = { 0.0f };
		                uint16_t  i                              = 0u;
	
		audio_IN  = i2s_rx();	
		audio_chL = (audio_IN & 0x0000FFFF);       
		audio_chR = ((audio_IN >> 16u) & 0x0000FFFF); 
		
	
		for (i = 0; i < N_THETA_ARRAY; i++) {
				theta_increment[i] = ((2 * PI * frequency_table[i]) / SAMPLING_FREQ);
				theta[i]          += theta_increment[i];
			
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

int
main(void)
{
		audio_init ( hz8000, line_in, intr, I2S_HANDLER);
		
		for (;;) {
				/* Do nothing. */
		}
		
		/* Should not reach this. */
		/* return 0; */
}
