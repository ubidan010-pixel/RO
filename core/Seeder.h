#ifndef _ITF_SEEDER_H_
#define _ITF_SEEDER_H_

#include <time.h>

namespace ITF
{

    /////////////////////////////////////////////////////////////////////////////////////
    ///standardSeeder
    class Seeder
    {
    public:

        /// Basic seeder constructor.
        ITF_INLINE Seeder(  ) : m_seed(0), m_value(0)
        {
        }

        /// Basic seeder constructor.
        /// @param  _seed   The seed.
	    ITF_INLINE Seeder( u32 _seed )
		    : m_seed( _seed )
	    {
		    Reset();
        }
        
        /// Seeder constructor
        /// @param  _seed   The seed.
        /// @param  _value  The current value.
        ITF_INLINE Seeder( u32 _seed, u32 _value )
            : m_seed( _seed ), m_value(_value)
        {
        }

        /// Get the state of the seed.
        /// @param [in,out] _seed   If non-null, the seed.
        /// @param [in,out] _value  If non-null, the value.
        ITF_INLINE void getState( u32* _seed, u32* _value ) const
        {
            if(_seed)   *_seed  = m_seed;
            if(_value)  *_value = m_value;
        }

        /// Reset the seeder to it's original value, with an optional seed
        ITF_INLINE void Reset()
        {
            m_value = m_seed;
        }

        // _rangeMin <= random number < _rangeMax
        static u32 GetUnseededU32(u32 _rangeMin, u32 _rangeMax)
        {
            u32 u = u32((double)rand() / (u64(RAND_MAX) + 1) * (_rangeMax - _rangeMin) + _rangeMin);
            return u;
        }

        /// Get a random u32
        /// @return The random u32.
	    ITF_INLINE u32 GetU32()
	    {
		    m_value = m_value * 0x0019660Du + 0x3C6EF35Fu;
		    return m_value;
        }

        /// Get a random u32 between 0 and _max
        /// @return The random u32.
        ITF_INLINE u32 GetU32( u32 _max )
        {
            return GetU32() % (_max + 1);
        }

        /// Get a random u32 between _min and _max. (min < max)
        /// @return The random u32.
        ITF_INLINE u32 GetU32( u32 _min, u32 _max )
        {
            if (_min == _max)
                return _min;

            ITF_ASSERT(_min < _max);
            return _min + GetU32(_max - _min);
        }

        /// Get a random float between 0.0f and  1.0f.
        /// @return The random value.
	    ITF_INLINE f32 GetFloat()
	    {
		    u32 val = GetU32();
		    val >>= 9;
		    val |= 0x3F800000;
		    return *(f32*)&val - 1.0f;
	    }

        /// Get a random float between 0 and _maxmin.
        /// @param  _max    The maximum.
        /// @return The random value.
	    ITF_INLINE f32 GetFloat( f32 _maxmin )
	    {
		    return GetFloat() * _maxmin;
	    }

        /// Get a random float between _min and _max. (min < max)
        /// @param  _min    The minimum.
        /// @param  _max    The maximum.
        /// @return The random value.
	    ITF_INLINE f32 GetFloat( f32 _min, f32 _max )
        {
            ITF_ASSERT(_min <= _max);
		    return GetFloat() * (_max - _min) + _min;
	    }

        /// Stress tests to check if everything is alright.
        static void stressTest()
        {
#if defined(ASSERT_ENABLED)
            u32 seed = (u32)time(NULL);
            Seeder test(seed);

            u32 referenceResult     = test.GetU32();
            for(u32 i = 0; i < 10; ++i)
            {
                test.Reset();
                ITF_ASSERT(test.GetU32() == referenceResult);
            }

            const u32 limitMinU32   = 1000;
            const u32 limitMaxU32   = 1500;
            const f32 limitMin      = 42.0f;
            const f32 limitMax      = 666.0f;

            const u32 iterCount     = 20000;

            for(u32 i = 0; i < iterCount; ++i)
            {
                const u32 ceiledU32 = test.GetU32(limitMaxU32);
                ITF_ASSERT(ceiledU32 <= limitMaxU32);

                const u32 flooredU32 = test.GetU32(limitMinU32, limitMaxU32);
                ITF_ASSERT(flooredU32 >= limitMinU32 && flooredU32 <= limitMaxU32);

                const f32 fracF32 = test.GetFloat();
                ITF_ASSERT(fracF32 >= 0.0f && fracF32 <= 1.0f);

                const f32 ceiledF32 = test.GetFloat(limitMax);
                ITF_ASSERT(ceiledF32 >= 0.0f && ceiledF32 <= limitMax);

                const f32 flooredF32 = test.GetFloat(limitMin, limitMax);
                ITF_ASSERT(flooredF32 >= limitMin && flooredF32 <= limitMax);
            }
#endif // _DEBUG
        }
        
        ITF_INLINE void SetSeed(u32 _seed)
        {
            m_seed = _seed;
        }

        ITF_INLINE static Seeder& getSharedSeeder()
        {
#if !defined(ITF_CTR) && !defined(ITF_WII)
                static Seeder s_sharedSeeder = (u32)time(NULL);
#else
                static Seeder s_sharedSeeder = (u32)0; //Todo_CTR
#endif
                return s_sharedSeeder;
        }

    private:
	    u32	m_seed;
	    u32	m_value;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //The following seeder was published by G. Marasaglia in sci.stat.math
    class MarsagliaSeeder
    {
    public:

        /// Basic seeder constructor.
        ITF_INLINE MarsagliaSeeder(  )
        {
            reset();
        }

        /// Basic seeder constructor.
        /// @param  _seed   The seed.
        ITF_INLINE MarsagliaSeeder( u32 _seed1, u32 _seed2 )
        {
            reset(_seed1, _seed2);
        }



        /// Reset the seeder to it's original value, with an optional seed
        ITF_INLINE void reset(u32 _seedOffset1=0, u32 _seedOffset2=0)
        {
            m_y = 362436069;
            m_z = 521288629+_seedOffset2;
            m_x = 123456789+_seedOffset1;
        }

        /// Get a random u32
        /// @return The random u32.
        ITF_INLINE u32 GetU32()
        {
            m_x ^= m_x << 16;
            m_x ^= m_x >> 5;
            m_x ^= m_x << 1;
            u32 t = m_x;
            m_x = m_y;
            m_y = m_z;
            m_z = t ^ m_x ^ m_y;
            u32 result = (m_z>>1) | (((m_z>>7)&1)<<31); //modification to original Marsaglia formula
            return result;
        }

        /// Get a random u32 between 0 and _max
        /// @return The random u32.
        ITF_INLINE u32 GetU32( u32 _max )
        {
            return GetU32() % (_max + 1);
        }

        /// Get a random u32 between _min and _max. (min < max)
        /// @return The random u32.
        ITF_INLINE u32 GetU32( u32 _min, u32 _max )
        {
            if (_min == _max)
                return _min;

            ITF_ASSERT(_min < _max);
            return _min + GetU32(_max - _min);
        }

        /// Get a random float between 0.0f and  1.0f.
        /// @return The random value.
        ITF_INLINE f32 GetFloat()
        {
            u32 val = GetU32();
            val >>= 9;
            val |= 0x3F800000;
            return *(f32*)&val - 1.0f;
        }

        /// Get a random float between 0 and _maxmin.
        /// @param  _max    The maximum.
        /// @return The random value.
        ITF_INLINE f32 GetFloat( f32 _maxmin )
        {
            return GetFloat() * _maxmin;
        }

        /// Get a random float between _min and _max. (min < max)
        /// @param  _min    The minimum.
        /// @param  _max    The maximum.
        /// @return The random value.
        ITF_INLINE f32 GetFloat( f32 _min, f32 _max )
        {
            ITF_ASSERT(_min <= _max);
            return GetFloat() * (_max - _min) + _min;
        }

    private:
        u32	m_z, m_y;
        u32 m_x;
    };


} // namespace ITF

#endif // _ITF_SEEDER_H_
