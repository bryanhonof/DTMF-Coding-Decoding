#include "stm32f4xx.h" 
#include <stdio.h>
#include "arm_math.h"

#define 	L 					512
#define 	BLOCKSIZE 	8

float 	inSignalF32								[L];
float 	outSignalF32							[L];
float 	biquad_coeffs_f32[5] 	= 	{0.01f, 0.0f, -0.01f, 1.690660431255413f, -0.9801f};
float 	biquad_state_f32[2] 	= 	{0};

/*----------------------------------------------------------------------------
main program
*----------------------------------------------------------------------------*/

int main (void)  {               /* execution starts here */
   int i, samp;
   arm_biquad_cascade_df2T_instance_f32 DTMF_BIQUAD;

   /* ----------------------------------------------------------------------
   ** Process with a floating-point Biquad filter
   ** ------------------------------------------------------------------- */

   arm_biquad_cascade_df2T_init_f32(&DTMF_BIQUAD, 1, biquad_coeffs_f32, biquad_state_f32);

   for(samp = 0; samp < L; samp += BLOCKSIZE) {

      arm_biquad_cascade_df2T_f32 (&DTMF_BIQUAD, inSignalF32 + samp, 
                                   outSignalF32 + samp, BLOCKSIZE);

   }
}
