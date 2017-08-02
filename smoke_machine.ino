/*******************************************************************************

 Bare Conductive Touch MP3 player
 ------------------------------
 
 Touch_MP3.ino - touch triggered MP3 playback

 You need twelve MP3 files named TRACK000.mp3 to TRACK011.mp3 in the root of the 
 microSD card. 
 
 When you touch electrode E0, TRACK000.mp3 will play. When you touch electrode 
 E1, TRACK001.mp3 will play, and so on.

  SD card    
  │
    TRACK000.mp3  
    TRACK001.mp3  
    TRACK002.mp3  
    TRACK003.mp3  
    TRACK004.mp3  
    TRACK005.mp3  
    TRACK006.mp3  
    TRACK007.mp3  
    TRACK008.mp3  
    TRACK009.mp3  
    TRACK010.mp3  
    TRACK011.mp3  
 
 Based on code by Jim Lindblom and plenty of inspiration from the Freescale 
 Semiconductor datasheets and application notes.
 
 Bare Conductive code written by Stefan Dzisiewski-Smith and Peter Krige.
 
 This work is licensed under a MIT license https://opensource.org/licenses/MIT
 
 Copyright (c) 2016, Bare Conductive
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 Modification by:
 Quentin Aellen
 Dominik Bärlocher
 digitec.ch 

*******************************************************************************/

// compiler error handling
#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <Wire.h>
#define MPR121_ADDR 0x5C
#define MPR121_INT 4

// mapping and filter definitions
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.3f // 0.0f to 1.0f - higher value = more smoothing
float lastProx = 0;

// the electrode to monitor
#define ELECTRODE 0

// mp3 includes
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>

// mp3 variables
SFEMP3Shield MP3player;
byte result;
int lastPlayed = 0;

// Output Signal to the relay
#define CH10 10 //Digital Pin 10, connection to Relay

// sd card instantiation
SdFat sd;

void setup(){  
  Serial.begin(57600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(LED_BUILTIN, 0);
  pinMode(CH10, OUTPUT);
   
  //while (!Serial) ; {} //uncomment when using the serial monitor 
  Serial.println("Bare Conductive Touch MP3 player");

  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  if(!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  MPR121.setInterruptPin(MPR121_INT);

  MPR121.setTouchThreshold(40);
  MPR121.setReleaseThreshold(20);
 
  
    // slow down some of the MPR121 baseline filtering to avoid 
  // filtering out slow hand movements
  MPR121.setRegister(MPR121_NHDF, 0x01); //noise half delta (falling)
  MPR121.setRegister(MPR121_FDLF, 0x3F); //filter delay limit (falling)
}

void loop(){
  // readTouchInputs();
  // update all of the data from the MPR121
  MPR121.updateAll();

  // read the difference between the measured baseline and the measured continuous data
  int reading = MPR121.getBaselineData(ELECTRODE)-MPR121.getFilteredData(ELECTRODE);
 

  // print out the reading value for debug
  Serial.println(reading); 

  // constrain the reading between our low and high mapping values
  unsigned int prox = constrain(reading, LOW_DIFF, HIGH_DIFF);
  
  // implement a simple (IIR lowpass) smoothing filter
  lastProx = (filterWeight*lastProx) + ((1-filterWeight)*(float)prox);

  // map the LOW_DIFF..HIGH_DIFF range to 0..255 (8-bit resolution for analogWrite)
  uint8_t thisOutput = (uint8_t)map(lastProx,LOW_DIFF,HIGH_DIFF,0,255);

  // output the mapped value to the LED
  analogWrite(LED_BUILTIN, thisOutput);


  if (reading < 20) {
    digitalWrite(CH10, LOW);
    delay(5000);
    

  

      

  }
   
    if (reading > 20) {
    
    digitalWrite(CH10, HIGH);
   // delay(2000);
   // digitalWrite(CH10, LOW);
   
} 
}
