#pragma once

class PerlinNoise
{
public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);

    double noise(double x, double y, double z = 0.0) const;

private:
    unsigned char perm_[512];  // パーミュテーションテーブル
    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y, double z) const;

    void initializePermutationTable();
    void initializePermutationTable(unsigned int seed);
};

