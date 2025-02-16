#include "random.h"

Random::Random(int seed) {
	SetSeed(seed);
}

void Random::SetSeed(int seed) {
	this->seed = (seed ^ value) & mask;
}

int Random::Next(int min, int max) {
	return min + Next(max - min);
}

int Random::Next(int n) {
	if ((n & -n) == n) { // i.e., n is a power of 2
		seed = (seed * value + 0xBL) & mask;
		long raw = (long)((unsigned long)seed >> (48 - 31));
		return (int)((n * raw) >> 31);
	}
	
	int bits, val;
	do {
		seed = (seed * value + 0xBL) & mask;
		bits = (int)((unsigned long)seed >> (48 - 31));
		val = bits % n;
	} while (bits - val + (n - 1) < 0);
	return val;
}

float Random::NextFloat() {
	seed = (seed * value + 0xBL) & mask;
	int raw = (int)((unsigned long)seed >> (48 - 24));
	return raw / ((float)(1 << 24));
}
