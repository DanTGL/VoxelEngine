#include "noise.h"

PerlinNoise::PerlinNoise(unsigned int seed) {
  perlin.SetSeed(seed);
  // perlin.SetFrequency(1.0);
  // perlin.SetPersistence(2.0);
}

PerlinNoise::~PerlinNoise() {}

double PerlinNoise::compute(double x, double y) {
  // return noise::ValueNoise3D(x, y, 1, seed);
  return perlin.GetValue(x, y, 1);
}

OctaveNoise::OctaveNoise(unsigned int seed, int octaves) {
  perlin.SetSeed(seed);
  // perlin.SetFrequency(1.0);
  // perlin.SetPersistence(0.5);
  perlin.SetOctaveCount(octaves);
}

OctaveNoise::~OctaveNoise() {}

double OctaveNoise::compute(double x, double y) {
  /*double amplitude = 1, frequency = 1;
  double sum = 0;

  for (int i = 0; i < sizeof(noise) / sizeof(PerlinNoise); i++) {
          sum += noise[i]->compute(x * frequency, y * frequency) * amplitude;
          amplitude *= 2.0;
          frequency *= 0.5;
  }

  return sum;*/
  return perlin.GetValue(x, y, 1);
}

CombinedNoise::CombinedNoise(Noise *noise1, Noise *noise2) {
  this->noise1 = noise1;
  this->noise2 = noise2;
}

CombinedNoise::~CombinedNoise() {
  delete[] noise1;
  delete[] noise2;
}

double CombinedNoise::compute(double x, double y) {
  return noise1->compute(x + noise2->compute(x, y), y);
}

ImprovedNoise::ImprovedNoise(Random *rnd) {
  p = (unsigned char *)malloc(sizeof(unsigned char) * 512);

  // shuffle randomly using fisher-yates
  for (int i = 0; i < 256; i++)
    p[i] = (unsigned char)i;

  for (int i = 0; i < 256; i++) {
    int j = rnd->Next(i, 256);
    unsigned char temp = p[i];
    p[i] = p[j];
    p[j] = temp;
  }
  for (int i = 0; i < 256; i++)
    p[i + 256] = p[i];
}

ImprovedNoise::~ImprovedNoise() { free(p); }

double ImprovedNoise::compute(double x, double y) {
  int xFloor = x >= 0 ? (int)x : (int)x - 1;
  int yFloor = y >= 0 ? (int)y : (int)y - 1;
  int X = xFloor & 0xFF, Y = yFloor & 0xFF;
  x -= xFloor;
  y -= yFloor;

  double u = x * x * x * (x * (x * 6 - 15) + 10); // Fade(x)
  double v = y * y * y * (y * (y * 6 - 15) + 10); // Fade(y)
  int A = p[X] + Y, B = p[X + 1] + Y;

  // Normally, calculating Grad involves a function call. However, we can
  // directly pack this table (since each value indicates either -1, 0 1) into a
  // set of bit flags. This way we avoid needing to call another function that
  // performs branching
  const int xFlags = 0x46552222, yFlags = 0x2222550A;

  int hash = (p[p[A]] & 0xF) << 1;
  double g22 = (((xFlags >> hash) & 3) - 1) * x +
               (((yFlags >> hash) & 3) - 1) * y; // Grad(p[p[A], x, y)
  hash = (p[p[B]] & 0xF) << 1;
  double g12 = (((xFlags >> hash) & 3) - 1) * (x - 1) +
               (((yFlags >> hash) & 3) - 1) * y; // Grad(p[p[B], x - 1, y)
  double c1 = g22 + u * (g12 - g22);

  hash = (p[p[A + 1]] & 0xF) << 1;
  double g21 =
      (((xFlags >> hash) & 3) - 1) * x +
      (((yFlags >> hash) & 3) - 1) * (y - 1); // Grad(p[p[A + 1], x, y - 1)
  hash = (p[p[B + 1]] & 0xF) << 1;
  double g11 =
      (((xFlags >> hash) & 3) - 1) * (x - 1) +
      (((yFlags >> hash) & 3) - 1) * (y - 1); // Grad(p[p[B + 1], x - 1, y - 1)
  double c2 = g21 + u * (g11 - g21);

  return c1 + v * (c2 - c1);
}

ImprovedOctaveNoise::ImprovedOctaveNoise(int octaves, Random *rnd) {
  this->octaves = octaves;
  for (int i = 0; i < octaves; i++) {
    baseNoise.push_back(new ImprovedNoise(rnd));
  }
}

ImprovedOctaveNoise::~ImprovedOctaveNoise() {
  while (!baseNoise.empty()) {
    delete[] baseNoise.front();
  }

  baseNoise.clear();
}

double ImprovedOctaveNoise::compute(double x, double y) {
  double amplitude = 1, frequency = 1;
  double sum = 0;
  for (int i = 0; i < octaves; i++) {
    sum += baseNoise[i]->compute(x * frequency, y * frequency) * amplitude;
    amplitude *= 2.0;
    frequency *= 0.5;
  }
  return sum;
}

ImprovedCombinedNoise::ImprovedCombinedNoise(ImprovedOctaveNoise *noise1,
                                             ImprovedOctaveNoise *noise2) {
  this->noise1 = noise1;
  this->noise2 = noise2;
}

ImprovedCombinedNoise::~ImprovedCombinedNoise() {}

double ImprovedCombinedNoise::compute(double x, double y) {
  double offset = noise2->compute(x, y);
  return noise1->compute(x + offset, y);
}
