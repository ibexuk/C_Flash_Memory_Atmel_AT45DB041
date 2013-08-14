
######################################################################
##### Open sourced by IBEX, an electronic product design company #####
#####    based in South East England.  http://www.ibexuk.com     #####
######################################################################

The Atmel AT45DB041D is a 4-megabit SPI serial bus flash memory device.


The Atmel AT45DB041 flash memory IC is a SPI bus device. It provides very low cost bulk memory to embedded designs.

Memory organisation:
0×00000 – 0x7FFFF
524288 bytes x 8 (512KB)
Organised as 2,048 pages of 256 bytes (To avoid non binary complexities this driver only uses 256 bytes of each page instead of the 264 which are actually available)


This driver provides the following functions:


TEST FLASH MEMORY

BYTE flash_test_memory (void)

An example of how to use this drivers functions.

Also a useful function if you want to verify the flash memory driver is working properly.

Returns 1 if test passed, 0 if not.


IS FLASH IC PRESENT

BYTE flash_check_present (void)

Useful to check flash chip is there and working


ERASE FLASH MEMORY

void flash_erase_all (void)


WRITE PAGE

void flash_write_page (DWORD address, BYTE *buffer, BYTE erase_first)

Call with:

address Binary byte address (must be the 1st byte of a page)

buffer Array of 256 bytes of data to be written

erase_first 1 = erase buffer first, 0 = don’t erase (if you’ve previously called flash_erase_all)


READ PAGE

void flash_read_page (DWORD address, BYTE *buffer)

Call with:

address Binary byte address (must be the 1st byte of a page)

buffer Array of 256 bytes of data to store read bytes in


READ STATUS

BYTE flash_read_status (void)

Returns status byte (bit 7 low = device is busy)


Please view the sample source code files for full information regarding the driver.