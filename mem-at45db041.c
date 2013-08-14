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



#include "main.h"					//Global data type definitions (see https://github.com/ibexuk/C_Generic_Header_File )


#define	FLASH_C
#include "mem-at45db041.h"



//***************************************
//***************************************
//********** TEST FLASH MEMORY **********
//***************************************
//***************************************
//An example of how to use this drivers functions.
//Also a useful function if you want to verfify flash memory driver is working properly.  Comment out if not required.
//Returns 1 if test passed, 0 if not.
/*
BYTE flash_test_memory (void)
{
	WORD count;
	DWORD_VAL dw_temp;
	BYTE buffer[256];


	//----- ERASE ALL FLASH -----
	flash_erase_all();
	
	for (count = 0; count < 256; count++)
		buffer[count] = 0x00;

	//----- WRITE ENTIRE MEMORY SPACE -----
	for (dw_temp.Val = 0; dw_temp.Val <= FLASH_END_ADDRESS; dw_temp.Val++)
	{
		FLASH_CLR_WDT;				//Clear our watchdog timer as this may take several seconds
		
		buffer[(dw_temp.v[0])] = (BYTE)(dw_temp.v[0] + dw_temp.v[1]);		//Write each byte within a page with a unique value and for page use an incrementing offset so that pages are not all the same
		
		if (dw_temp.v[0] == 0xff)
			flash_write_page(dw_temp.Val, &buffer[0], 0);
	}

	//----- READ AND VERIFY ENTIRE MEMORY SPACE -----
	for (dw_temp.Val = 0; dw_temp.Val <= FLASH_END_ADDRESS; dw_temp.Val++)
	{
		FLASH_CLR_WDT;				//Clear our watchdog timer as this may take several seconds
		
		if (dw_temp.v[0] == 0)
			flash_read_page(dw_temp.Val, &buffer[0]);
		
		if (buffer[(dw_temp.v[0])] != (BYTE)(dw_temp.v[0] + dw_temp.v[1]))
		{
			//----- FAILED -----
			return(0);
		}
	}
	//----- SUCCESS -----
	return(1);
}
*/



//*****************************************
//*****************************************
//********** IS FLASH IC PRESENT **********
//*****************************************
//*****************************************
//Useful to check flash chip is there and working
BYTE flash_check_present (void)
{
	BYTE data;
	data = flash_read_status();
	
	if ((data & 0x3c) == 0x1c)		//These status response bits are a fixed value that we should see
		return(1);					//Flash is present
	else
		return(0);
}



//****************************************
//****************************************
//********** ERASE FLASH MEMORY **********
//****************************************
//****************************************
//This function doesn't use the chip erase command as the errata recomends not using it due to reliability
//AT45DB041 Erase time: Typical 7.68Sec, Max 19.2Sec
void flash_erase_all (void)
{
	WORD block_id;
	BYTE b_temp ;


	for (block_id = 0; block_id < ((FLASH_END_ADDRESS + 1) / FLASH_BLOCK_SIZE); block_id++)
	{
		FLASH_CS(0);

		//----- ERASE BLOCK -----
		//A block is 8 pages = 0x800
		//Write command
		FLASH_SPI_WRITE_BUFFER(0x50);
		while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
			;
		b_temp = FLASH_SPI_READ_BUFFER;
	
		// 0 0 0 0 PA10 PA9 PA8 PA7
		FLASH_SPI_WRITE_BUFFER((BYTE)(block_id >> 4));
		while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
			;
		b_temp = FLASH_SPI_READ_BUFFER;
	
		//PA6 PA5 PA4 PA3 0 0 0 0 
		FLASH_SPI_WRITE_BUFFER((BYTE)((block_id & 0x0f) << 4));
		while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
			;
		b_temp = FLASH_SPI_READ_BUFFER;
		
		//0 0 0 0 0 0 0 0 
		FLASH_SPI_WRITE_BUFFER(0x00);
		while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
			;
		b_temp = FLASH_SPI_READ_BUFFER;
		
		//Start erase (low-to-high transition on CS pin)
		FLASH_CS(1);
		
		//Wait for busy to clear (AT45DB041 typical 30mS, max 75mS)
		while ((flash_read_status() & 0x80) == 0)
		{
			FLASH_CLR_WDT;
		}
	}
}



//********************************
//********************************
//********** WRITE PAGE **********
//********************************
//********************************
//Call with:
//	address			Binary byte address (must be the 1st byte of a page)
//	buffer			Array of 256 bytes of data to be written
//	erase_first		1 = erase buffer first, 0 = don't erase (if you've previously called flash_erase_all)
void flash_write_page (DWORD address, BYTE *buffer, BYTE erase_first)
{
	BYTE b_temp;
	BYTE count;

	FLASH_CS(0);

	//---------------------------
	//----- DO BUFFER WRITE -----
	//---------------------------
	//Write command
	FLASH_SPI_WRITE_BUFFER(0x84);				//Buffer Write
	while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	//Address
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;
	
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;


	//Data
	count = 0;
	while (1)
	{
		FLASH_SPI_WRITE_BUFFER(*buffer++);
		while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
			;
		b_temp = FLASH_SPI_READ_BUFFER;
		
		count++;
		if (count == 0)
			break;
	}
	
	FLASH_CS(1);
	DO_CS_HIGH_DELAY;
	FLASH_CS(0);


	//---------------------------------------------------
	//----- DO BUFFER 1 TO MAIN MEMORY PAGE PROGRAM -----
	//---------------------------------------------------
	//Write command
	if (erase_first)
	{
		FLASH_SPI_WRITE_BUFFER(0x83);			//0x83 = write with built in erase
	}
	else
	{
		FLASH_SPI_WRITE_BUFFER(0x88);			//0x88 = write without built in erase
	}
	while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	//Address
	FLASH_SPI_WRITE_BUFFER((BYTE)((address >> 15) & 0x000000ff));		// 0 0 0 0 PA10 PA9 PA8 PA7
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	FLASH_SPI_WRITE_BUFFER((BYTE)((address >> 7) & 0x000000fe));		//PA6 PA5 PA4 PA3 PA2 PA1 PA0 0 (offset for DataFlash Page Size = 264 Bytes)
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;
	
	FLASH_SPI_WRITE_BUFFER(0x00);										//0 0 0 0 0 0 0 0
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;


	//Start write (low-to-high transition occurs on CS pin)
	FLASH_CS(1);

	//Wait for busy to clear (AT45DB041 typical 14mS, max 35mS)
	while ((flash_read_status() & 0x80) == 0)
	{
		FLASH_CLR_WDT;
	}

}



//*******************************
//*******************************
//********** READ PAGE **********
//*******************************
//*******************************
//Call with:
//	address			Binary byte address (must be the 1st byte of a page)
//	buffer			Array of 256 bytes of data to store read bytes in
void flash_read_page (DWORD address, BYTE *buffer)
{
	BYTE b_temp;
	BYTE count;

	FLASH_CS(0);

	//----- DO MAIN MEMORY PAGE READ -----
	//Write command
	FLASH_SPI_WRITE_BUFFER(0xd2);				//Main Memory Page Read
	while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	//Address
	FLASH_SPI_WRITE_BUFFER((BYTE)((address >> 15) & 0x000000ff));		// 0 0 0 0 PA10 PA9 PA8 PA7
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	FLASH_SPI_WRITE_BUFFER((BYTE)((address >> 7) & 0x000000fe));		//PA6 PA5 PA4 PA3 PA2 PA1 PA0 0 (offset for DataFlash Page Size = 264 Bytes)
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;
	
	FLASH_SPI_WRITE_BUFFER(0x00);										//0 0 0 0 0 0 0 0
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	//32 don't care bits
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;
	
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;
	
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;
	
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)
		;
	b_temp = FLASH_SPI_READ_BUFFER;

	/* This is not required for this device
	//1 bit to turnaround
	FLASH_CLK(1);
	FLASH_SPI_SSPEN(0);			//Disable SPI peripheral
	FLASH_CLK(1);
	DO_CS_HIGH_DELAY;
	FLASH_SPI_SSPEN(1);			//Re-enable SPI peripheral
	*/
	
	//Read page
	count = 0;
	while (1)
	{
		FLASH_SPI_WRITE_BUFFER(0x00);
		while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
			;
		*buffer++ = FLASH_SPI_READ_BUFFER;
		
		count++;
		if (count == 0)
			break;
	}

	FLASH_CS(1);
}




//*********************************
//*********************************
//********** READ STATUS **********
//*********************************
//*********************************
//Returns status byte.
//Bit 7 low = device is busy
BYTE flash_read_status (void)
{
	BYTE b_temp;
	BYTE status_register;

	FLASH_CS(0);

	//Write command
	FLASH_SPI_WRITE_BUFFER(0xD7);
	while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
		;
	b_temp = FLASH_SPI_READ_BUFFER;


	/* This is not required for this device
	//1 bit to turnaround
	FLASH_CLK(1);
	FLASH_SPI_SSPEN(0);			//Disable SPI peripheral
	FLASH_CLK(1);
	DO_CS_HIGH_DELAY;
	FLASH_SPI_SSPEN(1);			//Re-enable SPI peripheral
	*/
	
	//Get Status
	FLASH_SPI_WRITE_BUFFER(0x00);
	while(!FLASH_SPI_BUF_FULL)					//Wait for SPI to complete
		;
	status_register = FLASH_SPI_READ_BUFFER;

	FLASH_CS(1);

	return(status_register);
}






