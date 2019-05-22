#include <stdio.h>
#include "arm_math.h"
#include "audio.h"
#include "s6e2cc.h"  

#define L 				512
#define SR 				8000
#define BLOCKSIZE 8

volatile 	int16_t audio_chR			=		 0;    //16 bits audio data channel right
volatile 	int16_t audio_chL			=		 0;    //16 bits audio data channel left
float 	 	biquad_coeffs_f32[5] 	= 	{0.01f, 0.0f, -0.01f, 1.690660431255413f, -0.9801f};
float 	 	biquad_state_f32[2] 	= 	{0};
float 		inSignalF32								[L];
float 		outSignalF32							[L];

void I2S_HANDLER(void) {   /****** I2S Interruption Handler *****/

gpio_toggle(TEST_PIN);

audio_IN  	= 		i2s_rx();	
audio_chL 	= 	(	audio_IN 				& 0x0000FFFF);       //Separate 16 bits channel left
audio_chR 	= 	((audio_IN >>16)	& 0x0000FFFF); //Separate 16 bits channel right
audio_OUT 	= 	((audio_chR<<16 	& 0xFFFF0000)) + (audio_chL & 0x0000FFFF);	//Put the two channels toguether again
	
i2s_tx(audio_OUT);
}


/*----------------------------------------------------------------------------
main program
*----------------------------------------------------------------------------*/

int main (void)  {               /* execution starts here                     */
   gpio_set_mode(TEST_PIN,Output);
	
	 audio_init ( hz48000, line_in, intr, I2S_HANDLER);
	
	 int samp;

	 arm_biquad_cascade_df2T_instance_f32 DTMF_BIQUAD;

   /* ----------------------------------------------------------------------
   ** Process with a floating-point Biquad filter
   ** ------------------------------------------------------------------- */

   arm_biquad_cascade_df2T_init_f32(&DTMF_BIQUAD, 1, biquad_coeffs_f32, biquad_state_f32);

   for(samp = 0; samp < L; samp += BLOCKSIZE) {

      arm_biquad_cascade_df2T_f32(&DTMF_BIQUAD, inSignalF32 + samp, 
                                  outSignalF32 + samp, BLOCKSIZE);
   }
}
