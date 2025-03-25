
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

  Serial.println("begin compressor");
  set_default_values(-30.0f, 3.0f, 10.0f, 30.0f);

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
  this->compression_threshold = compression_threshold;
  this->compression_ratio = compression_ratio;
  this->attack_ms = attack_ms;
  this->release_ms = release_ms;
  Serial.println("setting default values");

  return (true);
}

int max_sample = 32767;
int min_sample = -32768;
int count = 0;
// still giving some eroneous values
// they are very negative numbers
// temporary fix: set to minimum value for these numbers
float min_dBFS = -160.0f;
float sample_to_dBFS(int sample) {
  float output = sample;
  if (sample == 0) {
    output = 0.0f;
    return output;
  } else if (sample > 0) {
    if (sample == max_sample) {
      output = 0.0f;
      return output;
    }
    output = 20 * log10f(output / max_sample);
    if (output < min_dBFS) {
      return min_dBFS;
    }
    return output;
  } else {
    if (sample == min_sample) {
      output = 0.0f;
      return output;
    }
    output = 20.0f * log10f((output * -1.0f) / (min_sample * -1.0f));
    if (output < min_dBFS) {
      return min_dBFS;
    }
    return output;
  }
}

// float sample_to_dBFS(int sample) {
//
//   float output = sample;
//   if (output == max_sample) {
//     output = 0.0f;
//     return output;
//   }
//   output = 20 * log10f(output / max_sample);
//   if (output < min_dBFS) {
//     return min_dBFS;
//   }
//   return output;
// }

// dBFS = 20 * log(A / Amax)
// dBFS / 20 = log(A / Amax)
// dBFS / 20 = log(A) - log(Amax)
// dBFS / 20 + log(Amax) = log(A)
short dBFS_to_sample(float dBFS, bool negative_signal) {
  // todo: apply the opposite of the dBFS function
  // convert back into a short that cen be used for output
  float log_of_max = log10f(max_sample);
  float left_side;
  left_side = log_of_max + (dBFS / 20.0f);
  float answer = powf(10, left_side);
  if (negative_signal) {
    answer *= -1.0f;
  }
  if (count % 1000 == 0) {

    // Serial.print("dBFS = ");
    // Serial.println(dBFS);
    // Serial.print("log of max = ");
    // Serial.println(log_of_max);
    // Serial.print("left side = ");
    // Serial.println(left_side);
    // Serial.print("answer = ");
    // Serial.println(answer);
  }
  short output = answer;
  return output;
}

// returns average level for given audio block
float calculate_average_volume_db(audio_block_t *block) {
  count++;
  if (count >= 10000) {
    count = 0;
  }
  short *data;
  data = block->data;
  float total = 0.0f;
  float dBFS_values[AUDIO_BLOCK_SAMPLES];
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int datum = data[i];

    // calculate the instantaneous signal power (square the signal)
    datum = pow(datum, 2);
    datum = sqrt(datum);
    datum = sample_to_dBFS(datum);
    total += datum;
    dBFS_values[i] = datum;
  }
  float sample_count = AUDIO_BLOCK_SAMPLES;
  float average = total / sample_count;
  if (count % 1000 == 0) {
    Serial.print("dBFS values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dBFS_values[i]);
      Serial.print(", ");
    }
    Serial.println("");
    Serial.print("total = ");
    Serial.println(total);
    Serial.print("average = ");
    Serial.println(average);
  }

  return average;
}

void compress_block(audio_block_t *block) {
  short *samples;
  samples = block->data;
  short original_samples[AUDIO_BLOCK_SAMPLES];
  short dBFS_samples[AUDIO_BLOCK_SAMPLES];
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    original_samples[i] = samples[i];
    int sample = samples[i];
    bool is_negative = false;
    if (sample < 0) {
      is_negative = true;
    }
    float sample_dbfs = sample_to_dBFS(sample);
    dBFS_samples[i] = sample_dbfs;
    // todo: apply compression ratio to sample_dbfs
    short compressed_sample = dBFS_to_sample(sample_dbfs, is_negative);
    samples[i] = compressed_sample;
  }

  if (count % 1000 == 0) {
    Serial.print("uncompressed values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(original_samples[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("dBFS values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dBFS_samples[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("compressed values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(samples[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }
}
void AudioEffectCompressor::update(void) {
  audio_block_t *block;
  // short *bp;
  block = receiveWritable(0);

  if (!block)
    return;

  // bp = block->data;

  // audio_block_t *compressed_block = AudioEffectCompressor::allocate(20);
  audio_block_t *compressed_block;
  compressed_block = AudioEffectCompressor::allocate();
  float volume_db = calculate_average_volume_db(block);
  if (count % 100 == 0) {
    // Serial.print("level = ");
    // Serial.println(volume_db);
    // Serial.print("threshold = ");
    // Serial.println(compression_threshold);
  }

  if (volume_db > compression_threshold) {
    // copy original block to compressed_block
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      compressed_block->data[i] = block->data[i];
    }
    compress_block(compressed_block);
    // Serial.println("trigger compression");

    if (count % 1000 == 0) {
      Serial.print("block samples: ");
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        Serial.print(block->data[i]);
        Serial.print(", ");
      }
      Serial.println("");
      Serial.print("compressed block samples: ");
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        Serial.print(compressed_block->data[i]);
        Serial.print(", ");
      }
      Serial.println("");
    }

    transmit(compressed_block, 0);
    release(compressed_block);
    release(block);
  } else {
    // transmit the block and release memory
    transmit(block, 0);
    release(block);
    release(compressed_block);
  }
}
