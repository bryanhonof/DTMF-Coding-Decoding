// average_intr.c 

#include "audio.h"

volatile int16_t audio_chR=0;    //16 bits audio data channel right
volatile int16_t audio_chL=0;    //16 bits audio data channel left

#define N 5
float32_t h[N];
float32_t x[N];
int16_t k = 0;
int16_t i;
/****** I2S Interruption Handler *****/

void I2S_HANDLER(void) {                   //I2S interruption	

float32_t yn = 0.0;
	
audio_IN = i2s_rx();	
audio_chL = (audio_IN & 0x0000FFFF);       //Separate 16 bits channel left
audio_chR = ((audio_IN >>16)& 0x0000FFFF); //Separate 16 bits channel right

    x[k++] = (float32_t)(audio_chL);
    if (k >= N) k = 0;
    for (i=0 ; i<N ; i++)
    {
      yn += h[i]*x[k++];
			if (k >= N) k = 0;
		}
 	  audio_chL= (int16_t)(yn);
	
audio_OUT = ((audio_chL <<16 & 0xFFFF0000)) + (audio_chL & 0x0000FFFF);	//Put the two channels toguether again
i2s_tx(audio_OUT);

}

int main(void)
{
  for (i=0 ; i<N ; i++) h[i] = 1.0/N;
  audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}
