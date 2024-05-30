/*

avr @ 16Mhz


on pro mini:

0   rxd    rxd
1   txd    txd

2   pd2    (int0)   light pen  
3   pd3    data
4   pd4    sck
5   pd5    cs
6   pd6    
7   pd7    

8   pb0    
9   pb1    
10  pb2    
11  pb4    
12  pb3
13  pb5    


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


volatile uint8_t Flag;


void setupInt(void);


int main( void ) {
 
 uint8_t image[8];

 uint8_t line, row;
 uint8_t i, x, y, c;
 
 uint8_t penX, penY;
 int8_t detX, detY;
 uint8_t step;
 

    // set up directions 
  DDRB = (OUTPUT << PB0 | OUTPUT << PB1 | INPUT << PB2 | INPUT << PB3 | INPUT << PB4 |OUTPUT << PB5 | INPUT << PB6 | INPUT << PB7);
  DDRD = (INPUT << PD0 | INPUT << PD1 | INPUT << PD2 |OUTPUT << PD3 |OUTPUT << PD4 |OUTPUT << PD5 |OUTPUT << PD6 |INPUT << PD7);        
  DDRC = (INPUT << PC0 | INPUT << PC1 | INPUT << PC2 |INPUT << PC3 |INPUT << PC4 |INPUT << PC5 |INPUT << PC6 ); 
  
  max7219Init();
  setupInt();
  sei(); 

  penX = penY = 0;

   
  while(1) {
  
  send16(max7219MakePacket(cmdINT,    0x07)); // 1/2 brightness (7/15)     
  detX = detY = -1;
  
  
  
  
  SetBit(6, PORTD);// started scan
  
  for( i = 0; i < 8; i++) image[i] = 0xFF; // set all leds, to search for ANY pixel.
  max7219Blit( image );   
  Delay(800); // this is a 'finish scanning and accept new data' delay
  Flag = 0;
  Delay(4000);
  
  if (Flag) { // I saws a pixel!
  
    detX = 0;
    for(step = 32; step != 0; step >>= 1 ) {
      c = step;
      i = 1;
      for( y = 0; y < 8; y++ ) {
	image[y] = 0x00;
	for(x = 0; x < 8; x++ ) {
          c--;
          image[y] |= (i<<x);
	  i = (c==0)?0x01-i:i;
	  c = (c==0)?step:c;
	} 
      } 
      max7219Blit( image );   
      Delay(800); // this is a 'finish scanning and accept new data' delay
      Flag = 0;
      Delay(4000);
      detX = Flag?(detX|step):detX;
    }    
  
    // Then process the resulting pixel number a bit.
    penY = 7-(detX >> 3);
    penX = 7-(detX & 0x07);
  
  }   
  ClearBit(6, PORTD); // scan done.
   
   
   
   
   send16(max7219MakePacket(cmdINT,    0x0C)); // 1/2 brightness (C/15)     
     for( i = 0; i < 8; i++) { // clear image
       image[i] = 0x00;
     }
     
     image[penY] = (1<<penX);
     
     max7219Blit( image );   
     
     Delay(16000);   //<--- this is the amount of time to show the detected location before searching again.
   
  }

}



void setupInt() {        
  EICRA = (1<<ISC10) | (1<<ISC00);  // we need to set up int0 and int1 to trigger interrupts on both edges         
  EIMSK = (1<<INT0) | (0<<INT1); // then enable them.
}
 

ISR(INT0_vect){ // the scanning produces brief pulses, we just need to know if it saw one since we last cleared the flag.
   Flag = 1;
}
















