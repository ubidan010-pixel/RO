#include "precompiled_gameplay.h"

#ifndef _ITF_TRAJECTORYPROVIDER_H_
#include "gameplay/AI/Utils/TrajectoryProvider.h"
#endif //_ITF_TRAJECTORYPROVIDER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    void TrajectoryProvider::init( const f32 _trajectoryLength, const f32 _sampleLength,
                                   const Vec2d& _pos, const Vec2d& _dir )
    {
        m_sampleLength = _sampleLength;
        u32 sampleCount = (u32)floor(_trajectoryLength / m_sampleLength);
        sampleCount += 10;  // just to be safe...

        m_samples.resize(sampleCount);
        m_samplesCursor = 0;    // we're gonna write in the whole array, so next time we write we'll start back at 0

        for (u32 i = 0; i < sampleCount; i++)
        {
            m_samples[i] = _pos - m_sampleLength * (f32)i * _dir;
        }

        m_lastPos = _pos;
        m_sampleRest = 0.0f;
    }

    void TrajectoryProvider::update( const Vec2d& _pos )
    {
        ITF_ASSERT_CRASH(m_samples.size() > 0, "must have at least 2 points, was trajectory initialized properly?");

        m_lastPos = _pos;

        Vec2d prevSample = m_samples[getSampleIndex(m_samples.size() - 1)];
        Vec2d dir = _pos - prevSample;
        f32 length = dir.norm();
        if ( length != 0.0f )
        {
            dir /= length;
        }

        while (length > m_sampleLength)
        {
            const Vec2d pos = prevSample + dir * m_sampleLength;
            m_samples[m_samplesCursor] = pos;
            m_samplesCursor++;
            if (m_samplesCursor >= m_samples.size())
            {
                m_samplesCursor = 0;
            }
            prevSample = pos;
            length -= m_sampleLength;
        }
        m_sampleRest = f32_Max( 0.0f, length );
    }

    Vec2d TrajectoryProvider::getPos( f32 _dist )
    {
        ITF_ASSERT_CRASH(m_samples.size() > 0, "must have at least 2 points, was trajectory initialized properly?");

        const u32 lastSampleIndex = getSampleIndex(m_samples.size() - 1);
        const Vec2d lastSample = m_samples[lastSampleIndex];

        if (_dist < m_sampleLength)
        {
            return Interpolate(m_lastPos, lastSample, _dist / m_sampleLength);
        }
        else
        {
            // remove what hasn't been sampled yet
            _dist -= m_sampleRest;
            ITF_ASSERT(_dist > 0);

            const u32 sampleCount = (u32)(_dist / m_sampleLength);
            ITF_ASSERT(sampleCount < m_samples.size());

            const u32 sampleIndex = m_samples.size() - sampleCount - 1;   // remove 1 to fix the real size

            const f32 rest = _dist -  sampleCount * m_sampleLength;

            return Interpolate(m_samples[getSampleIndex(sampleIndex)], m_samples[getSampleIndex(sampleIndex - 1)], rest / m_sampleLength);
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TrajectoryProvider::drawDebug( const f32 _z, const Color& _color /*= Color::white()*/, const f32 _width /*= 1.f*/, const f32 _duration /*= 0.f*/ )
    {
        ITF_ASSERT_CRASH(m_samples.size() > 0, "must have at least 2 points, was trajectory initialized properly?");

        for (u32 i = 0; i < m_samples.size() - 1; i++)
        {
            const Vec2d& p0 = m_samples[getSampleIndex(i)];
            const Vec2d& p1 = m_samples[getSampleIndex(i + 1)];
            Vec2d normal( (p1 - p0).normalize().getPerpendicular() );
            //String s;s.setTextFormat("%d", i);
            //AIUtils::drawEdge(p0, p1, normal, _z, Color::White, 0, &s);
            DebugDraw::line2D(p0, p1, _z, _color, _width, _duration);
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE
};