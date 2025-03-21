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
// #include "AudioEffectCompressor2_F32.h"
#include "AudioEffectCompressor_F32.h"
#include "AudioConvert_F32.h"
// #include <OpenAudio_ArduinoLibrary.h>


// AudioEffectCompressor2_F32  compressor1; // Audio Compressor s
AudioConvert_I16toF32     int2Float1, int2Float2;    //Converts Int16 to Float.  See class in AudioStream_F32.h
AudioEffectCompressor_F32 comp1, comp2;
AudioConvert_F32toI16     float2Int1, float2Int2;    //Converts Float to Int16.  See class in AudioStream_F32.h
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
AudioAnalyzeRMS          rms1;           //xy=686,291
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
AudioConnection          patchCord122(mixer1, rms1);
// AudioConnection          patchCord10(mixer1, 0, i2s2, 0);
// AudioConnection          patchCord11(mixer1, 0, i2s2, 1);

// just testing if we can convert to float32, convert back, and still hear sound
// AudioConnection          patchCord10(mixer1, 0, int2Float1, 0);
// AudioConnection          patchCord11(int2Float1, 0, float2Int1, 1);
// AudioConnection          patchCord111(float2Int1, 0, i2s2, 1);
// end just testing


// compressor connections
AudioConnection          patchCord10(mixer1, 0, int2Float1, 0);
AudioConnection          patchCord101(mixer1, 0, int2Float2, 0);
AudioConnection          patchCord11(int2Float1, 0, comp1, 1);
AudioConnection          patchCord111(int2Float2, 0, comp2, 1);
AudioConnection_F32     patchCord121(comp1, 0, float2Int1, 0); //Left.  makes Float connections between objects
AudioConnection_F32     patchCord13(comp2, 0, float2Int2, 0); //Right.  makes Float connections between objects
// AudioConnection          patchCord15(compressor1, 0, float2Int1, 1);
AudioConnection          patchCord14(float2Int1, 0, i2s2, 0);
AudioConnection          patchCord141(float2Int2, 0, i2s2, 1);
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

//define a function to configure the left and right compressors
void setupMyCompressors(boolean use_HP_filter, float knee_dBFS, float comp_ratio, float attack_sec, float release_sec) {
  comp1.enableHPFilter(use_HP_filter);   comp2.enableHPFilter(use_HP_filter);
  comp1.setThresh_dBFS(knee_dBFS);       comp2.setThresh_dBFS(knee_dBFS);
  comp1.setCompressionRatio(comp_ratio); comp2.setCompressionRatio(comp_ratio);

  float fs_Hz = AUDIO_SAMPLE_RATE;
  comp1.setAttack_sec(attack_sec, fs_Hz);       comp2.setAttack_sec(attack_sec, fs_Hz);
  comp1.setRelease_sec(release_sec, fs_Hz);     comp2.setRelease_sec(release_sec, fs_Hz);
}

void printCompressorState(Stream *s) {
  s->print("Current Compressor: Pre-Gain (dB) = ");
  s->print(comp1.getPreGain_dB());
  s->print(", Level (dBFS) = ");
  s->print(comp1.getCurrentLevel_dBFS());
  s->print(", ");
  s->print(comp2.getCurrentLevel_dBFS());
  s->print(", Dynamic Gain L/R (dB) = ");
  s->print(comp1.getCurrentGain_dB());
  s->print(", ");
  s->print(comp2.getCurrentGain_dB());
  s->println();
};

void printMixerState(Stream *s) {
  s->print("Current Mixer Gain = ");
  s->print(rms1.read());
  s->println("");
}

void printFloatState(Stream *s) {
  s->print("Is Float 1 Active = ");
  s->print(int2Float1.isActive());
    s->print(int2Float2.isActive());
      s->print(float2Int1.isActive());
      s->print(float2Int2.isActive());
  s->println("");
    s->print("float2Int1  = ");
          s->print(float2Int2.f32_memory_used);
  s->println("");

}

// void setupFloats() {
//   int2Float1.initialize_f32_memory();
// }
//
void printFFTState(Stream *s) {
    s->print("FFT Value = ");
  s->print(fft1024_1.read(0,5));
  s->println("");
}

void setup() {
    AudioMemory(20);
    AudioMemory_F32(16);
    Serial.begin(115200);
    a1history = analogRead(A1);
    // setupFLoats();

    // compressor stuff
    // gain0.setGain_dB(-25.0f);  // Consider (-50.0f);
    // gain1.setGain_dB(-25.0f);  // Consider (-50.0f);
    // AudioEffectCompressor2_F32 *pc1 = &compressor1;
    // basicCompressorBegin(pc1, -5.0f, 2.0);
    // basicCompressorBegin(pc1, -45.0f, 8.0);
      //choose the compressor parameters...note that preGain is set by the potentiometer in the main loop()
    // boolean use_HP_filter = true; //enable the software HP filter to get rid of DC?
    boolean use_HP_filter = false; //enable the software HP filter to get rid of DC?
    float knee_dBFS, comp_ratio, attack_sec, release_sec;
    if (false) {
        Serial.println("Configuring Compressor for fast response for use as a limitter.");
        knee_dBFS = -5.0f; comp_ratio = 2.0f;  attack_sec = 0.005f;  release_sec = 0.200f;
        //knee_dBFS = -15.0f; comp_ratio = 5.0f;  attack_sec = 0.005f;  release_sec = 0.200f;
    } else {
        Serial.println("Configuring Compressor for slow response more like an automatic volume control.");
        knee_dBFS = -50.0; comp_ratio = 5.0;  attack_sec = 1.0;  release_sec = 2.0;
    }

    //configure the left and right compressors with the desired settings
    setupMyCompressors(use_HP_filter, knee_dBFS, comp_ratio, attack_sec, release_sec);
    // end compressor stuff
}

unsigned long updatePeriod_millis = 100; //how many milliseconds between updating gain reading?
unsigned long lastUpdate_millis = 0;
unsigned long curTime_millis = 0;
int prev_gain_dB = 0;
unsigned long lastMemUpdate_millis = 0;
void loop() {
    // Serial.println(fft1024_1.available());
      // amp1.gain(amplitude);
  // amp2.gain(amplitude);
  //   delay(4000);
  // if (fft1024_1.read(0,5) > 0.08) {
  // duck();

  // }
  

    // wait(450);
    curTime_millis = millis(); //what time is it right now
    printCompressorState(&Serial);
    printMixerState(&Serial);
    printFFTState(&Serial);
    printFloatState(&Serial);
  delay(1000);
  // put your main code here, to run repeatedly:

}


