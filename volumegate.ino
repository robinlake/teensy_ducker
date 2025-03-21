#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "AudioEffectCompressor2_F32.h"
#include "AudioConvert_F32.h"
#include "AudioEffectGain_F32.h"
#include "AudioEffectGain_F32.h"
#include "output_i2s_f32.h"
// #include <OpenAudio_ArduinoLibrary.h>


AudioEffectCompressor2_F32  compressor1; // Audio Compressor s
AudioConvert_I16toF32     int2Float1 ;    //Converts Int16 to Float.  See class in AudioStream_F32.h
AudioConvert_F32toI16     float2Int1;    //Converts Float to Int16.  See class in AudioStream_F32.h
AudioEffectGain_F32         gain0;       // Sets volume sent to output
AudioEffectGain_F32         gain1;       // Sets the same
AudioOutputI2S_F32          i2sOut;
// GUItool: begin automatically generated code
AudioInputI2S2           i2s2_1;         //xy=180,351
AudioInputI2S            i2s1;           //xy=188,143
AudioAmplifier           amp4;           //xy=330,404
AudioAmplifier           amp3;           //xy=339,320
AudioAmplifier           amp1;           //xy=341,119
AudioAmplifier           amp2;           //xy=345,169
AudioAnalyzeFFT1024      fft1024_1;      //xy=350,225
AudioMixer4              mixer1;         //xy=537,243
AudioOutputI2S           i2s2;           //xy=682,206
AudioConnection          patchCord1(i2s2_1, 0, amp3, 0);
AudioConnection          patchCord2(i2s2_1, 1, amp4, 0);
AudioConnection          patchCord3(i2s1, 0, amp1, 0);
AudioConnection          patchCord4(i2s1, 1, amp2, 0);
AudioConnection          patchCord5(i2s1, 1, fft1024_1, 0);
AudioConnection          patchCord6(amp4, 0, mixer1, 3);
AudioConnection          patchCord7(amp3, 0, mixer1, 2);
AudioConnection          patchCord8(amp1, 0, mixer1, 0);
AudioConnection          patchCord9(amp2, 0, mixer1, 1);
AudioConnection          patchCord12(i2s2_1, 1, fft1024_1, 0);
AudioConnection          patchCord10(mixer1, 0, i2s2, 0);
AudioConnection          patchCord11(mixer1, 0, i2s2, 1);

// compressor connections
// AudioConnection          patchCord10(mixer1, 0, int2Float1, 0);
// AudioConnection          patchCord11(int2Float1, 0, compressor1, 1);
// AudioConnection          patchCord15(compressor1, 0, float2Int1, 1);
// AudioConnection          patchCord13(float2Int1, 0, i2s2, 0);
// AudioConnection          patchCord14(float2Int1, 0, i2s2, 1);
// end compressor connections

// GUItool: end automatically generated code

uint32_t  mytime = 0;
float amplitude = 1.0;
float volume_floor = 0.0;
int hold = 10;
int release = 100;
int a1history=0;


void release_rise() {
  float current_gain = volume_floor;
  float step = (amplitude - volume_floor) / release;
  for (int i = 0; i < release; i++) {
    current_gain += step;
    amp1.gain(current_gain);
    amp2.gain(current_gain);
    delay(1);
  }
}

void duck() {
 amp1.gain(volume_floor);
 amp2.gain(volume_floor);
 delay(hold);
 release_rise();
}

// code for turning knob
void wait(unsigned int milliseconds) {
  elapsedMillis msec=0;
    while (msec <= milliseconds) {
      int a1 = analogRead(A1);
      if (a1 > a1history + 50 || a1 < a1history - 50) {
        Serial.print("Knob (pin A1) = ");
        Serial.println(a1);
        hold = a1/10;
        a1history = a1;
      }
    }
}



void setup() {
    AudioMemory(20);
    AudioMemory_F32(100);
    Serial.begin(115200);
    a1history = analogRead(A1);

    // compressor stuff
    gain0.setGain_dB(-25.0f);  // Consider (-50.0f);
    gain1.setGain_dB(-25.0f);  // Consider (-50.0f);
    AudioEffectCompressor2_F32 *pc1 = &compressor1;
    basicCompressorBegin(pc1, -5.0f, 2.0);
    // basicCompressorBegin(pc1, -45.0f, 8.0);
    // end compressor stuff
}

void loop() {
    // Serial.println(fft1024_1.available());
    // Serial.println(fft1024_1.read(0,5));
      // amp1.gain(amplitude);
  // amp2.gain(amplitude);
  //   delay(4000);
  if (fft1024_1.read(0,5) > 0.08) {
  duck();

  }

    wait(250);
  // delay(2000);
  // put your main code here, to run repeatedly:

}


