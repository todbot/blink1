/* 
 * File:   flashprog.h
 * Author: tod
 *
 * Created on February 12, 2013, 11:21 PM
 *
 * Some notes:
 * - write length is 32 words
 * - words are 14-bits in length, xc8 only uses bottom 8-bits
 * - when erased, a word in flash reads as 0x3fff
 * - xc8 doesn't do anything with upper 6-bits of 14-bit word
 * - so when byte of say 0xab is stored by xc8, word is 0x3fab
 * - PIC16F1455 chip errata requires clk set to 4MHz, not 8MHz or 16MHz
 * - preferred use of these funcs are:
 *     clock_slow();  // NOTE: must do this because of chip errata
 *    //flash_erase( 0x1f80 );
 *    //flash_write( 0x1f80, ram_data );
 *    flash_erase( flash_data );
 *    flash_write( flash_data, ram_data );
 *     clock_fast();
 *
 */

#ifndef FLASHPROG_H
#define	FLASHPROG_H

// used by flash_erase() and flash_write()
#define flash_unlock() { \
    PMCON2 = 0x55; \
    PMCON2 = 0xAA; \
    WR = 1;        \
    asm("nop");    \
    asm("nop");    \
}


//
// Erase 32 words of program memory starting at secified address.
//
// address  = 16bits address (0->1FFF for pic 8K).
//
//void flash_erase(unsigned short address)
void flash_erase(void* address)
{
    PMADRL=(((uint16_t)address)&0xff);    // load address
    PMADRH=(((uint16_t)address)>>8);      // load address

    PMCON1 = 0;      // access FLASH program, not config
    FREE = 1;        // perform an erase on next WR command, cleared by hardware
    WREN = 1;        // allow program/erase
    flash_unlock();
    WREN = 0;        // disallow program/erase
}


//
// write 32 bytes of program memory starting at 'address'
// using data
//
//void flash_write(unsigned short address, uint8_t *data )
void flash_modify(uint8_t* address, uint8_t* data )
{
    unsigned char wdi;

    // erase operation

    CFGS = 0;      // access FLASH program, not config

    PMADRL=(((uint16_t)address)&0xff);        // load address
    PMADRH=(((uint16_t)address)>>8);          // load address

    FREE = 1;        // perform an erase on next WR command, cleared by hardware
    WREN = 1;        // allow program/erase
    flash_unlock();
    // cpu stalls here for 2ms typical
    WREN = 0;        // disallow program/erase

    // write operation

    CFGS = 0;                 // access FLASH program, not config

    PMADRL=(((uint16_t)address)&0xff);        // load address
    PMADRH=(((uint16_t)address)>>8);          // load address

    FREE = 0;                   // perform write
    LWLO = 1;                   // only load latches
    WREN = 1;                   // allow program/erase

    for( wdi=0; wdi<31; wdi++ ) {
        PMDATH = 0x00;          // we're going to ignore upper 6 bits
        PMDATL = data[wdi];     // and put byte in lower 8-bits, like xc8 does

        flash_unlock();

        PMADR++;
    }
    PMDATH = 0x00;
    PMDATL = data[31];

    LWLO = 0;                  // write lateches to flash
    //WREN = 1;                  // allow program/erase

    flash_unlock();
    // cpu stalls here for 2ms typical

    WREN = 0;                       // disallow program/erase
}

//
// write 32 bytes of program memory starting at 'address'
// using data
//
//void flash_write(unsigned short address, uint8_t *data )
void flash_write(void* address, uint8_t* data )
{
    unsigned char wdi;

    PMADRL=(((uint16_t)address)&0xff);        // load address
    PMADRH=(((uint16_t)address)>>8);          // load address

    PMCON1 = 0;                 // access FLASH program, not config
    LWLO = 1;                   // only load latches
    WREN = 1;                   // allow program/erase

    for( wdi=0; wdi<31; wdi++ ) {
        PMDATH = 0x00;          // we're going to ignore upper 6 bits
        PMDATL = data[wdi];     // and put byte in lower 8-bits, like xc8 does

        flash_unlock();

        PMADR++;
    }
    PMDATH = 0x00;
    PMDATL = data[31];

    PMCON1 = 0;                // access FLASH program, not config
    LWLO = 0;                  // this time start write
    WREN = 1;                  // allow program/erase

    flash_unlock();

    WREN = 0;                       // disallow program/erase
}

// untested, don't use, just access "const" variable directly
void flash_read_block(unsigned short address, unsigned char **data)
{
    // TBD
}

// untested, don't use, just access "const" variable directly
unsigned int flash_read_word(unsigned int address)
{
    PMADRL=((address)&0xff);
    PMADRH=((address)>>8);
    CFGS = 0;					// access FLASH program, not config
    LWLO = 0;					// only load latches

    RD = 1;
    asm("nop");
    asm("nop"); //NOP();

    return ( (PMDATH)<<8 | (PMDATL) ) ;
}

// read config word from datasheet EXAMPLE 11-4
// also works to read device id and revision id (0x8005-0x8006)
uint16_t config_read(unsigned int addr)
{
    PMADRL = ((addr) & 0xff);
    PMADRH = 0; // ((0x8007) >>8);
    CFGS = 1;  // set configuration space
    GIE = 0;   // disable interrupts
    RD = 1;
    asm("nop");
    asm("nop"); //NOP();
    GIE = 1;   // restore interrupts
    return ( (PMDATL) | (PMDATH)<<8 ) ;
}

#endif	/* FLASHPROG_H */

