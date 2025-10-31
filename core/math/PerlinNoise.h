#ifndef _ITF_PERLIN_NOISE_H_
#define _ITF_PERLIN_NOISE_H_

// Borrowed from Dunia engine
// 1D version
// reference :
// http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
// 2D/3D version
// http://mrl.nyu.edu/perlin/noise ("Improved Noise" from Ken Perlin)

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

namespace ITF
{

class PerlinNoise
{
public:
    PerlinNoise() : m_frequency(1.f), m_persistence(0.4f), m_nbOctave(4), m_tmpOctaveSeed(0)
    {
        PreComputeTranslation();
    }

    PerlinNoise(f32 frequency, int nbOctave = 4, f32 persistence = 0.4f, u32 seed = u32())
        : m_frequency(frequency), m_persistence(persistence), m_nbOctave(nbOctave), m_uniRand(seed), m_tmpOctaveSeed(0)
    {
        PreComputeTranslation();
    }

    void SetFrequency(f32 freq)
    {
        m_frequency = freq;
        PreComputeTranslation();
    }

    void SetPersistence(f32 pers)
    {
        m_persistence = pers;
    }

    void SetNbOctave(i32 nbOctave)
    {
        m_nbOctave = nbOctave;
    }

    void SetSeed(u32 _seed)
    {
        m_uniRand.SetSeed(_seed);
        m_uniRand.Reset();
    }

    // returns true Perlin noise in various dimensions
    // NOTE: One- and two-dimension versions need testing
    f32 Get(f32 x);
    f32 Get(f32 x, f32 y);
    f32 Get(f32 x, f32 y, f32 z);

protected:
    f32 m_frequency;
    f32 m_persistence;
    i32 m_nbOctave;

    // items to get different noise between instances
    int m_tmpOctaveSeed; // used to produce random difference between octaves
    Seeder m_uniRand;
    // pre-calculated translations for Ken Perlin noise
    f32 m_transX;
    f32 m_transY;
    f32 m_transZ;

    void PreComputeTranslation() // precompute the random translation (used to avoid same result between several instances)
    {
        Seeder unirand = m_uniRand;
        m_transX = unirand.GetFloat()*m_frequency*1000.f;
        m_transY = unirand.GetFloat()*m_frequency*1000.f;
        m_transZ = unirand.GetFloat()*m_frequency*1000.f;
    }

    static inline f32 CosInterpolate(f32 a, f32 b, f32 t)
    {
        f32 f = (1.f - cosf(t * MTH_PI)) * 0.5f;
        return  a*(1.f-f) + b*f;
    }

    inline f32 Noise(int x)
    {
        // randomize the seed according to octave
        x += m_tmpOctaveSeed;
        x = (x<<13) ^ x;
        return ( 1.f - f32( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.f);
    }
    
    inline f32 SmoothNoise(int x)
    {
        return Noise(x)*0.5f
            +  Noise(x-1)*0.25f 
            +  Noise(x+1)*0.25f;
    }

    inline f32 InterpolatedNoise(f32 x)
    {
        int intX = int(x);
        f32 fracX = x - intX;

        f32 a = SmoothNoise(intX);
        f32 b = SmoothNoise(intX + 1);

        return CosInterpolate(a, b, fracX);
    }
};

}

#endif // _ITF_PERLIN_NOISE_H_

