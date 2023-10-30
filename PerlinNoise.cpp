#include "PerlinNoise.h"
#include <algorithm>
#include <numeric>
#include <random>

PerlinNoise::PerlinNoise()
{
    // パーリンノイズの初期化
    initializePermutationTable();
}

PerlinNoise::PerlinNoise(unsigned int seed)
{
    // シードを指定してパーリンノイズを初期化
    initializePermutationTable(seed);
}

void PerlinNoise::initializePermutationTable()
{
    // パーリンノイズのパーミュテーションテーブルを初期化
    std::iota(std::begin(perm), std::end(perm), 0);
    std::default_random_engine engine;
    std::shuffle(std::begin(perm), std::end(perm), engine);
    std::copy(std::begin(perm), std::end(perm), std::begin(perm) + 256);
}

void PerlinNoise::initializePermutationTable(unsigned int seed)
{
    // シードを使ってパーミュテーションテーブルを初期化
    std::iota(std::begin(perm), std::end(perm), 0);
    std::default_random_engine engine(seed);
    std::shuffle(std::begin(perm), std::end(perm), engine);
    std::copy(std::begin(perm), std::end(perm), std::begin(perm) + 256);
}

double PerlinNoise::fade(double t) const
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) const
{
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z) const
{
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double PerlinNoise::noise(double x, double y, double z) const
{
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;

    double p1 = lerp(u, grad(perm[AA], x, y, z), grad(perm[BA], x - 1, y, z));
    double p2 = lerp(u, grad(perm[AB], x, y - 1, z), grad(perm[BB], x - 1, y - 1, z));
    double p3 = lerp(u, grad(perm[AA + 1], x, y, z - 1), grad(perm[BA + 1], x - 1, y, z - 1));
    double p4 = lerp(u, grad(perm[AB + 1], x, y - 1, z - 1), grad(perm[BB + 1], x - 1, y - 1, z - 1));

    return lerp(w, lerp(v, p1, p2), lerp(v, p3, p4));
}
