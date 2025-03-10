#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S2           i2s2_1;         //xy=116,264
AudioEffectGranular      granular1;      //xy=298,326
AudioOutputI2S           i2s1;           //xy=442,259
// AudioMixer4              mixer1;         //xy=279,269
AudioAmplifier           amp1;           //xy=253,291
AudioAmplifier           amp2;           //xy=253,291


// AudioConnection          patchCord1(i2s2_1, 0, mixer1, 0);
// AudioConnection          patchCord2(i2s2_1, 1, mixer1, 1);
// AudioConnection          patchCord3(mixer1, 0, i2s1, 1);
// AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
// AudioConnection          patchCord6(granular1, 0, mixer1, 1);
// AudioConnection          patchCord7(i2s2_1, 1, granular1, 0);
AudioConnection          patchCord6(i2s2_1, 1, amp1, 0);
AudioConnection          patchCord7(amp1, 0, i2s1, 0);
// AudioConnection          patchCord7(amp1, 0, mixer1, 0);
// AudioConnection          patchCord6(granular1, 0, i2s1, 1);
// AudioConnection          patchCord7(i2s2_1, 1, granular1, 0);

#define GRANULAR_MEMORY_SIZE 12800  // enough for 290 ms at 44.1 kHz
int16_t granularMemory[GRANULAR_MEMORY_SIZE];

uint32_t  mytime = 0;
float amplitude = 1.0;
float volume_floor = 0.0;
int hold = 140;
int release = 1000;

void setup() {
  AudioMemory(20);
    granular1.begin(granularMemory, GRANULAR_MEMORY_SIZE);

      // sine1.frequency(440);
      // mixer1.gain(3, 1.0);

}


void release_rise() {
  float current_gain = volume_floor;
  float step = (amplitude - volume_floor) / release;
  for (int i = 0; i < release; i++) {
    current_gain += step;
    // mixer1.gain(1, current_gain);
    delay(1);
  }
}

void duck() {
//  mixer1.gain(1, volume_floor);
 delay(hold);
 release_rise();
//  mixer1.gain(1, amplitude);
}


void loop() {
  // mixer1.gain(1,amplitude);
  //   delay(4000);
  duck();
  delay(4000);



}
