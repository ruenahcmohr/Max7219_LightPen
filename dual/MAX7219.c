#include "MAX7219.h"

// Send a 16-bit word, MSB first
void send16(unsigned int data)
{
   unsigned int temp;
   for( temp = (0x0001<<15); temp != 0; temp >>= 1) {    
    if ( (data & temp) != 0 ) {    SendOne();
    } else {                       SendZero();
    }    
   NOP();
   NOP();
  } 
}

unsigned int max7219MakePacket(unsigned char cmd, unsigned char value)
{
   return ((cmd << 8) | value);
}

void max7219Init()
{
   // Initialize both displays
   sendToAll(cmdPOWER, 0x01); // Wakeup
   sendToAll(cmdTEST, 0x00);  // No test mode
   sendToAll(cmdINT, 0x07);   // 1/2 brightness (8/15)
   sendToAll(cmdSCAN, 0xFF);  // All digits
   sendToAll(cmdMODE, 0x00);  // No decode
}

void sendToAll(unsigned char cmd, unsigned char value)
{
   CSlow();
   send16(max7219MakePacket(cmd, value));
   send16(max7219MakePacket(cmd, value));
   CShigh();
}

void max7219Blit(uint8_t *left, uint8_t *right)
{
   for (int digit = 0; digit < 8; digit++)
   {
      CSlow();
      send16(max7219MakePacket(cmdDIG0 + digit, right[digit])); 
      send16(max7219MakePacket(cmdDIG0 + digit, left[digit]));
      CShigh();
   }
}

void max7219SetIntensity(unsigned char v)
{
   sendToAll(cmdINT, v);
}

void max7219SetScanLimit(unsigned char v)
{
   sendToAll(cmdSCAN, v);
}

void max7219SetDecodeFlags(unsigned char v)
{
   sendToAll(cmdMODE, v);
}
