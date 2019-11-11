#pragma once

class Random {
private:
	long seed;
	const long value = 0x5DEECE66DL;
	const long mask = (1L << 48) - 1;
public:
	Random(int seed);

	void SetSeed(int seed);
	int Next(int min, int max);
	int Next(int n);
	float NextFloat();
};