
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
#include <math.h>
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

float compression_threshold, compression_ratio, attack_ms, release_ms;

bool AudioEffectCompressor::set_default_values(float compression_threshold,
                                               float compression_ratio,
                                               float attack_ms,
                                               float release_ms) {
  compression_threshold = compression_threshold;
  compression_ratio = compression_ratio;
  attack_ms = attack_ms;
  release_ms = release_ms;

  return (true);
}

int max_sample = 32767;

float dBFS(short sample) {
  float output = sample;
  if (output == max_sample) {
    output = 0.0f;
    return output;
  }
  output = 20 * log10f(output / max_sample);
  return output;
}

// returns average level for given audio block
float calculate_volume_db(audio_block_t *block) {
  // Serial.println('calculate volume');
  short *data;
  data = block->data;
  float total = 0;
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int datum = data[i];
    // calculate the instantaneous signal power (square the signal)

    // Serial.print("level before squaring = ");
    // Serial.println(datum);
    datum = pow(datum, 2);
    datum = sqrt(datum);

    // convert signal power to dB
    // datum = log10f(datum);

    // Serial.print("level = ");
    // Serial.println(datum);
    // Serial.print("dBFS level = ");
    // Serial.println(dBFS(datum));
    datum = dBFS(datum);
    total += datum;
  }
  float sample_count = AUDIO_BLOCK_SAMPLES;
  float average = total / sample_count;
  // Serial.print("total = ");
  // Serial.println(total);
  Serial.print("average = ");
  Serial.println(average);
  return average;
}
int count = 0;
void AudioEffectCompressor::update(void) {
  audio_block_t *block;
  short *bp;
  block = receiveWritable(0);
  count++;
  if (count >= 10000) {
    count = 0;
  }

  if (!block)
    return;

  bp = block->data;
  if (count % 100 == 0) {
    float volume_db = calculate_volume_db(block);
  }

  // transmit the block and release memory
  transmit(block, 0);
  release(block);
}
