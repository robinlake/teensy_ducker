
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

/******************************************************************/

//                A u d i o E f f e c t C h o r u s
// Written by Pete (El Supremo) Jan 2014
// 140529 - change to handle mono stream - change modify() to voices()
// 140219 - correct storage class (not static)

boolean AudioEffectCompressor::begin(short *delayline, int d_length,
                                     int n_chorus) {
#if 0
Serial.print("AudioEffectCompressor.begin(Chorus delay line length = ");
Serial.print(d_length);
Serial.print(", n_chorus = ");
Serial.print(n_chorus);
Serial.println(")");
#endif

  l_delayline = NULL;
  delay_length = 0;
  l_circ_idx = 0;
  //
  //  if(delayline == NULL) {
  //    return(false);
  //  }
  //  if(d_length < 10) {
  //    return(false);
  //  }
  //  if(n_chorus < 1) {
  //    return(false);
  //  }
  //
  l_delayline = delayline;
  delay_length = d_length / 2;
  num_chorus = n_chorus;
  //
  return (true);
}

void AudioEffectCompressor::voices(int n_chorus) { num_chorus = n_chorus; }

// int last_idx = 0;
void AudioEffectCompressor::update(void) {
  audio_block_t *block;
  short *bp;
  // int sum;
  // int c_idx;

  if (l_delayline == NULL)
    return;

  // do passthru
  // It stores the unmodified data in the delay line so that
  // it isn't as likely to click
  // if(num_chorus <= 1) {
  //   // Just passthrough
  //   block = receiveWritable(0);
  //   if(block) {
  //     bp = block->data;
  //     for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++) {
  //       l_circ_idx++;
  //       if(l_circ_idx >= delay_length) {
  //         l_circ_idx = 0;
  //       }
  //       l_delayline[l_circ_idx] = *bp++;
  //     }
  //     transmit(block,0);
  //     release(block);
  //   }
  //   return;
  // }
  //
  //          L E F T  C H A N N E L

  block = receiveWritable(0);
  if (block) {
    bp = block->data;
    // uint32_t tmp = delay_length/(num_chorus - 1) - 1;
    // for(int i = 0;i < AUDIO_BLOCK_SAMPLES;i++) {
    //   l_circ_idx++;
    //   if(l_circ_idx >= delay_length) {
    //     l_circ_idx = 0;
    //   }
    //   l_delayline[l_circ_idx] = *bp;
    //   sum = 0;
    //   c_idx = l_circ_idx;
    //   for(int k = 0; k < num_chorus; k++) {
    //     sum += l_delayline[c_idx];
    //     if(num_chorus > 1)c_idx -= tmp;
    //     if(c_idx < 0) {
    //       c_idx += delay_length;
    //     }
    //   }
    //   *bp++ = sum/num_chorus;
    // }

    // transmit the block
    transmit(block, 0);
    release(block);
  }
}

/*
 * Actual compressor code
 */
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

void AudioEffectCompressor::realupdate(void) {
  audio_block_t *block;
  block = receiveWritable(0);

  if (!block)
    return;

  // calculate audio level (ie, a smoothed version of the signal power)
  audio_block_t *audio_level_dB_block = AudioStream::allocate();
  calcAudioLevel_dB(block, audio_level_dB_block);

  // compute the desired gain based on the observed audio level
  audio_block_t *gain_block = AudioStream::allocate();
  calcGain(audio_level_dB_block, gain_block);

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
