#ifndef __max7219
#define __max7219

  #include <avr/io.h>
  #include "avrcommon.h"

  // Bit positions, PORTD 
  // #define DATA             3
  // #define SCLK             4
  // #define CS               5
  
  // Re-arranged to align with arduino SPI libraries ( and hardware )
  
  /*
  signal   arduino   avr
  DATA        11     PB3      
  SCLK        13     PB5
  CS          10     PB2  
  */
  
  // Bit positions, PORTB
  #define DATA             3
  #define SCLK             5
  #define CS               2
  
  
  #define DATAlow()              ClearBit(DATA, PORTB)
  #define SCLKlow()              ClearBit(SCLK, PORTB)
  #define CSlow()                ClearBit(CS, PORTB)

  
  #define DATAhigh()             SetBit(DATA, PORTB)
  #define SCLKhigh()             SetBit(SCLK, PORTB)
  #define CShigh()               SetBit(CS, PORTB)


  #define pulseSCK()            SCLKlow(); NOP(); NOP(); SCLKhigh(); NOP()
  #define pulseCS()             CShigh(); NOP(); NOP(); CSlow()
  
  #define SendOne()             DATAhigh(); pulseSCK()
  #define SendZero()            DATAlow();  pulseSCK()
  

  void send16 (unsigned int bits);
  unsigned int max7219MakePacket(unsigned char cmd, unsigned char value) ;
  void max7219Init( ) ;
  void max7219Blit( uint8_t *i) ;
  void max7219SetDisplay( unsigned char digit, unsigned char v) ;
  void max7219SetIntensity( unsigned char v)   ;
  void max7219SetScanLimit( unsigned char v)   ;
  void max7219SetDecodeFlags( unsigned char v) ;

  typedef enum cmd_e { cmdNOP = 0, cmdDIG0, cmdDIG1, cmdDIG2, cmdDIG3, cmdDIG4,cmdDIG5, cmdDIG6, cmdDIG7, cmdMODE, cmdINT, cmdSCAN, cmdPOWER, cmdTEST } M7219cmd_t;
  
#endif
