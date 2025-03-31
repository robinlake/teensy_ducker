
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
#include "utils.h"
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

int count = 0;

// returns average level for given audio block
float AudioEffectCompressor::calculate_average_volume_db(audio_block_t *block) {
  count++;
  if (count >= 3000) {
    count = 0;
  }
  short *data;
  data = block->data;
  float total = 0.0f;
  float dbfs_values[AUDIO_BLOCK_SAMPLES];
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int datum = data[i];

    // calculate the instantaneous signal power (square the signal)
    // datum = pow(datum, 2);
    // datum = sqrt(datum);
    datum = sample_to_dbfs(datum);
    total += datum;
    dbfs_values[i] = datum;
  }
  float sample_count = AUDIO_BLOCK_SAMPLES;
  float average = total / sample_count;
  if (count % 3000 == 0) {
    Serial.print("dbfs values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dbfs_values[i]);
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
  // if (count % 3000 == 0) {
  //   Serial.print("attack ms = ");
  //   Serial.println(this->attack_ms);
  //   Serial.print("attack samples = ");
  //   Serial.println(attack_samples);
  //   Serial.print("attack samples elapsed = ");
  //   Serial.println(attack_samples_elapsed);
  //   Serial.print("attack ratio = ");
  //   Serial.println(ratio);
  // }
  return reduction * ratio;
}

float AudioEffectCompressor::apply_release_ratio(float reduction) {
  float release_samples = ms_to_samples(this->release_ms);
  float ratio = release_samples_elapsed / release_samples;
  // if (count % 3000 == 0) {
  //   Serial.print("release ms = ");
  //   Serial.println(this->release_ms);
  //   Serial.print("release samples = ");
  //   Serial.println(release_samples);
  //   Serial.print("release samples elapsed = ");
  //   Serial.println(release_samples_elapsed);
  //   Serial.print("release ratio = ");
  //   Serial.println(ratio);
  // }
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

float AudioEffectCompressor::compress_dbfs(float dbfs) {
  if (dbfs < compression_threshold) {
    return dbfs;
  }
  // amount that signal exceeds threshold
  // float difference = dbfs - this->compression_threshold;
  // float gain = difference / this->compression_ratio;
  // float output = this->compression_threshold + gain;

  float reduction = dbfs - this->compression_threshold;
  // amount cut can only be proportional to point in attack curve
  // reduction = apply_attack_ratio(reduction);
  reduction = apply_ballistics(reduction);
  // makeup gain
  float makeup_gain = reduction / this->compression_ratio;
  float output = dbfs - reduction + makeup_gain;

  // if (count % 3000 == 0) {
  //   Serial.print("compression_threshold = ");
  //   Serial.println(compression_threshold);
  //   Serial.print("compression_ratio = ");
  //   Serial.println(compression_ratio);
  //   Serial.print("dbfs value = ");
  //   Serial.println(dbfs);
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
short dbfs_samples[AUDIO_BLOCK_SAMPLES];
short dbfs_samples_compressed[AUDIO_BLOCK_SAMPLES];
short pcm_samples_compressed[AUDIO_BLOCK_SAMPLES];

short AudioEffectCompressor::compress_sample(short sample, int i) {
  float sample_dbfs = sample_to_dbfs(sample);
  dbfs_samples[i] = sample_dbfs;
  float compressed_dbfs = compress_dbfs(sample_dbfs);
  dbfs_samples_compressed[i] = compressed_dbfs;
  bool is_negative = false;
  if (sample < 0) {
    is_negative = true;
  }
  short compressed_sample = dbfs_to_sample(compressed_dbfs, is_negative);
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
  // remaking compress_block equivalent
  if (count % 3000 == 0) {
    Serial.print("uncompressed values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(block->data[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    short sample = block->data[i];
    float sample_volume_db = sample_to_dbfs(sample);
    if (sample_volume_db > compression_threshold) {
      if (attack_samples_elapsed < ms_to_samples(this->attack_ms)) {
        attack_samples_elapsed++;
      }
      compressed_block->data[i] = compress_sample(sample, i);
    } else {
      compressed_block->data[i] = sample;
    }
    // compressed_block->data[i] = block->data[i];
  }
  if (count % 3000 == 0) {
    Serial.print("uncompressed values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(block->data[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("dbfs values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dbfs_samples[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("compressed dbfs values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(dbfs_samples_compressed[i]);
      Serial.print(", ");
    }
    Serial.println("");

    Serial.print("compressed PCM values = ");
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      Serial.print(pcm_samples_compressed[i]);
      Serial.print(", ");
    }
    Serial.println("");
  }
  transmit(compressed_block, 0);
  release(compressed_block);
  release(block);
}
