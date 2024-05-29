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


void setupInt(void);

typedef struct {
  uint8_t nextState[2]; // Next state based on Flag (0 or 1)
  uint8_t detValue[2]; // Value to set det based on Flag (0 or 1)
} State;

State stateTable[] = {
  // nextState[0], nextState[1], detValue[0], detValue[1]
  [0x0F] = {{0xF0, 0x03}, {0xFF, 0xFF}}, 
  [0xF0] = {{0xFF, 0x30}, {0xFF, 0xFF}}, 
  [0x03] = {{0x0C, 0x01}, {0xFF, 0xFF}}, 
  [0x01] = {{0x02, 0xFE}, {0xFF, 0x00}}, 
  [0x02] = {{0xFF, 0xFE}, {0xFF, 0x01}}, 
  [0x0C] = {{0xFF, 0x04}, {0xFF, 0xFF}}, 
  [0x04] = {{0x08, 0xFE}, {0xFF, 0x02}}, 
  [0x08] = {{0xFF, 0xFE}, {0xFF, 0x03}}, 
  [0x30] = {{0xC0, 0x10}, {0xFF, 0xFF}}, 
  [0x10] = {{0x20, 0xFE}, {0xFF, 0x04}}, 
  [0x20] = {{0xFF, 0xFE}, {0xFF, 0x05}}, 
  [0xC0] = {{0xFF, 0x40}, {0xFF, 0xFF}}, 
  [0x40] = {{0x80, 0xFE}, {0xFF, 0x06}}, 
  [0x80] = {{0xFF, 0xFE}, {0xFF, 0x07}}, 
  [0x00] = {{0x0F, 0x0F}, {0xFF, 0xFF}} 
};

int main( void ) {
 
 uint8_t image[8];

 uint8_t line, row;
 uint8_t i;
 
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
  
  send16(max7219MakePacket(cmdINT,    0x03)); // 1/2 brightness (7/15)     
  detX = detY = -1;
  
  
  SetBit(6, PORTD);
 
uint8_t step = 0x00; 
while (step < 0xFE) {    
  uint8_t flagIndex = Flag ? 1 : 0;
  State currentState = stateTable[step];

  // step step step
  step = currentState.nextState[flagIndex];

  // update x detection if needed
  if (currentState.detValue[flagIndex] != 0xFF) {
    detX = currentState.detValue[flagIndex];
  }

  if (step < 0xFE) {
    for (int i = 0; i < 8; i++) {
      image[i] = step;
    } 
    max7219Blit(image);   
    Delay(2000); // 'finish scanning and accept new data' delay
    Flag = 0;
    Delay(3000); // Change this delay down to about 3000 min. for high-speed scanning.
  }
}

    
if (detX != -1) {
    uint8_t step = 0x00; 
    while (step < 0xFE) {    
        uint8_t flagIndex = Flag ? 1 : 0;
        State currentStateY = stateTable[step];

        // step step step
        step = currentStateY.nextState[flagIndex];

        // Update y detection if needed
        if (currentStateY.detValue[flagIndex] != 0xFF) {
            detY = currentStateY.detValue[flagIndex];
        }

        if (step < 0xFE) {
            for (int i = 0; i < 8; i++) {
                image[i] = (step & (1 << i)) ? 0xFF : 0x00;
            }
            max7219Blit(image);   
            Delay(2000); // 'finish scanning and accept new data' delay
            Flag = 0;
            Delay(3000); // Change this delay down to about 3000 min. for high-speed scanning.
        }
    }
    
    if (detY != -1) { // Accept the position if we got an X and Y
        penX = detX;
        penY = detY;
    }
}

   
   ClearBit(6, PORTD);
   send16(max7219MakePacket(cmdINT,    0x0C)); // 1/2 brightness (C/15)     
     for( i = 0; i < 8; i++) { // clear image
       image[i] = 0x00;
     }
     
     image[penY] = (1<<penX);
     
     max7219Blit( image );   
     
     Delay(80000);   //<--- this is the amount of time to show the detected location before searching again.
   
  }

}



void setupInt() {        
  EICRA = (1<<ISC10) | (1<<ISC00);  // we need to set up int0 and int1 to trigger interrupts on both edges         
  EIMSK = (1<<INT0) | (0<<INT1); // then enable them.
}
 

ISR(INT0_vect){ // the scanning produces brief pulses, we just need to know if it saw one since we last cleared the flag.
   Flag = 1;
}
















