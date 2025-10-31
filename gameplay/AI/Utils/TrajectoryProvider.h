#ifndef _ITF_TRAJECTORYPROVIDER_H_
#define _ITF_TRAJECTORYPROVIDER_H_

namespace ITF
{
    class TrajectoryProvider
    {
    public:
        TrajectoryProvider()
            : m_sampleLength(0.1f)
            , m_samplesCursor(0)
            , m_sampleRest( 0.0f )
        {}

        void    init( const f32 _trajectoryLength, const f32 _sampleLength,
                      const Vec2d& _pos, const Vec2d& _dir );
        bbool   isInitialized() const { return m_samples.size() > 0; }
        void    update( const Vec2d& _pos );
        Vec2d   getPos( f32 _dist );
#ifdef ITF_SUPPORT_DEBUGFEATURE
        void    drawDebug( const f32 _z, const Color& _color = Color::white(), const f32 _width = 1.f, const f32 _duration = 0.f );
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:
        f32                 m_sampleLength;
        SafeArray<Vec2d>    m_samples;
        u32                 m_samplesCursor;    // next time we write, write there; next time we read, start there
        Vec2d               m_lastPos;
        f32                 m_sampleRest;

        ITF_INLINE u32 getSampleIndex( const u32 _i )
        {
            u32 index = m_samplesCursor + _i;

            if (index >= m_samples.size())
            {
                index -= m_samples.size();
            }

            return index;
        }
    };

};

#endif //_ITF_TRAJECTORYPROVIDER_H_