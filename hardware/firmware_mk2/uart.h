
#ifndef UART_BAUD_RATE
#warning "UART_BAUD_RATE not set, setting it to 115200"
#define UART_BAUD_RATE 115200
#endif

#define BAUD_SPBRGH  (((_XTAL_FREQ/UART_BAUD_RATE)/64 - 1) >> 8)
#define BAUD_SPBRGL  (((_XTAL_FREQ/UART_BAUD_RATE)/64 - 1) >> 8)


// Watchdog sleep times
#define WATCHDOG_SLEEP_4S 0b01100

// Note: The watchdog operating mode configuration is done in configuration word 1 
void watchdog_configure(char interval) 
{
    // Configure the watchdog interval (e.g. 10010 for 256 seconds)
    WDTCONbits.WDTPS = interval & 0b11111;
}

void watchdog_sleep(void) 
{
    WDTCONbits.SWDTEN = 0b1;
    SLEEP();
    WDTCONbits.SWDTEN = 0b0;
}

// SPBRGH:SPBRGL = ((Fosc/baudrate)/ 8) - 1 , when BRGH=0 & BRG16=1
// SPBRGH:SPBRGL = ((Fosc/baudrate)/64) - 1 , when BRGH=1 & BRG16=1
//   e.g. 16MHz clk & 9600 bps : ((16e6/9600)/64 - 1) = 25
//   e.g. 48MHz clk & 9600 bps : ((48e6/9600)/64 - 1) = 77
//   e.g.                        ((20e6/115200)/4 -1) =   42.4        x
//   e.g.                        ((48e6/115200)/4 -1) =  103.16       x
//   e.g.                        ((48e6/ 57600)/4 -1) =  207.3        x
//   e.g.                        ((48e6/  9600)/4 -1) = 1249          x
// 
//void UART_setBaudRate(const unsigned char type)
void UART_setBaudRate()
{
    SPBRGH =  ((_XTAL_FREQ/UART_BAUD_RATE)/4 - 1) >> 8;
    SPBRGL =  ((_XTAL_FREQ/UART_BAUD_RATE)/4 - 1) & 0xff;
    /*
    // 9600
    SPBRGH = 1249 >> 8;
    SPBRGL = 1249 & 0xff;
    // 57600
    SPBRGH = 207 >> 8;
    SPBRGL = 207 & 0xff;
    // 115200
    SPBRGH = 103 >> 8;
    SPBRGL = 103 & 0xff;
    */
}

// Configure the UART for full-duplex asynchronous transmission 
void UART_initialize(void) 
{
    //UART_setBaudRate(BAUD_115200);
    UART_setBaudRate();

    // Configure the RX/DT pin as an input
    //TRISCbits.TRISC4 = 1;

    // Enable the asynchronous transceiver
    TXSTAbits.TXEN = 1;  // tx: enable transmit
    TXSTAbits.SYNC = 0;  // tx/rx: async mode
    TXSTAbits.BRGH = 1;  // tx/rx: high speed baud rate

    RCSTAbits.SPEN = 1;  // rx: serial port enable
    RCSTAbits.CREN = 1;  // rx: continuous receive 

    BAUDCONbits.BRG16 = 1;     // Use 16 bits for the baud rate
}

void UART_write(const unsigned char character) 
{
    // Wait while the transmit shift register empties
    while(!PIR1bits.TXIF) {}

    // Indicate the character to be written to the Transmit Shift Register
    TXREG = character;

    // Add a one cycle delay to ensure that the TXIF flag is valid
    _delay(1);
}

void UART_writeString(const unsigned char *string) 
{
    unsigned char i = 0;

    while(*(string + i) != '\0') {
        UART_write(*(string + i));
        i += 1;
    }
}

/*
void main(void) {
    // Perform initialisations
    //watchdog_configure(WATCHDOG_SLEEP_4S);
    UART_initialise();
    watchdog_sleep();
    UART_writeString("UART TESTING");
}
*/
