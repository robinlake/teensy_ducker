#include <math.h>
short max_sample = 32767;
short min_sample = -32768;
float min_dbfs = -160.0f;
float sample_to_dbfs(short sample) {

  // float output = sqrt(pow(sample, 2));
  float output = sample;
  if (sample > 0) {
    if (sample == max_sample) {
      output = 0.0f;
      return output;
    }
    output = 20 * log10f(output / max_sample);
    if (output < min_dbfs) {
      return min_dbfs;
    }
    return output;
  } else {
    if (sample == min_sample) {
      output = 0.0f;
      return output;
    }
    output = 20.0f * log10f((output * -1.0f) / (min_sample * -1.0f));
    if (output < min_dbfs) {
      return min_dbfs;
    }
    return output;
  }
}

// dbfs = 20 * log(A / Amax)
// dbfs / 20 = log(A / Amax)
// dbfs / 20 = log(A) - log(Amax)
// dbfs / 20 + log(Amax) = log(A)
short dbfs_to_sample(float dbfs, bool negative_signal) {
  // todo: apply the opposite of the dbfs function
  // convert back into a short that cen be used for output
  float log_of_max = log10f(max_sample);
  float left_side;
  left_side = log_of_max + (dbfs / 20.0f);
  float answer = powf(10, left_side);
  if (negative_signal) {
    answer *= -1.0f;
  }
  // if (count % 3000 == 0) {
  //
  //   // Serial.print("dbfs = ");
  //   // Serial.println(dbfs);
  //   // Serial.print("log of max = ");
  //   // Serial.println(log_of_max);
  //   // Serial.print("left side = ");
  //   // Serial.println(left_side);
  //   // Serial.print("answer = ");
  //   // Serial.println(answer);
  // }
  short output = answer;
  return output;
}
