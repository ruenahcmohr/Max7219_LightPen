#ifndef __max7219
#define __max7219

  #include <avr/io.h>
  #include "avrcommon.h"

  // Bit positions, PORTD 
  #define DATA             3
  #define SCLK             4
  #define CS               5
  
  
  
  #define DATAlow()              ClearBit(DATA, PORTD)
  #define SCLKlow()              ClearBit(SCLK, PORTD)
  #define CSlow()                ClearBit(CS, PORTD)

  
  #define DATAhigh()             SetBit(DATA, PORTD)
  #define SCLKhigh()             SetBit(SCLK, PORTD)
  #define CShigh()               SetBit(CS, PORTD)


  #define pulseSCK()            SCLKlow(); NOP(); NOP(); SCLKhigh(); NOP()
  #define pulseCS()             CShigh(); NOP(); NOP(); CSlow()
  
  #define SendOne()             DATAhigh(); pulseSCK()
  #define SendZero()            DATAlow();  pulseSCK()
  

  void send16 (unsigned int bits);
  unsigned int max7219MakePacket(unsigned char cmd, unsigned char value) ;
  void max7219Init( ) ;
  void max7219Blit( uint8_t *i, uint8_t *j) ;
  void max7219SetDisplay( unsigned char digit, unsigned char v) ;
  void max7219SetIntensity( unsigned char v)   ;
  void max7219SetScanLimit( unsigned char v)   ;
  void max7219SetDecodeFlags( unsigned char v) ;
  void sendToAll( unsigned char cmd, unsigned char value) ;

  typedef enum cmd_e { cmdNOP = 0, cmdDIG0, cmdDIG1, cmdDIG2, cmdDIG3, cmdDIG4,cmdDIG5, cmdDIG6, cmdDIG7, cmdMODE, cmdINT, cmdSCAN, cmdPOWER, cmdTEST } M7219cmd_t;
  
#endif
