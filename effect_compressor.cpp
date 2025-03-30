
/* Audio Library for Teensy 3.X, 4.X
 * Copyright (c) 2025, Lily Lake
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

bool AudioEffectCompressor::begin(float compression_threshold = -20.0f,
                                  float compression_ratio = 3.0f,
                                  float attack_ms = 1.0f,
                                  float release_ms = 30.0f) {

  Serial.println("begin compressor");
  set_default_values(compression_threshold, compression_ratio, attack_ms,
                     release_ms);

  return (true);
}

float compression_threshold, compression_ratio, attack_ms, release_ms;

bool AudioEffectCompressor::set_default_values(float compression_threshold,
                                               float compression_ratio,
                                               float attack_ms,
                                               float release_ms) {
  this->compression_threshold = compression_threshold;
  this->compression_ratio = compression_ratio;
  this->attack_ms = attack_ms;
  this->release_ms = release_ms;

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

  // float output = sqrt(pow(sample, 2));
  float output = sample;
  if (sample > 0) {
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

// dBFS = 20 * log(A / Amax)
// dBFS / 20 = log(A / Amax)
// dBFS / 20 = log(A) - log(Amax)
// dBFS / 20 + log(Amax) = log(A)
short dBFS_to_sample(float dBFS, bool negative_signal) {
  float log_of_max = log10f(max_sample);
  float left_side;
  left_side = log_of_max + (dBFS / 20.0f);
  float answer = powf(10, left_side);
  if (negative_signal) {
    answer *= -1.0f;
  }
  if (count % 3000 == 0) {

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
float AudioEffectCompressor::calculate_average_volume_db(audio_block_t *block) {
  count++;
  if (count >= 3000) {
    count = 0;
  }
  short *data;
  data = block->data;
  float total = 0.0f;
  float sample_count = 0.0f;
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float datum = data[i];

    datum = sample_to_dBFS(datum);
    total += datum;
    sample_count += 1.0f;
  }
  float average = total / sample_count;
  if (count % 3000 == 0) {
    Serial.print("total = ");
    Serial.println(total);
    Serial.print("average = ");
    Serial.println(average);
  }

  return average;
}

float ms_to_samples(float ms) {
  float ms_per_sample = 2.9f / AUDIO_BLOCK_SAMPLES;
  float samples = ms / ms_per_sample;
  // if (count % 3000 == 0) {
  //   Serial.print("audio block samples");
  //   Serial.println(AUDIO_BLOCK_SAMPLES);
  //   Serial.print("ms_per_sample = ");
  //   Serial.println(ms_per_sample);
  // }
  return samples;
}

float attack_samples_elapsed = 0;
int release_samples_elapsed = 0;

float AudioEffectCompressor::apply_attack_ratio(float reduction) {
  float attack_samples = ms_to_samples(this->attack_ms);
  float ratio = attack_samples_elapsed / attack_samples;
  return reduction * ratio;
}

float AudioEffectCompressor::apply_release_ratio(float reduction) {
  float release_samples = ms_to_samples(this->release_ms);
  float ratio = release_samples_elapsed / release_samples;
  return reduction * ratio;
}

float AudioEffectCompressor::apply_ballistics(float reduction) {
  float ratio;
  if (attack_samples_elapsed > 0) {
    ratio = apply_attack_ratio(reduction);
    return ratio;
  }
  if (release_samples_elapsed > 0) {

    ratio = apply_release_ratio(reduction);
    return ratio;
  }
  ratio = 0.0f;
  return ratio;
}

float AudioEffectCompressor::compress_dBFS(float dBFS) {
  if (dBFS < compression_threshold) {
    return dBFS;
  }
  // amount that signal exceeds threshold
  // float difference = dBFS - this->compression_threshold;
  // float gain = difference / this->compression_ratio;
  // float output = this->compression_threshold + gain;

  float reduction = dBFS - this->compression_threshold;
  // amount cut can only be proportional to point in attack curve
  // reduction = apply_attack_ratio(reduction);
  reduction = apply_ballistics(reduction);
  // makeup gain
  float makeup_gain = reduction / this->compression_ratio;
  float output = dBFS - reduction + makeup_gain;

  // if (count % 3000 == 0) {
  //   Serial.print("compression_threshold = ");
  //   Serial.println(compression_threshold);
  //   Serial.print("compression_ratio = ");
  //   Serial.println(compression_ratio);
  //   Serial.print("dBFS value = ");
  //   Serial.println(dBFS);
  //   Serial.print("difference = ");
  //   Serial.println(difference);
  //   Serial.print("gain = ");
  //   Serial.println(gain);
  //   Serial.print("output = ");
  //   Serial.println(output);
  // }
  return output;
}
short original_samples[AUDIO_BLOCK_SAMPLES];
float dBFS_samples[AUDIO_BLOCK_SAMPLES];
float dBFS_samples_compressed[AUDIO_BLOCK_SAMPLES];
short pcm_samples_compressed[AUDIO_BLOCK_SAMPLES];

short AudioEffectCompressor::compress_sample(short sample, int i) {
  bool is_negative = false;
  if (sample < 0) {
    is_negative = true;
  }
  float sample_dBFS = sample_to_dBFS(sample);
  dBFS_samples[i] = sample_dBFS;
  float compressed_dBFS = compress_dBFS(sample_dBFS);
  dBFS_samples_compressed[i] = compressed_dBFS;
  short compressed_sample = dBFS_to_sample(compressed_dBFS, is_negative);
  pcm_samples_compressed[i] = compressed_sample;
  return compressed_sample;
}

void increment_atttack(float attack_samples) {
  if (attack_samples_elapsed < attack_samples) {
    attack_samples_elapsed++;
  }
}

void AudioEffectCompressor::update(void) {
  audio_block_t *block;
  block = receiveWritable(0);

  if (!block)
    return;

  audio_block_t *compressed_block;
  compressed_block = AudioEffectCompressor::allocate();
  float volume_db = calculate_average_volume_db(block);

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    short sample = block->data[i];
    float sample_volume_db = sample_to_dBFS(sample);
    if (sample_volume_db > compression_threshold) {
      if (attack_samples_elapsed < ms_to_samples(this->attack_ms)) {
        attack_samples_elapsed++;
      }
      compressed_block->data[i] = compress_sample(sample, i);
    } else {
      compressed_block->data[i] = sample;
    }
  }
  if (count % 3000 == 0) {
    Serial.print("uncompressed values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(block->data[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("dBFS values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dBFS_samples[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("compressed dBFS values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dBFS_samples_compressed[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("compressed PCM values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(pcm_samples_compressed[i]);
      Serial.print(", ");
    }
    Serial.println("");
    Serial.println(" ");
  }
  transmit(compressed_block, 0);
  release(compressed_block);
  release(block);
}
