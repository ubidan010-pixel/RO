#include "precompiled_core.h"

#ifndef _ITF_PERLIN_NOISE_H_
#include "Core/math/PerlinNoise.h"
#endif // _ITF_PERLIN_NOISE_H_

#ifndef _ITF_MATHF32_H_
#include "core/math/Mathf32.h"
#endif _ITF_MATHF32_H_

// Ken Perlin Improved Noise (Siggraph 2002)
// http://mrl.nyu.edu/perlin/noise

namespace ITF
{

static const int permut[512] = {
                                151,160,137,91,90,15,
                                131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                                190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                                88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                                77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                                102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                                135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                                5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                                223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                                129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                                251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                                49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                                138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
                                // rinse, repeat
                                151,160,137,91,90,15,
                                131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                                190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                                88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                                77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                                102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                                135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                                5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                                223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                                129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                                251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                                49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                                138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
                               };

static inline f32 Fade(f32 t)
{
    return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}

static inline f32 Lerp(f32 t, f32 a, f32 b)
{
    return a + t * (b - a);
}

// Generates gradients at run-time on each Get() call???? UGH!
static inline f32 Grad(int hash, f32 x, f32 y, f32 z) 
{
    int h = hash & 15;                      
    float u = h<8 ? x : y,                 
        v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

////////////////////////////////////////////////////////////////////////////////////
static inline f32 ImprovedPerlinNoise(f32 x) 
{
#ifdef ASSERT_ENABLED
    f32 originalX = x;
#endif
    f32 flX = floorf(x);

    // Find the segment's starting point
    int i = int(flX) & 255;

    // Keep only the decimal part to make x relative to the line segment
    x -= flX;                                

    // Compute the interpolant using a curve
    float u = Fade(x);

    // Find the gradients at each of segment's ends
    // Why -1? I don't quite get the Grad() method!
    f32 gradientL = Grad(permut[i], x, 0.0f, 0.0f),
        gradientR = Grad(permut[i+1], x-1, 0.0f, 0.0f);

    f32 result = Lerp
    (
        u,
        gradientL,
        gradientR
    );

    ITF_ASSERT_MSG(f32_Abs(result) <= 1.0f, "Perlin is going crazy! res=%f (x=%f)", result, originalX);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
static inline f32 ImprovedPerlinNoise(f32 x, f32 y) 
{
#ifdef ASSERT_ENABLED
    f32 originalX = x;
    f32 originalY = y;
#endif

    f32 flX = floorf(x),
        flY = floorf(y);

    // Find the square in the integer grid
    int i = int(flX) & 255,
        j = int(flY) & 255;

    // Keep only the decimal part to make x relative to the line segment
    x -= flX;                                
    y -= flY;                                

    // Compute the interpolants in each dimension
    float u = Fade(x);
    float v = Fade(y);

    // Find the gradients at each of square's corners
    f32 gradientUL = Grad(permut[i]+j, x, y, 0.0f),
        gradientUR = Grad(permut[i+1]+j, x-1, y, 0.0f),
        gradientLL = Grad(permut[i]+j+1, x, y-1, 0.0f),
        gradientLR = Grad(permut[i+1]+j+1, x-1, y-1, 0.0f);

    f32 result = Lerp
    (
        v,
        Lerp
        (
            u,
            gradientUL,
            gradientUR
        ),
        Lerp
        (
            u,
            gradientLL,
            gradientLR
        )
    );

    ITF_ASSERT_MSG(f32_Abs(result) <= 1.0f, "Perlin is going crazy! res=%f (x=%f, y=%f)", result, originalX, originalY);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
static inline f32 ImprovedPerlinNoise(f32 x, f32 y, f32 z) 
{
#ifdef ASSERT_ENABLED
    f32 originalX = x;
    f32 originalY = y;
    f32 originalZ = z;
#endif

    f32 flX = floorf(x),
        flY = floorf(y),
        flZ = floorf(z);

    int i = int(flX) & 255,                  
        j = int(flY) & 255,                  
        k = int(flZ) & 255;

    x -= flX;                                
    y -= flY;                                
    z -= flZ;

    float u = Fade(x),                                
        v = Fade(y),                               
        w = Fade(z);

    int A = permut[i  ]+j, AA = permut[A]+k, AB = permut[A+1]+k, 
        B = permut[i+1]+j, BA = permut[B]+k, BB = permut[B+1]+k;

    f32 result = Lerp
    (
        w,
        Lerp
        (
            v,
            Lerp
            (
                u, 
                Grad(permut[AA  ], x  , y  , z  ),
                Grad(permut[BA  ], x-1, y  , z  )
            ),
            Lerp
            (
                u,
                Grad(permut[AB  ], x  , y-1, z  ),
                Grad(permut[BB  ], x-1, y-1, z  )
            )
        ),
        Lerp
        (
            v,
            Lerp
            (
                u,
                Grad(permut[AA+1], x  , y  , z-1),
                Grad(permut[BA+1], x-1, y  , z-1)
            ), 
            Lerp
            (
                u,
                Grad(permut[AB+1], x  , y-1, z-1),
                Grad(permut[BB+1], x-1, y-1, z-1)
            )
        )
    );

    ITF_ASSERT_MSG(f32_Abs(result) <= 1.0f, "Perlin is going crazy! res=%f (x=%f, y=%f, z=%f)", result, originalX, originalY, originalZ);
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
f32 PerlinNoise::Get(f32 x)
{
    // add the offsets
    x += m_transX;

    // loop on octaves
    f32 result = 0.f;
    f32 amplitude = 1.f;
    f32 frequency = m_frequency;
    for(int i = 0; i<m_nbOctave; ++i) 
    {
        result += ImprovedPerlinNoise(x*frequency) * amplitude;
        amplitude *= m_persistence;
        frequency *= 2.f;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
f32 PerlinNoise::Get(f32 x, f32 y)
{
    // add the offsets
    x += m_transX;
    y += m_transY;

    // loop on octaves
    f32 result = 0.f;
    f32 amplitude = 1.f;
    f32 frequency = m_frequency;
    for(int i = 0; i<m_nbOctave; ++i) 
    {
        result += ImprovedPerlinNoise(x*frequency, y*frequency) * amplitude;
        amplitude *= m_persistence;
        frequency *= 2.f;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
f32 PerlinNoise::Get(f32 x, f32 y, f32 z)
{
    // add the offsets
    x += m_transX;
    y += m_transY;
    z += m_transZ;

    // loop on octaves
    f32 result = 0.f;
    f32 amplitude = 1.f;
    f32 frequency = m_frequency;
    for(int i = 0; i<m_nbOctave; ++i) 
    {
        result += ImprovedPerlinNoise(x*frequency, y*frequency, z*frequency) * amplitude;
        amplitude *= m_persistence;
        frequency *= 2.f;
    }

    return result;
}

}