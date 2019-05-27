#include <stdio.h>
#include "arm_math.h"
#include "audio.h"
#include "s6e2cc.h"  

#define  L 				512

volatile 	int16_t audio_chR			=		 0;    //16 bits audio data channel right
volatile 	int16_t audio_chL			=		 0;    //16 bits audio data channel left
q31_t 		inSignalQ31								 [L];
q31_t 		fftSignalQ31							 [2*L];
q31_t 		fftMagnitudeQ31						 [2];
q31_t 		hanning_window_q31				 [L];


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
	 int i;

	 arm_rfft_instance_q31 DTMF_RFFT;
   arm_cfft_radix4_instance_q31 DTMF_CFFT;	
	 gpio_set_mode(TEST_PIN,Output);

	 audio_init ( hz48000, line_in, intr, I2S_HANDLER);

   /* ----------------------------------------------------------------------
   ** Process with Q31 FFT
   ** ------------------------------------------------------------------- */

   // Create the Hanning window.  This is usually done once at the
   // start of the program.

  for(i=0; i<L; i++) {
      hanning_window_q31[i] = 
          (q31_t) (0.5f * 2147483647.0f * (1.0f - cosf(2.0f*PI*i / L)));
  }

	for(;;){
		 // Apply the window to the input buffer
		 arm_mult_q31(hanning_window_q31, inSignalQ31, inSignalQ31, L);

		 arm_rfft_init_q31(&DTMF_RFFT, 512, 0, 1);
		 //arm_rfft_init_q31(&DTMF_RFFT, &DTMF_CFFT, 512, 0, 1);
		 // Compute the FFT
		 arm_rfft_q31(&DTMF_RFFT, inSignalQ31, fftSignalQ31);

		 arm_cmplx_mag_q31(fftSignalQ31, fftMagnitudeQ31, L);
	}
}
