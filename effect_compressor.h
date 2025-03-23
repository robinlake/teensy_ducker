/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Pete (El Supremo)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef effect_compressor_h_
#define effect_compressor_h_

#include <Arduino.h> // github.com/PaulStoffregen/cores/blob/master/teensy4/Arduino.h
#include <AudioStream.h> // github.com/PaulStoffregen/cores/blob/master/teensy4/AudioStream.h
#include <arm_math.h> //ARM DSP extensions.  https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html

/******************************************************************/

//                A u d i o E f f e c t C h o r u s
// Written by Pete (El Supremo) Jan 2014
// 140219 - correct storage class (not static)

class AudioEffectCompressor : public AudioStream {
public:
  AudioEffectCompressor(void) : AudioStream(1, inputQueueArray) {}

  bool begin();
  bool set_default_values(float compression_threshold, float compression_ratio,
                          float attack_ms, float release_ms);
  virtual void update(void);
  void calcAudioLevel_dB(audio_block_t *audio_block,
                         audio_block_t *audio_level_dB_block);
  void calcGain(audio_block_t *gain_block, audio_block_t *level_dB_block);
  void calcInstantaneousTargetGain(audio_block_t *inst_targ_gain_dB_block,
                                   audio_block_t *audio_level_dB_block);
  void calcSmoothedGain_dB(audio_block_t *inst_targ_gain_dB_block,
                           audio_block_t *gain_dB_block);
  static float pow10f(float x);

private:
  audio_block_t *inputQueueArray[1];
  short *l_delayline;
  short l_circ_idx;
  int num_chorus;
  int delay_length;
  float threshold;
  float ratio;
  float attack_ms;
  float release_ms;
  float prev_level_lp_pow = 1.0;
  float prev_gain_dB = 0.0; // last gain^2 used
                            // HP filter state-related variables
  // arm_biquad_casd_df1_inst_f32 hp_filt_struct;
  static const uint8_t hp_nstages = 1;
  float hp_coeff[5 * hp_nstages] = {
      1.0, 0.0, 0.0, 0.0, 0.0}; // no filtering. actual filter coeff set later
  float hp_state[4 * hp_nstages];
  void updateThresholdAndCompRatioConstants(void);
  void setHPFilterCoeff(void);
  void setThreshPow(float t_pow);
  static float log2f_approx(float X);
};

#endif
