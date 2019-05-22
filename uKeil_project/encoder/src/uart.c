#include <uart.h>

void
UART_init(stc_mfs_hl_uart_config_t *cfgStruct)
{		
		// Setup the Serial communication
		// Baud rate : 115200 bps (230400, 460800, 921600)
		cfgStruct->u32DataRate = 115200; 
		// Bit direction : LSB first
		cfgStruct->bBitDirection = FALSE; 
		// Inverted serial data format : NRZ
		cfgStruct->bSignalSystem = FALSE; 
		// Inverted serial data format : NRZ
		cfgStruct->bHwFlow = FALSE;
		// Inverted serial data format : NRZ
		cfgStruct->pu8TxBuf = au8UartTxBuf;
		// Use the reception buffer in the driver (*2)
		cfgStruct->pu8RxBuf = au8UartRxBuf;
		// Size of transmit buffer (for *1)  : 128bytes
		cfgStruct->u16TxBufSize = UART_TX_BUFFSIZE;
		// Size of reception buffer (for *2) : 256bytes
		cfgStruct->u16RxBufSize = UART_RX_BUFFSIZE; 
		// Unread counts of reception buffer to call RX Callback function : 1byte
		cfgStruct->u16RxCbBufFillLvl = UART_RX_BUFF_FILL_LVL;
		// Uart mode : Normal mode
		cfgStruct->u8UartMode = MfsUartNormal;
		// Parity    : none
		cfgStruct->u8Parity = MfsParityNone;
		// Stop bit  : 1bit
		cfgStruct->u8StopBit = MfsOneStopBit;
		// Data bits : 8bit
		cfgStruct->u8CharLength = MfsEightBits;
		// FIFO      : not use
		cfgStruct->u8FifoUsage = MfsHlUseNoFifo; 
		// Callback function : if NULL ==> interrupt not used
		cfgStruct->pfnRxCb = NULL; 
		// Callback function : if NULL ==> interrupt not used
		cfgStruct->pfnTxCb = NULL; 
		
		
		//************ Configure UART Serial0 *********************
		// Disable Analog input (P21:SIN0_0/AN17, P22:SOT0_0/AN16)
		FM4_GPIO->ADE = 0;
		// Set UART Ch0_0 Port (SIN, SOT)
		FM4_GPIO->PFR2 = FM4_GPIO->PFR2 | 0x0006;
		FM4_GPIO->EPFR07 = FM4_GPIO->EPFR07 | 0x00000040;
		//*********************************************************
		
		// Initialize the MFS ch.0 as UART
		if (Ok != Mfs_Hl_Uart_Init(&MFS0, cfgStruct))
		{
				while(1);
		}
		
		return;
}

void
UART_write(char* message)
{
		Mfs_Hl_Write(&MFS0, (uint8_t*) message, strlen(message), FALSE, FALSE);
	
		return;
}
