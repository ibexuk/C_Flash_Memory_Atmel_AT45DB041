/*
IBEX UK LTD http://www.ibexuk.com
Electronic Product Design Specialists
RELEASED SOFTWARE

The MIT License (MIT)

Copyright (c) 2013, IBEX UK Ltd, http://ibexuk.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//Project Name:		ATMEL AT45DB041 DRIVER






//*************************************
//*************************************
//********** DRIVER REVISION **********
//*************************************
//*************************************
//
//V1.00
//- Original release



//*********************************
//*********************************
//********** USAGE NOTES **********
//*********************************
//*********************************

//Include this header file in any .c files within your project from which you wish to use it's functions.

//---------------------------------------
//----- SPI PORT SETUP REQUIREMENTS -----
//---------------------------------------
//Up to 33MHz SPI port (for continuous read which is limiting speed spec of this device)
//Data latched on rising edge of SCK, data is output on the falling edge of SCK.

/*
Example setup for PIC18 family:-
	//----- SETUP SSP1 -----
	//Used for: Flash memory IC
	//CLK idle in low state
	//AT45DB041 clocks in data on the rising edge, outputs data on the falling edge
	SSPSTAT = 0b01000000;
	SSPCON1 = 0b00100000;		//Fosc / 4 = 10MHz
*/

/*
Example setup for DSPIC33FJ family:-
	//----- SETUP SPI 1 -----
	//Used for: Flash memory IC
	w_temp = SPI1BUF;
	SPI1STAT = 0;
	SPI1CON1 = 0x0320;				//SPI in master mode
									//Data is valid on the rising edge of the clock (Transmit occurs on transition from active to idle clock state)
									//Clock low in idle bus state
	SPI1CON1bits.PPRE1 = 1;			//Prescallers 4:1 1:1 = 10MHz (max possible from this device)
	SPI1CON1bits.PPRE0 = 0;
	SPI1CON1bits.SPRE2 = 1;
	SPI1CON1bits.SPRE1 = 1;
	SPI1CON1bits.SPRE0 = 1;
	SPI1CON2 = 0;
	SPI1STATbits.SPIEN = 1;			//Enable the port
*/

/*
Example setup for PIC24HJ family:-
	//----- SETUP SPI 1 -----
	//Used for: Flash memory IC
	w_temp = SPI1BUF;
	SPI1STAT = 0;
	SPI1CON1 = 0b0000001100100001;	//SPI in master mode (SPI1STATbits.SPIEN must be 0 to write to this register)
									//Data is valid on the rising edge of the clock (Transmit occurs on transition from active to idle clock state)
									//Clock low in idle bus state
	SPI1CON1bits.PPRE1 = 1;			//Prescallers 4:1 1:1 = 10MHz (max possible from this device)
	SPI1CON1bits.PPRE0 = 0;
	SPI1CON1bits.SPRE2 = 1;
	SPI1CON1bits.SPRE1 = 1;
	SPI1CON1bits.SPRE0 = 1;
	SPI1CON2 = 0;
	SPI1STATbits.SPIEN = 1;			//Enable the port
*/
/*
Example setup for PIC32 family:-
	//----- SETUP SPI 1 -----
	//Used for: Flash Memory
	//Idle low, active high
	//Transmit on active to idle (falling edge)
	SpiChnOpen(SPI_CHANNEL1, (SPI_CON_MSTEN | SPI_OPEN_CKE_REV | SPI_CON_MODE8), 2);       	//40MHz fpb / 2 = 20MHz
*/








//##### ADDING A FUNCTION TO READ INDVIDIUAL BYTES USING A LOCAL BUFFER FOR A PAGE OF MEMORY#####
//Add to INTERNAL ONLY MEMORY DEFINITIONS:-
/*
DWORD flash_buffer_address = 0xffffffff;
BYTE flash_buffer[256];
*/

//Add to fucntion definitions
/*
BYTE flash_read_byte (DWORD address);
extern BYTE flash_read_byte (DWORD address);
*/

//Add this function:-
//*******************************
//*******************************
//********** READ BYTE **********
//*******************************
//*******************************
/*
BYTE flash_read_byte (DWORD address)
{
	//LOAD PAGE IF NECESSARY
	if ((flash_buffer_address & 0xffffff00) != (address & 0xffffff00))
	{
		flash_buffer_address = (address & 0xffffff00);
	
		flash_read_page(flash_buffer_address, &flash_buffer[0]);
	}
	return(flash_buffer[(address & 0x000000ff)]);
}
*/





//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef FLASH_C_INIT		//Do only once the first time this file is used
#define	FLASH_C_INIT

//----------------------------
//----- FLASH MEMORY MAP -----
//----------------------------
//AT45DB041,  FLASH MEMORY
//524288bytes x 8 (512KB)
//2,048 pages of 256 bytes (To avoid non binary complexities this driver only uses 256 bytes of each page instead of the 264 which are actually available)
//0x00000 - 0x7ffff
#define	FLASH_END_ADDRESS			0x7ffff
#define	FLASH_BLOCK_SIZE			0x800			//A block is 8 pages


//##################
//##### PIC 18 #####
//##################
/*
#define	FLASH_CLR_WDT		ClrWdt()				//Clear watchdog timer

//----- PINS -----
#define	FLASH_CS(state)		LATCbits.LATC0 = state

//----- SPI BUS READ WRITE DEFINES -----
#define	FLASH_SPI_BUF_FULL				PIR1bits.SSPIF				//Flag that the SPI receive buffer contains a received byte, also signifying that transmit is complete
#define FLASH_SPI_READ_BUFFER			SSPBUF						//Read byte from SPI bus
#define FLASH_SPI_WRITE_BUFFER(data)	SSPBUF = data				//Write byte to SPI bus

#define	DO_CS_HIGH_DELAY	Nop(); Nop()										//Delay of at least 50nS
*/


//#####################
//##### DSPIC33FJ #####
//#####################
/*
#define	FLASH_CLR_WDT		ClrWdt()				//Clear watchdog timer

//----- PINS -----
#define	FLASH_CS(state)		_LATF5 = state

//----- SPI BUS READ WRITE DEFINES -----
#define	FLASH_SPI_BUF_FULL				IFS0bits.SPI1IF							//Flag that the SPI receive buffer contains a received byte, also signifying that transmit is complete
#define FLASH_SPI_READ_BUFFER			SPI1BUF									//Read byte from SPI bus
#define FLASH_SPI_WRITE_BUFFER(data)	IFS0bits.SPI1IF = 0; SPI1BUF = data		//Write byte to SPI bus

#define	DO_CS_HIGH_DELAY	Nop(); Nop()										//Delay of at least 50nS
*/


//###################
//##### PIC24HJ #####
//###################
#define	FLASH_CLR_WDT		ClrWdt()				//Clear watchdog timer

//----- PINS -----
#define	FLASH_CS(state)		_LATG15 = state

//----- SPI BUS READ WRITE DEFINES -----
#define	FLASH_SPI_BUF_FULL				IFS0bits.SPI1IF							//Flag that the SPI receive buffer contains a received byte, also signifying that transmit is complete
#define FLASH_SPI_READ_BUFFER			SPI1BUF									//Read byte from SPI bus
#define FLASH_SPI_WRITE_BUFFER(data)	IFS0bits.SPI1IF = 0; SPI1BUF = data		//Write byte to SPI bus

#define	DO_CS_HIGH_DELAY	Nop(); Nop()										//Delay of at least 50nS


//#################
//##### PIC32 #####
//#################
/*
#define	FLASH_CLR_WDT		ClearWDT()				//Clear watchdog timer

//----- PINS -----
#define	FLASH_CS(state)		(state ? mPORTCSetBits(BIT_14) : mPORTCClearBits(BIT_14))

//----- SPI BUS READ WRITE DEFINES -----
#define	FLASH_SPI_BUF_FULL				SpiChnDataRdy(SPI_CHANNEL1)				//>0 when the SPI receive buffer contains a received byte, also signifying that transmit is complete
#define FLASH_SPI_READ_BUFFER			(BYTE)SpiChnReadC(SPI_CHANNEL1)			//Register to read last received byte from
#define FLASH_SPI_WRITE_BUFFER(data)	SpiChnWriteC(SPI_CHANNEL1, (unsigned int)data)	//Macro to write a byte and start transmission over the SPI bus

#define	DO_CS_HIGH_DELAY	Nop(); Nop()										//Delay of at least 50nS
*/


#endif		//#ifndef FLASH_C_INIT




//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef FLASH_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------
BYTE flash_read_status (void);


//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)
BYTE flash_check_present (void);
void flash_erase_all (void);
void flash_write_page (DWORD address, BYTE *buffer, BYTE erase_first);
void flash_read_page (DWORD address, BYTE *buffer);
BYTE flash_test_memory (void);


#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------
extern BYTE flash_check_present (void);
extern void flash_erase_all (void);
extern void flash_write_page (DWORD address, BYTE *buffer, BYTE erase_first);
extern void flash_read_page (DWORD address, BYTE *buffer);
extern BYTE flash_test_memory (void);


#endif




//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef FLASH_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------



//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)



#else
//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------



#endif



