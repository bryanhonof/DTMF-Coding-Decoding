// fir_intr.c 

#include "audio.h"

volatile int16_t audio_chR=0;    
volatile int16_t audio_chL=0;    

#include "bp1750.h"

float32_t x[N];
int16_t k = 0;


void I2S_HANDLER(void) {                  

float32_t yn = 0.0;
int i;
	
audio_IN = i2s_rx();	
audio_chL = (audio_IN & 0x0000FFFF);       
audio_chR = ((audio_IN >>16)& 0x0000FFFF); 

	  gpio_set(TEST_PIN, HIGH);
    x[k++] = (float32_t)(audio_chL);
    if (k >= N) k = 0;
    for (i=0 ; i<N ; i++)
    {
      yn += h[i]*x[k++];
			if (k >= N) k = 0;
		}
 	  audio_chL= (int16_t)(yn);
    gpio_set(TEST_PIN, LOW);
		
audio_OUT = ((audio_chL <<16 & 0xFFFF0000)) + (audio_chL & 0x0000FFFF);	
i2s_tx(audio_OUT);

}

int main(void)
{
  gpio_set_mode(TEST_PIN, Output);
  audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}
