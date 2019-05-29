// fir_prbs_CMSIS_dma.c

#include "audio.h"

volatile int16_t audio_chR=0;    
volatile int16_t audio_chL=0;    

#include "bp1750.h"

float32_t x[DMA_BUFFER_SIZE], y[DMA_BUFFER_SIZE], state[N+(DMA_BUFFER_SIZE)-1];

arm_fir_instance_f32 S;


void DMA_HANDLER (void)
{
      if (dstc_state(0)){ //check interrupt status on channel 0

					if(tx_proc_buffer == (PONG))
						{
						dstc_src_memory (0,(uint32_t)&(dma_tx_buffer_pong));    //Soucrce address
						tx_proc_buffer = PING; 
						}
					else
						{
						dstc_src_memory (0,(uint32_t)&(dma_tx_buffer_ping));    //Soucrce address
						tx_proc_buffer = PONG; 
						}
				tx_buffer_empty = 1;                                        //Signal to main() that tx buffer empty					
       
				dstc_reset(0);			                                        //Clean the interrup flag
    }
    if (dstc_state(1)){ //check interrupt status on channel 1

					if(rx_proc_buffer == PONG)
					  {
						dstc_dest_memory (1,(uint32_t)&(dma_rx_buffer_pong));   //Destination address
						rx_proc_buffer = PING;
						}
					else
						{
						dstc_dest_memory (1,(uint32_t)&(dma_rx_buffer_ping));   //Destination address
						rx_proc_buffer = PONG;
						}
					rx_buffer_full = 1;   
						
				dstc_reset(1);		
    }
}

void proces_buffer(void) 
{
	int ii;
  uint32_t *txbuf, *rxbuf;

  if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
  else txbuf = dma_tx_buffer_pong; 
  if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
  else rxbuf = dma_rx_buffer_pong; 
			
  for (ii=0 ; ii<(DMA_BUFFER_SIZE) ; ii++){
    x[ii] = (float32_t)(10000.50f);
		}
  arm_fir_f32(&S,x,y,DMA_BUFFER_SIZE);
	
  for (ii=0 ; ii<(DMA_BUFFER_SIZE) ; ii++){
		*txbuf++ = (((short)(y[ii])<<16 & 0xFFFF0000)) + ((short)(y[ii]) & 0x0000FFFF);
		} 
  
  tx_buffer_empty = 0;
  rx_buffer_full = 0;
}

int main (void) {                          //Main function

	arm_fir_init_f32(&S,N,h,state,DMA_BUFFER_SIZE/2);
  audio_init ( hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
		proces_buffer();
	}
}
