#pragma once

class PerlinNoise
{
public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);

    double noise(double x, double y, double z = 0.0) const;
    void initializePermutationTable();
    void initializePermutationTable(unsigned int seed);
private:
    unsigned char perm[512];  // パーミュテーションテーブル
    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y, double z) const;


};

