#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
// AudioInputI2S_F32        audioInI2S1;    //xy=67.5,67
AudioInputI2S_F32        audioInI2S2;    //xy=116,333
AudioEffectGain_F32      gain2;          //xy=199,106
AudioEffectGain_F32      gain1;          //xy=208,51
AudioAnalyzeFFT1024_F32  FFT1024_1;      //xy=315,418
AudioMixer4_F32          mixer4_1;       //xy=479,200
AudioMixer4_F32          mixer4_2;       //xy=499,285
AudioEffectCompressor2_F32 compressor2_1;  //xy=543,102
AudioEffectCompressor2_F32 compressor2_2;  //xy=542,395
AudioOutputI2S_F32       audioOutI2S1;   //xy=786,215
// AudioConnection_F32          patchCord1(audioInI2S1, 0, gain1, 0);
// AudioConnection_F32          patchCord2(audioInI2S1, 1, gain2, 0);
AudioConnection_F32          patchCord3(audioInI2S2, 0, mixer4_1, 1);
AudioConnection_F32          patchCord4(audioInI2S2, 1, FFT1024_1, 0);
AudioConnection_F32          patchCord5(audioInI2S2, 1, mixer4_2, 1);
// AudioConnection_F32          patchCord6(gain2, 0, mixer4_2, 0);
// AudioConnection_F32          patchCord7(gain1, 0, mixer4_1, 0);
// AudioConnection_F32          patchCord8(mixer4_1, compressor2_1);
// AudioConnection_F32          patchCord9(mixer4_2, compressor2_2);

AudioConnection_F32          patchCord10(mixer4_1, 0, audioOutI2S1, 0);
AudioConnection_F32          patchCord11(mixer4_2, 0, audioOutI2S1, 1);
// AudioConnection_F32          patchCord10(compressor2_1, 0, audioOutI2S1, 0);
// AudioConnection_F32          patchCord11(compressor2_2, 0, audioOutI2S1, 1);
// GUItool: end automatically generated code
// uint32_t  mytime = 0;
// float amplitude = 1.0;
// float volume_floor = 0.0;
// int hold = 10;
// int release = 100;
// int a1history=0;
//
//
// void release_rise() {
//   float current_gain = volume_floor;
//   float step = (amplitude - volume_floor) / release;
//   for (int i = 0; i < release; i++) {
//     current_gain += step;
//     amp1.gain(current_gain);
//     amp2.gain(current_gain);
//     delay(1);
//   }
// }
//
// void duck() {
//  amp1.gain(volume_floor);
//  amp2.gain(volume_floor);
//  delay(hold);
//  release_rise();
// }
//
// // code for turning knob
// void wait(unsigned int milliseconds) {
//   elapsedMillis msec=0;
//     while (msec <= milliseconds) {
//       int a1 = analogRead(A1);
//       if (a1 > a1history + 50 || a1 < a1history - 50) {
//         Serial.print("Knob (pin A1) = ");
//         Serial.println(a1);
//         hold = a1/10;
//         a1history = a1;
//       }
//     }
// }
//

void setup() {
    AudioMemory(20);
    Serial.begin(115200);
    // a1history = analogRead(A1);
      // gain2.setGain_dB(0.0f);  // Consider (-50.0f);
  // gain1.setGain_dB(0.0f);  // Consider (-50.0f);
  int16_t delaySize = 256;     // Any power of 2, i.e., 256, 128, 64, etc.
  compressor2_1.setDelayBufferSize(delaySize);
  compressor2_2.setDelayBufferSize(delaySize);
   
  // Specify arbitrary 5-segment compression curve.  An example of specifying
  // compressionCurve.  See AudioEffectCompressor2_F32.h for more details.
  struct compressionCurve crv = { -2.0f, 0.0f,           // margin, offset
     {0.0f, -10.0f, -20.0f, -30.0f, -1000.0f},           // kneeDB[] 
     {  100.0f,  2.5f,   1.5f,     1.0f,      1.0f} };    // compressionRatio
  compressor2_1.setCompressionCurve(&crv);
  compressor2_1.begin();
  compressor2_2.setCompressionCurve(&crv);
  compressor2_2.begin();
  mixer4_1.gain(0, 1.0);
  mixer4_1.gain(1, 1.0);
  mixer4_2.gain(0,1.0);
  mixer4_2.gain(1,1.0);
}

void loop() {
    Serial.println(FFT1024_1.available());
    Serial.println(FFT1024_1.read(0,5));
      // amp1.gain(amplitude);
  // amp2.gain(amplitude);
  //   delay(4000);
  // if (fft1024_1.read(0,5) > 0.08) {
  // duck();

  // }

    // wait(250);
  delay(2000);
  // put your main code here, to run repeatedly:

}


