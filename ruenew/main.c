/*

avr @ 16Mhz 

on pro mini:

0   rxd    rxd
1   txd    txd

2   pd2    (int0)   light pen  
3   pd3    
4   pd4   
5   pd5    
6   pd6    
7   pd7    

8   pb0   Set pixel button 
9   pb1   Clear pixel button
10  pb2   CS 
11  pb3   DATA
12  pb4   
13  pb5   SCK 


A0  pc0    
A1  pc1    
A2  pc2    
A3  pc3    
A4  pc4    
A5  pc5    


A6  adc6  
A7  adc7  


*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "MAX7219.h"
#include "avrcommon.h"
#include "nopDelay.h"

/*
try success fail
----------------
0F     03    F0
F0     30    xx

03     01    0C
01     yy    02
02     yy    xx

0C     04    xx
04     yy    08
08     yy    xx


30     10    C0
10     yy    20
20     yy    xx

C0     40    xx
40     yy    80
80     yy    xx

This was all silly, I should have just used alternating bits that are AND'd togethor.
I'll try again later.

*/


volatile uint8_t Flag;
int8_t penX, penY;



void setupInt(void);
void GetLoc(void) ;




int main( void ) {
 
 uint8_t image[8]; 
 uint8_t i;


    // set up directions 
    
  DDRB = (INPUT << PB0 | INPUT << PB1 |OUTPUT << PB2 |OUTPUT << PB3 | INPUT << PB4 |OUTPUT << PB5 | INPUT << PB6 | INPUT << PB7);
  DDRD = (INPUT << PD0 | INPUT << PD1 | INPUT << PD2 | INPUT << PD3 | INPUT << PD4 | INPUT << PD5 | INPUT << PD6 |INPUT << PD7);        
  DDRC = (INPUT << PC0 | INPUT << PC1 | INPUT << PC2 | INPUT << PC3 | INPUT << PC4 | INPUT << PC5 | INPUT << PC6 ); 

  max7219Init();
  setupInt();
  sei(); 

  penX = penY = 0;

  for( i = 0; i < 8; i++) { // clear image
    image[i] = 0x00;
  }
  
  send16(max7219MakePacket(cmdINT,    0x02)); // 1/2 brightness (C/15)         
  max7219Blit( image );   
   
  while(1) {
    while(IsHigh(0, PINB) & IsHigh(1, PINB)); // wait for button
 
    GetLoc();

    send16(max7219MakePacket(cmdINT,    0x02)); // 1/2 brightness (C/15)   
    
    if ((penX != -1) && (penY != -1)) {
      if (IsLow(0, PINB)) image[penY] |= (1<<penX);    // draw button
      if (IsLow(1, PINB)) image[penY] &= (~(1<<penX)); // erase button
    }
    max7219Blit( image );   

    while(IsLow(0, PINB) | IsLow(1, PINB)); // wait for user to lift finger off button

    Delay(8000);   //<--- this is the amount of time to show the detected location before searching again.
   
  }

}







void GetLoc() {
  uint8_t image[8];
  uint8_t giveup;
  int8_t detX, detY;
  uint8_t step;
  uint8_t i;
 
  send16(max7219MakePacket(cmdINT,  0x0F)); // 1/2 brightness (7/15)     
  
  detX = detY = -1;
  giveup = 3;  
  SetBit(6, PORTD);
  
  while((detY == -1)& giveup) {
    for(giveup = 6; (detX == -1) & (giveup != 0); giveup--) {
      step = 0x00; 
      while(step < 0xFE) {    
	switch(step) { // sorry, Rue is state machine.      
	  case 0x0F: step = Flag?0x03:0xF0; break;      
	  case 0xF0: step = Flag?0x30:0xFF; break;
	  case 0x03: step = Flag?0x01:0x0C; break;
	  case 0x01: step = Flag?0xFE:0x02; detX = Flag?0:detX ; break;
	  case 0x02: step = Flag?0xFE:0xFF; detX = Flag?1:detX; break;
	  case 0x0C: step = Flag?0x04:0xFF; break;
	  case 0x04: step = Flag?0xFE:0x08; detX = Flag?2:detX; break;
	  case 0x08: step = Flag?0xFE:0xFF; detX = Flag?3:detX; break;
	  case 0x30: step = Flag?0x10:0xC0; break;
	  case 0x10: step = Flag?0xFE:0x20; detX = Flag?4:detX; break;
	  case 0x20: step = Flag?0xFE:0xFF; detX = Flag?5:detX; break;
	  case 0xC0: step = Flag?0x40:0xFF; break;
	  case 0x40: step = Flag?0xFE:0x80; detX = Flag?6:detX; break;
	  case 0x80: step = Flag?0xFE:0xFF; detX = Flag?7:detX; break;
	  default:   step = 0x0F;           break;        
	}
	if (step < 0xFE) {
	 for( i = 0; i<8; i++) {
	   image[i] = step;
	 } 
	 max7219Blit( image );   
	 Delay(2000); // this is a 'finish scanning and accept new data' delay
	 Flag = 0;
	 Delay(3600); // <---------- change this delay down to about 3000 min. for high speed scanning.
	}
      }
    } 
     if (detX != -1) {

       step = 0x00; 
       while(step < 0xFE) {    
	 switch(step) { // sorry, Rue is state machine.      
	   case 0x0F: step = Flag?0x03:0xF0; break;      
	   case 0xF0: step = Flag?0x30:0xFF; break;
	   case 0x03: step = Flag?0x01:0x0C; break;
	   case 0x01: step = Flag?0xFE:0x02; detY = Flag?0:detY ; break;
	   case 0x02: step = Flag?0xFE:0xFF; detY = Flag?1:detY; break;
	   case 0x0C: step = Flag?0x04:0xFF; break;
	   case 0x04: step = Flag?0xFE:0x08; detY = Flag?2:detY; break;
	   case 0x08: step = Flag?0xFE:0xFF; detY = Flag?3:detY; break;
	   case 0x30: step = Flag?0x10:0xC0; break;
	   case 0x10: step = Flag?0xFE:0x20; detY = Flag?4:detY; break;
	   case 0x20: step = Flag?0xFE:0xFF; detY = Flag?5:detY; break;
	   case 0xC0: step = Flag?0x40:0xFF; break;
	   case 0x40: step = Flag?0xFE:0x80; detY = Flag?6:detY; break;
	   case 0x80: step = Flag?0xFE:0xFF; detY = Flag?7:detY; break;
	   default:   step = 0x0F;           break;        
	 }
	 if (step < 0xFE) {
	  for( i = 0; i < 8; i++ ) {
	    image[i] = (step&(1<<i))?0xFF:0x00;
	  } 
	  max7219Blit( image );   
	  Delay(2000);
	  Flag = 0;
	  Delay(3600);// <---------- change this delay down to about 3000 min. for high speed scanning.
	 }

      }
    }
    

     
  }
  penX = detX;
  penY = detY;
    
  ClearBit(6, PORTD);
     
     

}


void setupInt() {        
  EICRA = (1<<ISC10) | (1<<ISC00);  // we need to set up int0 and int1 to trigger interrupts on both edges         
  EIMSK = (1<<INT0) | (0<<INT1); // then enable them.
}
 

ISR(INT0_vect){ // the scanning produces brief pulses, we just need to know if it saw one since we last cleared the flag.
   Flag = 1;
}















