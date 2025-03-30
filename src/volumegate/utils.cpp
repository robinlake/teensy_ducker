#include <math.h>
short max_sample = 32767;
short min_sample = -32768;
float min_dBFS = -160.0f;
float sample_to_dbfs(short sample) {

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
