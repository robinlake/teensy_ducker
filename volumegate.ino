#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S2           i2s2_1;         //xy=116,264
AudioOutputI2S           i2s1;           //xy=442,259
AudioAmplifier           amp1;           //xy=253,291
AudioAmplifier           amp2;           //xy=253,291
AudioConnection          patchCord1(i2s2_1, 0, amp1, 0);
AudioConnection          patchCord2(amp1, 0, i2s1, 0);
AudioConnection          patchCord3(i2s2_1, 1, amp1, 0);
AudioConnection          patchCord4(amp1, 0, i2s1, 1);

uint32_t  mytime = 0;
float amplitude = 1.0;
float volume_floor = 0.0;
int hold = 140;
int release = 1000;

void setup() {
  AudioMemory(20);
}


void release_rise() {
  float current_gain = volume_floor;
  float step = (amplitude - volume_floor) / release;
  for (int i = 0; i < release; i++) {
    current_gain += step;
    // amp1.gain( current_gain);
    // amp2.gain( current_gain);
    amp1.gain(current_gain);
    amp2.gain(current_gain);
    delay(1);
  }
}

void duck() {
//  amp1.gain( volume_floor);
//  amp2.gain( volume_floor);
 delay(hold);
 release_rise();
//  amp1.gain(1, amplitude);
//  amp2.gain(1, amplitude);
}


void loop() {
  // amp1.gain(amplitude);
  // amp2.gain(amplitude);
  //   delay(4000);
  duck();
  delay(4000);



}
