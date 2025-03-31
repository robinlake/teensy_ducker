extern short max_sample;
extern short min_sample;
float sample_to_dbfs(short sample);
short dbfs_to_sample(float dbfs, bool negative_signal);
