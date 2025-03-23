
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

#include "effect_compressor.h"
#include <Arduino.h>
#include <arm_math.h> //ARM DSP extensions.
// https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html

/******************************************************************/

//                A u d i o E f f e c t C o m p r e s s o r

bool AudioEffectCompressor::begin() {
#if 0
Serial.print("AudioEffectCompressor.begin(Chorus delay line length = ");
Serial.print(d_length);
Serial.print(", n_chorus = ");
Serial.print(n_chorus);
Serial.println(")");
#endif

  return (true);
}

/*
 * Actual compressor code
 */

// settings
float attack_sec, release_sec, level_lp_sec;
float thresh_dBFS =
    0.0; // threshold for compression, relative to digital full scale
float thresh_pow_FS = 1.0f; // same as above, but not in dB
void AudioEffectCompressor::setThreshPow(float t_pow) {
  thresh_pow_FS = t_pow;
  updateThresholdAndCompRatioConstants();
}
float comp_ratio = 1.0; // compression ratio
float pre_gain =
    -1.0; // gain to apply before the compression.  negative value disables
bool use_HP_prefilter;
//
// private parameters related to gain calculation
float attack_const, release_const,
    level_lp_const; // used in calcGain().  set by setAttack_sec() and
                    // setRelease_sec();
float comp_ratio_const,
    thresh_pow_FS_wCR; // used in calcGain();  set in
                       // updateThresholdAndCompRatioConstants()
float prev_level_lp_pow = 1.0;
float prev_gain_dB = 0.0;
void AudioEffectCompressor::updateThresholdAndCompRatioConstants(void) {
  comp_ratio_const = 1.0f - (1.0f / comp_ratio);
  thresh_pow_FS_wCR = powf(thresh_pow_FS, comp_ratio_const);
}

bool AudioEffectCompressor::set_default_values(float compression_threshold,
                                               float compression_ratio,
                                               float compressor_attack,
                                               float compressor_release) {
  threshold = compression_threshold;
  ratio = compression_ratio;
  attack_ms = compressor_attack;
  release_ms = compressor_release;

  return (true);
}

void setHPFilterCoeff(void) {
  // https://www.keil.com/pack/doc/CMSIS/DSP/html/group__BiquadCascadeDF1.html#ga8e73b69a788e681a61bccc8959d823c5
  // Use matlab to compute the coeff for HP at 20Hz:
  // [b,a]=butter(2,20/(44100/2),'high'); %assumes fs_Hz = 44100
  float b[] = {9.979871156751189e-01, -1.995974231350238e+00,
               9.979871156751189e-01}; // from Matlab
  float a[] = {1.000000000000000e+00, -1.995970179642828e+00,
               9.959782830576472e-01}; // from Matlab
  // setHPFilterCoeff_N2IIR_Matlab(b, a);
  // hp_coeff[0] = b[0];   hp_coeff[1] = b[1];  hp_coeff[2] = b[2]; //here are
  // the matlab "b" coefficients hp_coeff[3] = -a[1];  hp_coeff[4] = -a[2];
  // //the DSP needs the "a" terms to have opposite sign vs Matlab
}

void AudioEffectCompressor::update(void) {
  audio_block_t *block;
  short *bp;
  block = receiveWritable(0);

  if (!block)
    return;

  bp = block->data;
  //
  // // calculate audio level (ie, a smoothed version of the signal power)
  // audio_block_t *audio_level_dB_block = AudioStream::allocate();
  // calcAudioLevel_dB(block, audio_level_dB_block);
  //
  // // compute the desired gain based on the observed audio level
  // audio_block_t *gain_block = AudioStream::allocate();
  // calcGain(audio_level_dB_block, gain_block);
  //
  // transmit the block and release memory
  transmit(block, 0);
  release(block);
}

// Estimates the level of the audio (in dB)
// It swuares the signal and low-pass filters to get a time-averaged signal
// power
void AudioEffectCompressor::calcAudioLevel_dB(
    audio_block_t *audio_block, audio_block_t *audio_level_dB_block) {
  return;
}

// Computes the desired gain from the compressor. given an estimate
// of the signal level (in dB)
void AudioEffectCompressor::calcGain(audio_block_t *gain_block,
                                     audio_block_t *audio_level_dB_block) {
  // first, calculate the instantaneous target gain based on the compression
  // ratio
  audio_block_t *inst_targ_gain_dB_block = AudioStream::allocate();
  calcInstantaneousTargetGain(audio_level_dB_block, inst_targ_gain_dB_block);
  return;
}

// Compute the instantaneous desired gain, including the compression ratio and
// threshold for where the compression kicks in
void AudioEffectCompressor::calcInstantaneousTargetGain(
    audio_block_t *audio_level_dB_block,
    audio_block_t *inst_targ_gain_dB_block) {
  return;
}
// target gain level through time
void AudioEffectCompressor::calcSmoothedGain_dB(
    audio_block_t *inst_targ_gain_dB_block, audio_block_t *gain_dB_block) {
  return;
}
// Accelerate the powf(10.0,x) function
static float AudioEffectCompressor::pow10f(float x) {
  // return powf(10.0f,x)   //standard, but slower
  return expf(2.302585092994f * x); // faster:  exp(log(10.0f)*x)
}

// Accelerate the log10f(x)  function?
static float log10f_approx(float x) {
  // return log10f(x);   //standard, but slower
  return log2f_approx(x) * 0.3010299956639812f; // faster:  log2(x)/log2(10)
}

/* ----------------------------------------------------------------------
** Fast approximation to the log2() function.  It uses a two step
** process.  First, it decomposes the floating-point number into
** a fractional component F and an exponent E.  The fraction component
** is used in a polynomial approximation and then the exponent added
** to the result.  A 3rd order polynomial is used and the result
** when computing db20() is accurate to 7.984884e-003 dB.
** ------------------------------------------------------------------- */
// https://community.arm.com/tools/f/discussions/4292/cmsis-dsp-new-functionality-proposal/22621#22621
// float log2f_approx_coeff[4] = {1.23149591368684f,
// -4.11852516267426f, 6.02197014179219f, -3.13396450166353f};
static float AudioEffectCompressor::log2f_approx(float X) {
  // float *C = &log2f_approx_coeff[0];
  float Y;
  float F;
  int E;

  // This is the approximation to log2()
  F = frexpf(fabsf(X), &E);
  //  Y = C[0]*F*F*F + C[1]*F*F + C[2]*F + C[3] + E;
  // Y = *C++;
  Y = 1.23149591368684f;
  Y *= F;
  // Y += (*C++);
  Y += -4.11852516267426f;
  Y *= F;
  // Y += (*C++);
  Y += 6.02197014179219f;
  Y *= F;
  // Y += (*C++);
  Y += -3.13396450166353f;
  Y += E;

  return (Y);
}

// methods to set parameters of this module
void resetStates(void) {
  prev_level_lp_pow = 1.0f;
  prev_gain_dB = 0.0f;

  // initialize the HP filter.  (This also resets the filter states,)
      arm_biquad_cascade_df1_init_f32(&hp_filt_struct, hp_nstagehttps://www.keil.com/pack/doc/CMSIS/DSP/html/index.htmls, hp_coeff, hp_state);
}

// methods to return information about this module
float getPreGain_dB(void) { return 20.0 * log10f_approx(pre_gain); }
float getAttack_sec(void) { return attack_sec; }
float getRelease_sec(void) { return release_sec; }
float getLevelTimeConst_sec(void) { return level_lp_sec; }
float getThresh_dBFS(void) { return thresh_dBFS; }
float getCompressionRatio(void) { return comp_ratio; }
float getCurrentLevel_dBFS(void) {
  return 10.0 * log10f_approx(prev_level_lp_pow);
}
float getCurrentGain_dB(void) { return prev_gain_dB; }
