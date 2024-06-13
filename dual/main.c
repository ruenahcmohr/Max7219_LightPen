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

    uint8_t left[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t right[8] = {0, 0, 0, 0, 0, 0, 0, 0};
#define DELAY_TIME 100000
int i;
int space = -1;
volatile uint8_t Flag;
uint8_t smiley_face_left[8] = {
    0b00111100, // Row 1: Upper part of the face
    0b01000010, // Row 2: Start of the eyes
    0b10100101, // Row 3: Eyes
    0b10000001, // Row 4: Part of the face
    0b10011001, // Row 5: Start of the smile
    0b01011010, // Row 6: Smile continues
    0b00111100, // Row 7: Bottom part of the face
    0b00000000  // Row 8: Empty
};

// Data for the right matrix (right side of the smiley face)
uint8_t smiley_face_right[8] = {
    0b00111100, // Row 1: Upper part of the face
    0b01000010, // Row 2: Continuation of the eyes
    0b10100101, // Row 3: Eyes
    0b10000001, // Row 4: Part of the face
    0b10011001, // Row 5: End of the smile
    0b01011010, // Row 6: Smile ends
    0b00111100, // Row 7: Bottom part of the face
    0b00000000  // Row 8: Empty
};

void setupInt(void);
void clearDisplays(void);
int main(void)
{
  // Set up directions
  DDRB = (OUTPUT << PB0 | OUTPUT << PB1 | INPUT << PB2 | INPUT << PB3 | INPUT << PB4 | OUTPUT << PB5);
  DDRD = (OUTPUT << PD3 | OUTPUT << PD4 | OUTPUT << PD5 | OUTPUT << PD6);
  DDRC = 0; // All pins as input, change if necessary

  max7219Init();
  max7219SetIntensity(0x03);
  setupInt();
  sei();
  Delay(100000);
  clearDisplays();
  while (1)
  {
    space += 1;
    if(space == 8){
      space = 0;
    }
    
    clearDisplays();
    max7219Blit(left, right);
    Delay(10000);
    left[space] = 0x01;
    for (int i = 0; i < 8; ++i)
    {
      for(int j = 0;j < 8;++j){
      left[j] = left[j] << 1;
      max7219Blit(left, right);
      }
      if (i == 7)
      {
        Delay(20000);
      }
      else
      {
        Delay(100000);
      }
    }
    right[space] = 0x01;
    max7219Blit(left, right);
    Delay(100000);
    for (int j = 0; j < 8; ++j)
    {
       for(int j = 0;j < 8;++j){
      right[j] = right[j] << 1;
      max7219Blit(left, right);
      }
      if (i == 7)
      {
        Delay(20000);
      }
      else
      {
        Delay(100000);
      }
      }
    }
  }


void clearDisplays()
{
  uint8_t clear_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  max7219Blit(clear_data, clear_data);
}

void setupInt()
{
  EICRA = (1 << ISC10) | (1 << ISC00); // Trigger INT0 on logic change
  EIMSK = (1 << INT0);                 // Enable INT0, INT1 is not enabled
}

ISR(INT0_vect)
{
  Flag = 1; // Flag is set when INT0 is triggered
}