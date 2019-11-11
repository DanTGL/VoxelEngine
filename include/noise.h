#pragma once

#include <vector>
#include <noise\noise.h>
#include "random.h"

class Noise {
public:
	virtual double compute(double x, double z) = 0;
};

class PerlinNoise : public Noise {
public:
	PerlinNoise(unsigned int seed);
	~PerlinNoise();
	double compute(double x, double y) override;
private:
	noise::module::Perlin perlin;
	unsigned int seed;
};

class OctaveNoise : public Noise {
public:
	OctaveNoise(unsigned int seed, int octaves);
	~OctaveNoise();

	double compute(double x, double y) override;

private:
	int octaves;
	noise::module::Perlin perlin;
};

class CombinedNoise : public Noise {
public:
	CombinedNoise(Noise* noise1, Noise* noise2);
	~CombinedNoise();

	double compute(double x, double y) override;

private:
	Noise* noise1;
	Noise* noise2;
};

class ImprovedNoise {
public:
	ImprovedNoise(Random* rnd);
	~ImprovedNoise();
	double compute(double x, double y);
private:
	unsigned char* p;
};

class ImprovedOctaveNoise {
public:
	ImprovedOctaveNoise(int octaves, Random* rnd);
	~ImprovedOctaveNoise();
	double compute(double x, double y);
private:
	std::vector<ImprovedNoise*> baseNoise;
	int octaves;
};

class ImprovedCombinedNoise {
public:
	ImprovedCombinedNoise(ImprovedOctaveNoise* noise1, ImprovedOctaveNoise* noise2);
	~ImprovedCombinedNoise();
	double compute(double x, double y);

private:
	ImprovedOctaveNoise* noise1;
	ImprovedOctaveNoise* noise2;
};