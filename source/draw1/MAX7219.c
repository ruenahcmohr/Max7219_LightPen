#include "MAX7219.h"


// send a 16bit word, msb first
void send16 (unsigned int bits) {
  unsigned int temp;  
  CSlow();
  for( temp = (0x0001<<15); temp != 0; temp >>= 1) {    
    if ( (bits & temp) != 0 ) {    SendOne();
    } else {                       SendZero();
    }    
   NOP();
   NOP();
  } 
  CShigh();
}



unsigned int max7219MakePacket(unsigned char cmd, unsigned char value) {
  return ( (cmd<<8) | (value));
}


void max7219Init( ) { 
   send16(max7219MakePacket(cmdPOWER,  0x01));   // wakeup
   send16(max7219MakePacket(cmdTEST,   0x00));   // no test mode   
   send16(max7219MakePacket(cmdINT,    0x07)); // 1/2 brightness (8/15)     
   send16(max7219MakePacket(cmdSCAN,   0xFF)); // all digits
   send16(max7219MakePacket(cmdMODE,   0x00)); // no decode
  // send16(max7219MakePacket(cmdMODE,   0xFF));  // all decode
  
   send16(max7219MakePacket(cmdDIG0+0, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+1, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+2, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+3, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+4, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+5, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+6, 0x00)); // dots!
   send16(max7219MakePacket(cmdDIG0+7, 0x00)); // dots! 
    
}

void max7219SetDisplay( unsigned char digit, unsigned char v)   { 
   digit &= 0x07;
   send16(max7219MakePacket(cmdDIG0+digit, v));       
}

void max7219Blit( uint8_t *i) {
   send16(max7219MakePacket(cmdDIG0+0, *(i+0))); // dots!
   send16(max7219MakePacket(cmdDIG0+1, *(i+1))); // dots!
   send16(max7219MakePacket(cmdDIG0+2, *(i+2))); // dots!
   send16(max7219MakePacket(cmdDIG0+3, *(i+3))); // dots!
   send16(max7219MakePacket(cmdDIG0+4, *(i+4))); // dots!
   send16(max7219MakePacket(cmdDIG0+5, *(i+5))); // dots!
   send16(max7219MakePacket(cmdDIG0+6, *(i+6))); // dots!
   send16(max7219MakePacket(cmdDIG0+7, *(i+7))); // dots! 
}

void max7219SetIntensity( unsigned char v)   { send16(max7219MakePacket(cmdINT, v)); }
void max7219SetScanLimit( unsigned char v)   { send16(max7219MakePacket(cmdSCAN, v)); }
void max7219SetDecodeFlags( unsigned char v) { send16(max7219MakePacket(cmdMODE, v)); }


